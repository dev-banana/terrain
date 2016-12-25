/**
* \file scene.cpp
* \brief Fichier d'implémentation de la classe Scene
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#include "scene.hpp"
//------------------------------



/*==============================
            INIT
==============================*/   

/**
* \brief Initialisation
*
*   Chargement des propriétés de la scène
*   Appelle l'initialisation des classes Map, Sky, Animation
*
*/
int Scene::init()
{
    
    std::cout << std::endl << " MINECRAFT - Anna(K) Benneton&Bergmann " << std::endl  << std::endl ;


    std::cout << "Map init ... "  << std::endl;
    map.init() ;
    if( !map ) 
    {
        std::cout << "\terror" << std::endl ;
        return -1 ;   
    }

    std::cout << std::endl << "Sky init ... " << std::endl ;
    sky.init( map.getSizeMax() ) ;
    if( !sky ) 
    {
        std::cout << "\terror" << std::endl ;
        return -1 ;   
    }

    std::cout << std::endl << "Animation init ... " << std::endl ;
    animation.init() ;
    if( !animation ) 
    {
        std::cout << "\terror" << std::endl ;
        return -1 ;   
    }


    std::cout << std::endl << "Scene init : " << std::endl ;


    horloge.init() ;

    //observer
    camera.position( CAMERA_POS ) ;
    camera.set_zmax( map.getSizeMax()*6 ) ;


    //options gameplay
    is_gravity = GRAVITY ;
    is_collision = COLLISION ;
    is_mouse = MOUSE ;
    showConsole = SHOW_DEBUG ;

    //widgets des options
    widgets= create_widgets();
    b_is_gravity = is_gravity?1:0 ;
    b_is_collision = is_collision?1:0 ;
    b_is_journee = horloge.is_journee?1:0 ;
    b_is_texture = map.use_texture()?1:0 ;
    b_is_shadows = map.use_shadow()?1:0 ;
    b_weatherM = 0 ;
    b_weatherP = 0 ;
    b_speedM = 0 ;
    b_speedP = 0 ;


    /*
        CONFIGURATION DU PIPELINE
        
        pour obtenir une image correcte lorsque l'on dessine plusieurs triangles, il faut choisir lequel conserver pour chaque pixel...
        on conserve le plus proche de la camera, celui que l'on voit normalement. ce test revient a considerer que les objets sont opaques.
    */
    glClearColor(0.2f, 0.2f, 0.2f, 1.f) ;
    glClearDepth( 1.f ) ;  // profondeur par defaut
    glDepthFunc( GL_LESS ) ;
    glEnable( GL_DEPTH_TEST ) ;
    glFrontFace( GL_CCW ) ;
    glEnable( GL_FRAMEBUFFER_SRGB ) ; 


    std::cout << std::endl << "initialisation : OK " << std::endl << std::endl ;
    std::cout << std::endl << "commandes principales : " << std::endl<< std::endl ;
    std::cout << "  ROTATION SOURIS : CAPS" << std::endl ;
    std::cout << "  DEPLACEMENTS : Z-Q-S-D " << std::endl ;
    std::cout << "  SAUTER : ESPACE " << std::endl ;
    std::cout << "  ZOOMER : - ET +" << std::endl << std::endl ;
    std::cout << "  OPTIONS et DEBUGS :  TAB" << std::endl << std::endl ;

    return 0 ;
}



/*==============================
            QUIT
==============================*/

/**
* \brief libère les objets de la classe
*/
void Scene::quit()
{
    map.release();
    sky.release() ;
    animation.release() ;
    horloge.release() ;
    release_widgets( widgets ) ;
}


/*==============================
        INPUTS
==============================*/

