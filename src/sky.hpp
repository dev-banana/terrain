#ifndef _SKY_HPP
#define _SKY_HPP


#include "config.hpp"


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "window.h"
#include "program.h" //shaders
#include "mesh.h"
#include "wavefront.h"  // pour charger un objet au format .obj
#include "uniforms.h"
#include "texture.h"
#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "image_io.h"


#include "camera_fps.h"


class Light
{
    public:
        vec3 position ;
        vec3 ambient ;
        vec3 diffuse ;
        vec3 specular ;
} ;

class Sky
{
	private:

        /*
            GLSL
        */
        GLuint program ; // shader program
        GLuint vao ;
        GLuint v_buffer ;
        GLuint n_buffer ;


        /*
            PRIMITIVE
        */
        Mesh mesh_sphere ;
        int v_count ;
        int dome_radius ;


        /*
            SUN
        */
        Light sun ;
        int sun_height ;

        float meteo ;
        int changeMeteo ;


        /*
            TEXTURES
        */
        GLuint clouds1_text ;
        GLuint clouds2_text ;
        GLuint moon_text ;
        GLuint sun_text ;
        GLuint sky1_text ;
        GLuint sky2_text ;


		bool INIT = true ;


	public:

        Sky(){}
        ~Sky(){}

        void init( int ) ;
        void draw( Camera_FPS& ) ;
        void animate( int time ) ;
        void release() ;

        Light& getSun(){ return sun ; }
        float weather(){ return meteo ; }
        void weather( float _w ) ;

        // verifie que le monde est valide
        operator bool() const 
        { 
            return INIT ; 
        }
} ;

#endif
