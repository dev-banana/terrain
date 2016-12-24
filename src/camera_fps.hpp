/**
* \file camera_fps.hpp
* \brief Fichier de déclaration de la classe Camera_FPS
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#ifndef CAMERA_FPS_HPP
#define CAMERA_FPS_HPP
//------------------------------
//------------------------------
//libs
//gkit
#include "mat.h"
//tp2
#include "config.hpp"
class Map ;
//------------------------------


/**
* \class Camera_FPS
* \brief caméra à la 1ere personne
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*
*   Cette classe simule une caméra "humaine" avec une vue à la 1ere personne.
*   Elle permet de d'effectuer des déplacements tel que avancer/reculer, gauche/droite, sauter
*   Elle gère également la gravité et les collisions.
*
*/
class Camera_FPS
{
    protected:

        /*==============================
                    DÉPLACEMENTS
        ==============================*/ 
        vec3 pos ;  //!< Position de la caméra
        vec3 target ;   //!< Point observé

        Vector direction ;  //!< Vecteur de la direction actuelle
        Vector vecFront ;   //!< Vecteur de direction (Z dans le repère caméra)
        Vector vecUp ;  //!< Vecteur de direction (Y dans le repère caméra)
        Vector vecRight ;   //!< Vecteur de direction (X dans le repère caméra)
        float pitch, yaw ;  //!< Rotation de la caméra sur elle même

        /*==============================
                        VUE
        ==============================*/
        float zmin, zmax ;  //!< Profondeur de la caméra pour la projection
        float fov ; //!< Ouverture de la caméra (Field of Vue)


        /*==============================
                    PARAMETRES
        ==============================*/ 
        bool active_collision ; //!< Active ou Désactive les collisions
        vec3 size_cam ; //!< Boite englobante de la caméra (pour les collisions)
        float speed ;   //!< Vitesse de déplacement

        void update() ;
        bool canMove( Map&, Point ) ;


    public:

        Camera_FPS( ) ;

        /*==============================
                    DÉPLACEMENTS
        ==============================*/ 
        void avance( Map& ) ;
        void recule( Map& ) ;
        void gauche( Map& ) ;
        void droite( Map& ) ;
        void rotate( float, float ) ;
        void jump( Map&, float ) ;
        void gravite( Map&, float ) ;
        void zoomIn() ;
        void zoomOut() ;
        

        /*==============================
                        VUE
        ==============================*/
        Transform view() const ;
        Transform projection( const float, const float ) const ;
        Transform projection_ortho( const float, const float ) const ;


        /*==============================
                GETTERs SETTERs
        ==============================*/
        Point position() ;
        void position( const Point ) ;
        Point lookat() ;
        void lookat( const Point& ) ;
        void set_zmin( float ) ;
        void set_zmax( float ) ;
        void set_collision( bool ) ;
        Point getForward( int ) ;
};

#endif
