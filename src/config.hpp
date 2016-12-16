#ifndef _CONFIG_HPP
#define _CONFIG_HPP


#define GRAVITY_VALUE 9.81
#define GRAVITY false
#define COLLISION true
#define JOURNEE true
#define SHADOWS true
#define TEXTURE true
#define MOUSE true
#define SHOW_DEBUG true

#define DAY_SPEED 2000

#define CHUNK_SIZE 64
#define MAX_HEIGHT 40

#define CAMERA_POS ((Point)Point(0, 10, 0))

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

#endif
