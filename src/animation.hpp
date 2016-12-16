#ifndef _ANIMATION_HPP
#define _ANIMATION_HPP


#include "config.hpp"


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include "window.h"
#include "program.h" //shaders
#include "mesh.h"
#include "wavefront.h"  // pour charger un objet au format .obj
#include "uniforms.h"
#include "texture.h"
#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "image_io.h"


#include "sky.hpp"
#include "animation.hpp"
#include "camera_fps.h"
#include "map.hpp"


class Animation
{
	private:

        /*
            GLSL
        */
        GLuint program ; // shader principal

        GLuint vao ; // vertex array object
        GLuint v_buffer ;
        GLuint n_buffer ;
        GLuint t_buffer ;
        int v_count ;
   

        /*
			PRIMITIVE
		*/
        Mesh mesh_panda ;
		GLuint texture_panda ;
        Point pos_panda ;


		bool INIT = true ;

	public:

        Animation(){}
        ~Animation(){}

        void init() ;
        void draw( Camera_FPS&, Light& ) ;
        void animate( Map&, int ) ;
        void release() ;

        // verifie que le monde est valide
        operator bool() const 
        { 
            return INIT ; 
        }
} ;

#endif
