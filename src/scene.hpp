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
        bool is_gravity ;    //!< activation de la gravité
        bool is_collision ;    //!< activation des collisions caméra
        bool is_mouse ;    //!< activation de la rotation caméra avec la souris
        

        /*==============================
                DEBUG ET ACTIONS
        ==============================*/
        bool showConsole ;    //!< Affiche la console avec les informations de debug
        Widgets widgets;    //!< Liste des widgets de debug
        int b_is_gravity ;    //!< Bouton pour (des)activer la gravité
        int b_is_collision ;    //!< Bouton pour (des)activer les collisions
        int b_is_journee ;    //!< Bouton pour (des)activer les journees
        int b_is_texture;    //!< Bouton pour (des)activer les textures
        int b_is_shadows;    //!< Bouton pour (des)activer les ombres
        int b_weatherM ;    //!< Bouton pour rendre la météo plus mauvaise
        int b_weatherP ;    //!< Bouton pour améliorer la météo
        int b_speedM ;    //!< Bouton pour baisser la vitese des journées
        int b_speedP ;    //!< Bouton pour augmenter la vitesse des journées


        void drawInterface() ;


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

        void compute_input() ;
} ;

#endif //SCENE_HPP
