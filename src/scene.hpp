#ifndef _SCENE_HPP
#define _SCENE_HPP


#include "config.hpp"


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include "window.h"
#include "wavefront.h"  // pour charger un objet au format .obj
#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "image_io.h"
#include "text.h"
#include "widgets.h"


#include "camera_fps.h"
#include "map.hpp"
#include "sky.hpp"
#include "animation.hpp"


class Scene
{
	private:

        Camera_FPS camera ;

        GLuint time ;
        std::chrono::high_resolution_clock::time_point cpu_start ;
        int ellapse ;
        int timeOfDay ;
        float secondes ;
        float minutes ;
        float hour ;
        float speed ;

	Map map ;

        Sky sky ;

        Animation animation ;

        bool is_gravity ;
        bool is_journee ;
        bool is_collision ;
        bool is_mouse ;
        
        Widgets widgets;
        int b_is_gravity ;
        int b_is_collision ;
        int b_is_journee ;
        int b_is_texture;
        int b_is_shadows;
        int b_weatherM ;
        int b_weatherP ;
        int b_speedM ;
        int b_speedP ;

        bool showConsole ;
        void drawInterface() ;

	public:

        Scene(){}
        ~Scene(){}

        int init() ;
        int draw() ;
        void quit() ;

        void compute_input() ;
} ;

#endif