/**
* \brief Gère les actions utilisateur (clavier, souris)
*/
static int release[20] ; //!< tableau des touches enfoncées (en attente d'être relachée)
void Scene::compute_input()
{
    int mx, my ;
    int dx, dz = 0;
    unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
    const Uint8 *ks = SDL_GetKeyboardState(NULL);

    camera.set_collision( is_collision ) ;
    
    if( ks[SDL_SCANCODE_ESCAPE] )
        quit() ;
    else 
    if( key_state('z') )
        camera.avance( map ) ; 
    if( key_state('s') )
        camera.recule( map ) ; 
    if( key_state('q') )
        camera.gauche( map ) ; 
    if( key_state('d') )
        camera.droite( map ) ; 
    if( key_state(' ') )
        camera.jump( map, 1 ) ;
    if( ks[SDL_SCANCODE_KP_PLUS] )
        camera.zoomIn() ;
    if( ks[SDL_SCANCODE_KP_MINUS] )
        camera.zoomOut() ;

    if( ks[SDL_SCANCODE_TAB] )
        release[0] = 1 ;
    else if(release[0] == 1 )
    {
        release[0] = 0 ;
        showConsole = !showConsole ;
    }
    if( ks[SDL_SCANCODE_CAPSLOCK] )
        release[1] = 1 ;
    else if(release[1] == 1 )
    {
        release[1] = 0 ;
        is_mouse = !is_mouse ;
    }
    if( key_state('t') )
        release[2] = 1 ;
    else if(release[2] == 1 )
    {
        release[2] = 0 ;
        map.use_texture( !map.use_texture() ) ;
        b_is_texture = map.use_texture()?1:0 ;
    }
    if( key_state('y') )
        release[3] = 1 ;
    else if(release[3] == 1 )
    {
        release[3] = 0 ;
        map.use_shadow( !map.use_shadow() ) ;
        b_is_shadows = map.use_shadow()?1:0 ;
    }


    if( is_mouse && !(mb & SDL_BUTTON(1)) ) //tenir le bouton enfoncé désactive la rotation caméra
    camera.rotate( mx, my );

    map.update_chunks( camera ) ;
}

/*==============================
            DRAW
==============================*/

/**
* \brief Dessine la scène
* \return 1 si tous c'est bien passé
*
*   Boucle principale, elle appelle les différentes méthodes d'affichage des classes
*   Elle s'occupe des mise à jour en fonction du temps écoulé
*
*/
int Scene::draw( )
{

    glBeginQuery( GL_TIME_ELAPSED, horloge.gpu );
    horloge.cpu_start= std::chrono::high_resolution_clock::now();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;

    compute_input() ;

    if( is_gravity && SDL_GetTicks() - horloge.ellapse > 10 )
    {
        camera.gravite( map, GRAVITY_VALUE/15 ) ;
        horloge.ellapse = SDL_GetTicks() ;
    }


    sky.animate( horloge.minutes ) ;
    animation.animate( map, horloge.minutes ) ;


    sky.draw( camera ) ;
    map.draw( camera, sky.getSun() ) ;
    animation.draw( camera, sky.getSun() ) ;


    if( horloge.is_journee ){
        horloge.timeOfDay = (int)(SDL_GetTicks()*horloge.speed)%(1000*60*60*24) ; 
        horloge.secondes = horloge.timeOfDay/1000 ;
        horloge.minutes = horloge.secondes/60 ;
        horloge.hour = horloge.minutes/60 ;
    }
    

    drawInterface() ;
  
    return 1 ;
}

