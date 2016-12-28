/**
* \file scene.hpp
* \brief Fichier de déclaration de la classe Scene
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#ifndef SCENE_HPP
#define SCENE_HPP
//------------------------------
//------------------------------
//libs
#include <chrono>
//gkit
#include "widgets.h"
//ui
//tp2
#include "config.hpp"
#include "camera_fps.hpp"
#include "map.hpp"
#include "sky.hpp"
#include "animation.hpp"
#include "timer.hpp"
//------------------------------


/**
* \class Scene
* \brief classe qui permet définir une scène complète
*
*       Cette classe fait le lien et organise les différentes classes entre elles.
*       La scène regroupe la carte, le ciel, une caméra et les différentes animations.
*
*/
class Scene
{
    protected:

        Timer horloge ;    //!< gestion du temps
        Camera_FPS camera ;    //!< Camera principale
        Map map ;    //!< Le terrain
        Sky sky ;    //!< La skybox pour le ciel
        Animation animation ;    //!< Les objets animés


        /*==============================
                    GAMEPLAY
        ==============================*/
        bool is_godmod ;    //!< activation du God Mode
        bool is_gravity ;    //!< activation de la gravité
        bool is_collision ;    //!< activation des collisions caméra
        bool is_mouse ;    //!< activation de la rotation caméra avec la souris
        

        /*==============================
                DEBUG ET ACTIONS
        ==============================*/
        bool showConsole ;    //!< Affiche la console avec les informations de debug
        
        Widgets widgets_options ;    //!< Boutons et paramètres
        Widgets widgets_timer ;    //!< Indication des différents timers
        Widgets widgets_infos ;    //!< Indication diverse (speed, météo...)
        
        int b_is_godmod ;    //!< Bouton pour (des)activer le God Mode
        int b_is_gravity ;    //!< Bouton pour (des)activer la gravité
        int b_is_collision ;    //!< Bouton pour (des)activer les collisions
        int b_is_journee ;    //!< Bouton pour (des)activer les journees
        int b_is_texture ;    //!< Bouton pour (des)activer les textures
        int b_is_shadows ;    //!< Bouton pour (des)activer les ombres
        int b_is_meteo ;    //!< Bouton pour (des)activer les changements climatiques
        int b_weatherM ;    //!< Bouton pour rendre la météo plus mauvaise
        int b_weatherP ;    //!< Bouton pour améliorer la météo
        int b_speedM ;    //!< Bouton pour baisser la vitese des journées
        int b_speedP ;    //!< Bouton pour augmenter la vitesse des journées
        int b_camSpeedM ;    //!< Bouton pour baisser la vitese de déplacement
        int b_camSpeedP ;    //!< Bouton pour augmenter la vitesse de déplacement


        void compute_input() ;
        void drawInterface() ;
        void update_gameplay() ;


	public:
        /**
        * \brief Constructeur de la classe Scene
        */
        Scene(){}
        /**
        * \brief Destructeur de la classe Scene
        */
        ~Scene(){}

        int init() ;
        int draw() ;
        void quit() ;
} ;

#endif //SCENE_HPP
