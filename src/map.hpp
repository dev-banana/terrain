#ifndef _MAP_HPP
#define _MAP_HPP


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
#include "camera_fps.h"


class Chunk : public Mesh
{
    public:

        int v_count ;
        Mesh boundingbox ;

        std::map<vec3, std::vector<int>> cubeToVertex ;


        /*
            GLSL
        */
        GLuint vao ; // vertex array object
        GLuint v_buffer ;
        GLuint n_buffer ;

        void draw() ;
        bool is_visible( const Transform& ) ;
        bool check_collision( const Point, vec3 ) ;
        bool remove_vertex( const Point ) ;

        Chunk(){}
    	~Chunk() ;
} ;


class Map
{
	private:

        /*
            GLSL
        */
        GLuint program ; // shader principal
        GLuint program_depth ; // depth program


        /*
            DEPTH
        */

        bool use_shadows ;
        Camera_FPS camera_depth ;
        GLuint color_sampler;
        GLuint color_buffer;
        GLuint depth_buffer;
        GLuint framebuffer;
        int framebuffer_width;
        int framebuffer_height;      


		/*
			HEIGHTMAP
		*/
		Image heightmap ;
		unsigned int map_width  ;
        unsigned int map_height ;
        Point posDepart ;


        bool use_text ; 
        GLuint heightmap_text ;

        /*
			PRIMITIVE
		*/
		Mesh mesh_cube ;


		/*
			CHUNKS
		*/
		std::vector<Chunk> chunks ;
        std::vector<Chunk*> chunks_visibles ;

        int nbBloc ;
        bool is_digging ;

		bool INIT = true ;

        void build_shadow_map( Light& ) ;
        void draw_shadow_map() ;


    public:

        Map(){}
        ~Map(){}

        void init() ;
        void draw( Camera_FPS&, Light& ) ;
        void release() ;


        void update_chunks( Camera_FPS& ) ;

        int getSizeMax(){ return std::max(map_width, map_height ) ;}

        Chunk * get_chunk( const unsigned int ) ;
        int get_ichunk( const Point ) ;

        void build( Camera_FPS& ) ;
        void dig( Camera_FPS& ) ;

        bool digging(){ return is_digging ; }
        void digging( bool d ){ is_digging = d ; }

        bool use_texture(){ return use_text ; }
        void use_texture( bool t ){ use_text = t ; }

        bool use_shadow(){ return use_shadows ; }
        void use_shadow( bool s ){ use_shadows = s ; }

        // verifie que le monde est valide
        operator bool() const 
        { 
            return INIT ; 
        }
} ;

#endif
