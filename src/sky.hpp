/**
* \file sky.hpp
* \brief Fichier de déclaration des classes Sky et Light
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#ifndef SKY_HPP
#define SKY_HPP
//------------------------------
//------------------------------
//libs
//gkit
#include "window.h"
#include "program.h"
#include "mesh.h"
#include "wavefront.h"
#include "uniforms.h"
#include "texture.h"
//tp2
#include "config.hpp"
#include "timer.hpp"
#include "camera_fps.hpp"
//------------------------------


/**
* \class Light
* \brief classe qui permet définir une lumière
*
*   Une lumière a différentes caractéristiques.
*   Cette classe permet entre autre de définir le type d'éclairage souhaité.
*
*/
class Light
{
    public:
        vec3 position ;    //!< La position dans le monde de la lumière
        vec3 ambient ;    //!< La couleure ambiente de la lumière
        vec3 diffuse ;    //!< La couleure diffuse de la lumière
        vec3 specular ;    //!< La couleure spéculaire de la lumière
} ;

/**
* \class Sky
* \brief classe qui permet définir une skybox animée
*
*   Le ciel est un skydome, dont la couleur dépend de la météo et de l'heure.
*   Il est composé de nuages, d'étoiles, d'images pour la lune et le soleil.
*
*/
class Sky
{
	protected:

		bool INIT = true ;    //!< true si initialisation = OK


        /*==============================
                    GLSL
        ==============================*/
        GLuint program ;    //!< Shader principal
        GLuint vao ;    //!< Vertex Array Objects
        GLuint v_buffer ;   //!< Vertex Buffer
        GLuint n_buffer ;   //!< Normal Buffer


        /*==============================
                    PRIMITIVE
        ==============================*/ 
        Mesh mesh_sphere ;  //!< Définition d'un objet Sphere
        int v_count ;   //!< Nombre de vertex de l'objet sphere
        int dome_radius ;   //!< Rayon du dome


        /*==============================
                    SUN
        ==============================*/ 
        Light sun ; //!< La lumière principale (lumière du soleil)
        int sun_height ;    //!< La hauteur du soleil dans le ciel


        /*==============================
                    MÉTÉO
        ==============================*/ 
        bool is_meteo ; //!< (Des)Active le changement de météo
        float meteo ;   //!< La météo (0 = mauvais, 1 = ensoleillé)
        int changeMeteo ;   //!< Variable pour savoir si il faut changer la météo


        /*==============================
                    TEXTURES
        ==============================*/ 
        GLuint clouds1_text ;   //!< Texture des nuages par beau temps
        GLuint clouds2_text ;   //!< Texture des nuages par mauvais temps  
        GLuint moon_text ;  //!< Texture pour dessiner la lune
        GLuint sun_text ;   //!< Texture pour colorer le soleil en fonction de l'heure
        GLuint sky1_text ;  //!< Texture pour colorer le ciel en fonction de l'heure
        GLuint sky2_text ;  //!< Texture pour colorer l'horrizon' en fonction de l'heure




	public:

        Sky(){}
        ~Sky(){}

        void init( int ) ;
        void draw( Camera_FPS& ) ;
        void animate( Timer ) ;
        void release() ;


        /*==============================
                GETTERs SETTERs
        ==============================*/ 
        Light& getSun() ;
        float weather() ;
        void weather( float ) ;
        bool change_weather() ;
        void change_weather( bool ) ;


        /**
        * \brief Surcharge de l'opérateur de test
        * \return TRUE si l'initialisation s'est bien passée.
        */
        operator bool() const 
        { 
            return INIT ; 
        }
} ;

#endif //SKY_HPP
