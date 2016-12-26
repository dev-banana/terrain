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
    is_godmod = GODMOD ;
    is_gravity = GRAVITY ;
    is_collision = COLLISION ;
    is_mouse = MOUSE ;
    showConsole = SHOW_DEBUG ;

    //widgets des options
    widgets_options = create_widgets();
    widgets_timer = create_widgets();
    widgets_infos = create_widgets();
    b_is_godmod = is_godmod?1:0 ;
    b_is_gravity = is_gravity?1:0 ;
    b_is_collision = is_collision?1:0 ;
    b_is_journee = horloge.is_journee?1:0 ;
    b_is_texture = map.use_texture()?1:0 ;
    b_is_shadows = map.use_shadow()?1:0 ;
    b_is_meteo = sky.change_weather()?1:0 ;
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
    std::cout << std::endl << "commandes principales : TAB" << std::endl<< std::endl ;
    
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
    release_widgets( widgets_options ) ;
    release_widgets( widgets_timer ) ;
    release_widgets( widgets_infos ) ;
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

    horloge.timerBegin() ;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;

    compute_input() ;

    if( !is_godmod && is_gravity && SDL_GetTicks() - horloge.timeBefore > 5 )
    {
        camera.gravite( map, GRAVITY_VALUE/15 ) ;
    }
    camera.set_collision( is_collision && !is_godmod ) ;
    
    sky.animate( horloge ) ;
    animation.animate( map, horloge ) ;


    sky.draw( camera ) ;
    map.draw( camera, sky.getSun() ) ;
    animation.draw( camera, sky.getSun() ) ;

    
    horloge.timerEnd() ;

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
    default_color( widgets_options, Black() ) ;
    begin( widgets_options, 0, 4, 2 ) ;

        if( showConsole )
        {
            begin_line( widgets_options ) ; label( widgets_options, ",----------------------------------------------------------------------------------------------------------------------------," ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                                                         OPTIONS                                                            |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, ",------------------------------------------------------------+---------------------------------------------------------------," ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                                                            |                                                               |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                        "); button(widgets_options, "GOD-MODE ", b_is_godmod); label(widgets_options, "                         |                   COMMANDES CLAVIER                           |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                                                            |                                                               |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|         "); button(widgets_options, "COLLISION ", b_is_collision) ; label(widgets_options, "          |        "); button(widgets_options, "GRAVITE ", b_is_gravity) ; label(widgets_options, "          |     AVANCE : Z    RECULE : S     GAUCHE : Q    DROITE : D     |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                                                            |                                                               |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|    " ) ; button(widgets_options, "JOURNEES ", b_is_journee) ; label(widgets_options, "       (  SPEED : " ) ; button(widgets_options, " - ", b_speedM) ; label(widgets_options, "   |  " ) ; button(widgets_options, " + ", b_speedP) ; label(widgets_options, "  )        |       SAUTER : ESPACE        ZOOM-IN : +    ZOOM-OUT : -      |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                                                            |                                                               |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|    " ) ; button(widgets_options, "METEO ", b_is_meteo) ; label(widgets_options, "   (  " ) ; button(widgets_options, "mauvais temps  ", b_weatherM) ; label(widgets_options, "  |  " ) ; button(widgets_options, "beau temps ", b_weatherP) ; label(widgets_options, "  )    |                                                               |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                                                            |    GOD-MODE : D      LOCK-MOUSE : CAPSLOCK (ou clic gauche)   |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|    " ) ; button(widgets_options, "TEXTURE ", b_is_texture) ; label(widgets_options, "                                              |                                                               |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                                                            |         TEXTURES : T      OMBRES : O      JOURNEE : J         |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|    " ) ; button(widgets_options, "OMBRES ", b_is_shadows) ; label(widgets_options, "                                               |                                                               |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                                                            |                                                               |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "|                                                            |                                                               |" ) ;
            begin_line( widgets_options ) ; label( widgets_options, "'------------------------------------------------------------+---------------------------------------------------------------'" ) ;
        }
    end( widgets_options ) ;
    drawWidgets( widgets_options, window_width(), window_height() ) ;
        
    default_color( widgets_timer, Blue() ) ;
    begin( widgets_timer, 0, 0 ) ;
        label( widgets_timer, "  |                                                                                                                          |" ) ;
        begin_line( widgets_timer ) ; label( widgets_timer, "  |   [ DAY %03d - %02d:%02d::%02d (%s) ]", horloge.day, horloge.hour, horloge.minutes%60, horloge.secondes%60, (sky.weather() < 0.7)?"nuageux":((sky.weather() < 0.9)?"couvert":"soleil ") ) ;
        label( widgets_timer, "    -    performances : GPU ( %02dms %03dus )   -   CPU ( %02dms %03dus )  -   FPS ( %03d )  |", horloge.milli_gpu, horloge.micro_gpu, horloge.milli_cpu, horloge.micro_cpu, horloge.fps ) ;
        begin_line( widgets_timer ) ; label( widgets_timer, "  '---------------------,                                                                              ,---------------------'" ) ;
    end( widgets_timer ) ;
    drawWidgets( widgets_timer, window_width(), window_height() ) ;

    default_color( widgets_infos, Red() ) ;
    begin( widgets_infos, 0,2 ) ;
        begin_line( widgets_infos ) ; label( widgets_infos, "   ,-----------,        |       day speed : x%05d  -  meteo (%s) : %0.1f  -  god-mode : %s            |", (int)horloge.speed, sky.change_weather()?"ON ":"OFF", sky.weather(), is_godmod?"ON ":"OFF" ) ;
        begin_line( widgets_infos ) ; label( widgets_infos, "   |    TAB    |        '------------------------------------------------------------------------------'" ) ;
        begin_line( widgets_infos ) ; label( widgets_infos, "   '-----------'" ) ;
    end( widgets_infos ) ;
    drawWidgets( widgets_infos, window_width(), window_height() ) ;


    is_godmod = b_is_godmod ;
    is_gravity = b_is_gravity ;
    is_collision = b_is_collision ;
    sky.change_weather( b_is_meteo ) ;
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
        if( horloge.speed < 1 )
            horloge.speed = 1 ;
        b_speedM = 0 ;
    }
    if( b_speedP ){
        horloge.speed *= 2 ;
        if( horloge.speed > 16000 )
            horloge.speed = 16000 ;
        b_speedP = 0 ;
    }
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
    
    if( ks[SDL_SCANCODE_ESCAPE] )
        quit() ;
    else 
    if( key_state('z') )        // Z-Q-S-D  -> Déplacement
        camera.avance( map ) ; 
    if( key_state('s') )
        camera.recule( map ) ; 
    if( key_state('q') )
        camera.gauche( map ) ; 
    if( key_state('d') )
        camera.droite( map ) ; 
    if( key_state(' ') )        // SPACE  -> Déplacement
        camera.jump( map, 1 ) ;
    if( ks[SDL_SCANCODE_KP_PLUS] ) // + -> ZOOM IN
        camera.zoomIn() ;
    if( ks[SDL_SCANCODE_KP_MINUS] ) // - -> ZOOM OUT
        camera.zoomOut() ;

    if( ks[SDL_SCANCODE_TAB] )  //  TAB -> Console Debug
        release[0] = 1 ;
    else if(release[0] == 1 )
    {
        release[0] = 0 ;
        showConsole = !showConsole ;

        if( !showConsole )
        {
            is_mouse = true ;
            horloge.is_journee = ( b_is_journee == 1 )?true:false ;
        }
        else
        {
            is_mouse = false ;
            horloge.is_journee = false ;
        }
    }
    if( ks[SDL_SCANCODE_CAPSLOCK] ) // CAPSLOCK -> Active MOUSE
        release[1] = 1 ;
    else if(release[1] == 1 )
    {
        release[1] = 0 ;
        is_mouse = !is_mouse ;
    }

    if( key_state('t') )    // T -> ACTIVE TEXTURE
        release[2] = 1 ;
    else if(release[2] == 1 )
    {
        release[2] = 0 ;
        map.use_texture( !map.use_texture() ) ;
        b_is_texture = map.use_texture()?1:0 ;
    }
    if( key_state('o') )    // Y -> ACTIVE OMBRES
        release[3] = 1 ;
    else if(release[3] == 1 )
    {
        release[3] = 0 ;
        map.use_shadow( !map.use_shadow() ) ;
        b_is_shadows = map.use_shadow()?1:0 ;
    }

    if( key_state('j') )    // U -> ACTIVE JOURNEE
        release[4] = 1 ;
    else if(release[4] == 1 )
    {
        release[4] = 0 ;
        b_is_journee = (b_is_journee==0)?1:0 ;
        if( !showConsole )
            horloge.is_journee = !horloge.is_journee ;
    }

    if( key_state('g') )    // G -> ACTIVE GODMOD
        release[9] = 1 ;
    else if(release[9] == 1 )
    {
        release[9] = 0 ;
        is_godmod = !is_godmod ;
        b_is_godmod = is_godmod?1:0 ;
    }

    if( is_mouse && !(mb & SDL_BUTTON(1)) ) //tenir le bouton enfoncé désactive la rotation caméra
    camera.rotate( mx, my );

    map.update_chunks( camera ) ;
}