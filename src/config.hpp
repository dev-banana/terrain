/**
* \file config.hpp
* \brief Fichier de déclaration des variables globales utilisées à l'initialisation
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#ifndef CONFIG_HPP
#define CONFIG_HPP
//------------------------------
//------------------------------

#define GODMOD false
#define SHOW_DEBUG false

#define GRAVITY_VALUE 9.81
#define GRAVITY true
#define COLLISION true

#define SHADOWS true
#define MOUSE true

#define JOURNEE true
#define DAY_SPEED 1000
#define START_HOUR 6
#define USE_METEO true

#define CHUNK_SIZE 64
#define MAX_HEIGHT 40
#define LOAD_WIDTH 50
#define LOAD_HEIGHT 50

#define CAMERA_POS ((Point)Point(0, 10, 0))

#define TEXTURE true
#define PATH_HEIGHTMAP ((char *)"data/terrain/hp.jpg")
#define PATH_TEXTURE ((char *)"data/terrain/hp_texture.png")

#define PATH_MAP_SHADER ((char *)"src/shaders/map.glsl")
#define PATH_SKY_SHADER ((char *)"src/shaders/sky.glsl")
#define PATH_SHADOWS_SHADER ((char *)"src/shaders/depth.glsl")

#define CUBE_OBJ ((char *)"data/cube.obj")
#define SPHERE_OBJ ((char *)"data/sphere.obj")

#define FRAMEBUFFER_WIDTH 512
#define FRAMEBUFFER_HEIGHT 512

#define DRAW_TYPE GL_TRIANGLES //GL_LINES

#endif //CONFIG_HPP
