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

    //observer
    camera.position( CAMERA_POS ) ;
    camera.set_zmax( map.getSizeMax()*6 ) ;

    //durée des journée
    glGenQueries(1, &time) ;
    speed = DAY_SPEED ;
    timeOfDay = 0 ;
    ellapse = 0 ;
    hour = 0 ;
    minutes = 0 ;
    secondes = 0 ;

    //options gameplay
    is_gravity = GRAVITY ;
    is_journee = JOURNEE ;
    is_collision = COLLISION ;
    is_mouse = MOUSE ;
    showConsole = SHOW_DEBUG ;

    //widgets des options
    widgets= create_widgets();
    b_is_gravity = is_gravity?1:0 ;
    b_is_collision = is_collision?1:0 ;
    b_is_journee = is_journee?1:0 ;
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
    // glCullFace( GL_FRONT_AND_BACK ) ;
    // glEnable( GL_CULL_FACE ) ;

    // glEnable(GL_FOG) ;
    // GLfloat fogcolor[4] = {0.5, 0.5, 0.5, 1} ;
    // GLint fogmode = GL_EXP ;
    // glFogi (GL_FOG_MODE, fogmode) ;
    // glFogfv(GL_FOG_COLOR, fogcolor) ;
    // glFogf(GL_FOG_DENSITY, 0.35) ;
    // glFogf(GL_FOG_START, 1.0) ;
    // glFogf(GL_FOG_END, 5.0) ;


    std::cout << std::endl << "initialisation : OK " << std::endl << std::endl ;
    std::cout << std::endl << "commandes principales : " << std::endl<< std::endl ;
    std::cout << "  ROTATION SOURIS : activer : SHIFT+CAPS - desactiver : CAPS " << std::endl ;
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
    glDeleteQueries(1, &time);
    release_widgets( widgets ) ;
}


/*==============================
        INPUTS
==============================*/

/**
* \brief Gère les actions utilisateur (clavier, souris)
*/
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
    if( ks[SDL_SCANCODE_W] ){
        camera.avance( map ) ;
    }
    if( ks[SDL_SCANCODE_S] ){
        camera.recule( map ) ;
    }
    if( ks[SDL_SCANCODE_A] ){
        camera.gauche( map ) ;
    }
    if( ks[SDL_SCANCODE_D] ){
        camera.droite( map ) ;
    }
    if( ks[SDL_SCANCODE_SPACE] ){
        camera.jump( map, 1 ) ;
    }
    if( ks[SDL_SCANCODE_KP_PLUS] ){
        camera.zoomIn() ;
    }
    if( ks[SDL_SCANCODE_KP_MINUS] ){
        camera.zoomOut() ;
    }
    if( ks[SDL_SCANCODE_LSHIFT] && ks[SDL_SCANCODE_TAB] ){
        showConsole = false ;
    }
    else if( ks[SDL_SCANCODE_TAB] ){
        showConsole = true ;
    }
    if( ks[SDL_SCANCODE_LSHIFT] && ks[SDL_SCANCODE_CAPSLOCK] ){
        is_mouse = true ;
    }
    else if( ks[SDL_SCANCODE_CAPSLOCK] ){
        is_mouse = false ;
    }
    if( ks[SDL_SCANCODE_LSHIFT] && ks[SDL_SCANCODE_RETURN] ){
        map.build( camera ) ;
    }
    else if( ks[SDL_SCANCODE_RETURN] && !map.digging()){
        map.digging(true) ;
        map.dig( camera ) ;
        map.digging(false) ;
    }

    if( ks[SDL_SCANCODE_LSHIFT] && ks[SDL_SCANCODE_T] ){
        map.use_texture( false ) ;
        b_is_texture = 0 ;
    }
    else if( ks[SDL_SCANCODE_T] ){
        map.use_texture( true ) ;
        b_is_texture = 1 ;
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

    glBeginQuery( GL_TIME_ELAPSED, time );
    cpu_start= std::chrono::high_resolution_clock::now();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;

    compute_input() ;

    if( is_gravity && SDL_GetTicks() - ellapse > 10 )
    {
        camera.gravite( map, GRAVITY_VALUE/15 ) ;
        ellapse = SDL_GetTicks() ;
    }


    sky.animate( minutes ) ;
    animation.animate( map, minutes ) ;


    sky.draw( camera ) ;
    map.draw( camera, sky.getSun() ) ;
    animation.draw( camera, sky.getSun() ) ;


    if( is_journee ){
        timeOfDay = (int)(SDL_GetTicks()*speed)%(1000*60*60*24) ; 
        secondes = timeOfDay/1000 ;
        minutes = secondes/60 ;
        hour = minutes/60 ;
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
    long long int cpu_time= std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_stop - cpu_start).count();
    

    glEndQuery( GL_TIME_ELAPSED ) ;
    std::chrono::high_resolution_clock::time_point wait_start= std::chrono::high_resolution_clock::now();
    GLint64 gpu_time = 0 ;
    glGetQueryObjecti64v( time, GL_QUERY_RESULT, &gpu_time ) ;
    std::chrono::high_resolution_clock::time_point wait_stop= std::chrono::high_resolution_clock::now();
    long long int wait_time= std::chrono::duration_cast<std::chrono::nanoseconds>(wait_stop - wait_start).count();

    
    int milli_gpu = (int) ( gpu_time / 1000000 ) ;
    int micro_gpu = (int) ( (gpu_time / 1000) % 1000 );

    int milli_cpu = (int) ( cpu_time / 1000000 ) ;
    int micro_cpu = (int) ( (cpu_time / 1000) % 1000 );

    int fps = (int) 1/( (cpu_time+gpu_time+wait_time) / 1000000000.0 ) ;


    begin(widgets, 1, 0); 

    begin_line(widgets); 
    label( widgets, " ROTATATION SOURIS : (SHIFT)+CAPS ; DEPLACEMENTS : Z-Q-S-D ; JUMP : ESPACE ; ZOOM : +/-  ; TEXTURE : (SHIFT)+T" ) ;
    
    if( showConsole )
    {
        begin_line(widgets); 
        label( widgets, " HIDE OPTIONS : SHIFT+TAB" ) ;




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
        label(widgets, "  | HEURE : %02dh %02dm %02ds   |", (int)hour, (int)minutes%60, (int)secondes%60 ) ;
        begin_line(widgets); 
        label(widgets, "  |                       |") ;
        begin_line(widgets); 
        label(widgets, "  | JOURNEE :  " ) ; button(widgets, "  ", b_is_journee) ; label(widgets, "   |" ) ;
        begin_line(widgets); 
        label(widgets, "  |" ) ; 
            button(widgets, "-  ", b_speedM) ;
            label(widgets, "x%04d", (int)speed ) ;  
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
        is_journee = b_is_journee ;
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
            speed /= 2 ;
            b_speedM = 0 ;
        }
        if( b_speedP ){
            speed *= 2 ;
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