/**
* \brief Dessine la console de debug
*
*   Affichage des informations et des boutons pour changer l'état de certaines valeurs.
*
*/
void Scene::drawInterface()
{
    std::chrono::high_resolution_clock::time_point cpu_stop= std::chrono::high_resolution_clock::now();
    long long int cpu_time= std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_stop - horloge.cpu_start).count();
    

    glEndQuery( GL_TIME_ELAPSED ) ;
    std::chrono::high_resolution_clock::time_point wait_start= std::chrono::high_resolution_clock::now();
    GLint64 gpu_time = 0 ;
    glGetQueryObjecti64v( horloge.gpu, GL_QUERY_RESULT, &gpu_time ) ;
    std::chrono::high_resolution_clock::time_point wait_stop= std::chrono::high_resolution_clock::now();
    long long int wait_time= std::chrono::duration_cast<std::chrono::nanoseconds>(wait_stop - wait_start).count();

    
    int milli_gpu = (int) ( gpu_time / 1000000 ) ;
    int micro_gpu = (int) ( (gpu_time / 1000) % 1000 );

    int milli_cpu = (int) ( cpu_time / 1000000 ) ;
    int micro_cpu = (int) ( (cpu_time / 1000) % 1000 );

    int fps = (int) 1/( (cpu_time+gpu_time+wait_time) / 1000000000.0 ) ;


    begin(widgets, 1, 0); 

    begin_line(widgets); 
    label( widgets, " ROTATATION SOURIS : CAPS ; DEPLACEMENTS : Z-Q-S-D ; JUMP : ESPACE ; ZOOM : +/-  ; TEXTURE : T" ) ;
    
    if( showConsole )
    {
        begin_line(widgets); 
        label( widgets, " HIDE OPTIONS : TAB" ) ;




        begin_line(widgets); 
        label(widgets, "  _________________________") ;
        begin_line(widgets); 
        label(widgets, "  | GPU : %02dms %03dus      |", milli_gpu, micro_gpu ) ;
        begin_line(widgets); 
        label(widgets, "  | CPU : %02dms %03dus      |", milli_cpu, micro_cpu ) ;
        begin_line(widgets); 
        label(widgets, "  | FPS : %03d             |", fps ) ;
        begin_line(widgets); 
        label(widgets, "  |                       |") ;
        begin_line(widgets); 
        label(widgets, "  | GRAVITE :  " ) ; button(widgets, "  ", b_is_gravity) ; label(widgets, "   |" ) ;
        begin_line(widgets); 
        label(widgets, "  |                       |") ;
        begin_line(widgets); 
        label(widgets, "  | COLLISION :  " ) ; button(widgets, "  ", b_is_collision) ; label(widgets, " |" ) ;
        begin_line(widgets); 
        label(widgets, "  |                       |") ;
        begin_line(widgets); 
        label(widgets, "  | HEURE : %02dh %02dm %02ds   |", (int)horloge.hour, (int)horloge.minutes%60, (int)horloge.secondes%60 ) ;
        begin_line(widgets); 
        label(widgets, "  |                       |") ;
        begin_line(widgets); 
        label(widgets, "  | JOURNEE :  " ) ; button(widgets, "  ", b_is_journee) ; label(widgets, "   |" ) ;
        begin_line(widgets); 
        label(widgets, "  |" ) ; 
            button(widgets, "-  ", b_speedM) ;
            label(widgets, "x%04d", (int)horloge.speed ) ;  
            button(widgets, "+  ", b_speedP) ;
        label(widgets, "|") ;
        begin_line(widgets); 
        label(widgets, "  |                       |") ;
        begin_line(widgets); 
        label(widgets, "  | METEO :               |") ;
        begin_line(widgets); 
        label(widgets, "  |  " ) ; 
            button(widgets, " - ", b_weatherM) ;
            label(widgets, "%0.1f", sky.weather() ) ;  
            button(widgets, " + ", b_weatherP) ;
        label(widgets, "|") ;
        begin_line(widgets); 
        label(widgets, "  |                       |") ;
        begin_line(widgets); 
        label(widgets, "  | TEXTURE :  " ) ; button(widgets, "  ", b_is_texture) ; label(widgets, "   |" ) ;
        begin_line(widgets); 
        label(widgets, "  |                       |") ;
        begin_line(widgets); 
        label(widgets, "  | SHADOWS :  " ) ; button(widgets, "  ", b_is_shadows) ; label(widgets, "   |" ) ;
        begin_line(widgets); 
        label(widgets, "  |_______________________|") ;


        is_gravity = b_is_gravity ;
        is_collision = b_is_collision ;
        horloge.is_journee = b_is_journee ;
        map.use_texture( b_is_texture ) ;
        map.use_shadow( b_is_shadows ) ;

        if( b_weatherM ){
            sky.weather( sky.weather()-0.05 ) ;
            b_weatherM = 0 ;
        }
        if( b_weatherP ){
            sky.weather( sky.weather()+0.05 ) ;
            b_weatherP = 0 ;
        }
        if( b_speedM ){
            horloge.speed /= 2 ;
            b_speedM = 0 ;
        }
        if( b_speedP ){
            horloge.speed *= 2 ;
            b_speedP = 0 ;
        }

    }
    else
    {
        begin_line(widgets); 
        label( widgets, " SHOW OPTIONS : TAB" ) ;
    }
    end(widgets);
    
    drawWidgets(widgets, window_width(), window_height());
}