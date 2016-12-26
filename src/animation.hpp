/**
* \file animation.hpp
* \brief Fichier de déclaration de la classe Animation
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#ifndef ANIMATION_HPP
#define ANIMATION_HPP
//------------------------------
//------------------------------
//libs
//gkit
//tp2
#include "config.hpp"
#include "map.hpp"
//------------------------------


/**
* \class Animation
* \brief classe qui gère des objets supplémentaires
*
*   Cette classe regroupe les objets (personnages, décors..) supplémentaires.
*   Elle permet de travailler sur des petites animations sans interférer avec la carte
*   ou le ciel.
*
*/
class Animation
{
	protected:

		bool INIT = true ;    //!< true si initialisation = OK


        /*==============================
                    GLSL
        ==============================*/  
        GLuint program ;    //!< Shader principal

        GLuint vao ;    //!< Vertex Array Object
        GLuint v_buffer ;   //!< Vertex Buffer
        GLuint n_buffer ;   //!< Normal Buffer
        GLuint t_buffer ;   //!< Texture Buffer
   


        /*==============================
                    PRIMITIVE
        ==============================*/ 
        Mesh mesh_panda ;   //!< Un objet représentant un panda
        int v_count ;   //!< Le nombre de vertex
        GLuint texture_panda ;  //!< Sa texture assiciée
        Point pos_panda ;   //!< Sa position actuelle


	public:

        /**
        * \brief Constructeur de la classe Animation
        */
        Animation(){}
        /**
        * \brief Destructeur de la classe Animation
        */
        ~Animation(){}


        
        void init() ;
        void release() ;
        void animate( Map&, Timer ) ;
        void draw( Camera_FPS&, Light& ) ;
              

        /**
        * \brief Surcharge de l'opérateur de test
        * \return TRUE si l'initialisation s'est bien passée.
        */
        operator bool() const 
        { 
            return INIT ; 
        }
} ;

#endif //ANIMATION_HPP
