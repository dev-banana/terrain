/**
* \file animation.cpp
* \brief Fichier d'implémentation de la classe Animation
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#include "animation.hpp"
//------------------------------



/*==============================
            INIT
==============================*/   

/**
* \brief Initialisation
*
*   Chargement des textures et des objets
*
*/
void Animation::init()
{

    Mesh mesh_panda = read_mesh( "data/panda.obj" );
    v_count = mesh_panda.vertex_count() ;
    texture_panda = read_texture( 1, "data/texture_panda.jpg" ) ;
    pos_panda = Point(-3,10,3);

    /*
        SHADER init
    */
    program = read_program( "src/shaders/animation.glsl" ) ;
    program_print_errors(program) ;


    glGenBuffers(1, &v_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer);

    // dimensionne le buffer actif sur array_buffer, l'alloue et l'initialise avec les positions des sommets de l'objet
    glBufferData(GL_ARRAY_BUFFER,
        /* length */ mesh_panda.vertex_buffer_size(),
        /* data */ mesh_panda.vertex_buffer(),
        /* usage */ GL_STATIC_DRAW);
    // GL_STATIC_DRAW decrit l'utilisation du contenu du buffer. dans ce cas, utilisation par draw, sans modifications

    // on recommence avec les normales
    glGenBuffers(1, &n_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, n_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh_panda.normal_buffer_size(), mesh_panda.normal_buffer(), GL_STATIC_DRAW);

    // on recommence avec les textures
    glGenBuffers(1, &t_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, t_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh_panda.texcoord_buffer_size(), mesh_panda.texcoord_buffer(), GL_STATIC_DRAW);


    // creation d'un vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // recuperer l'identifiant de l'attribut : cf in vec3 position; dans le vertex shader
    GLint attribute= glGetAttribLocation(program, "position");
    if(attribute < 0)
        INIT = false ;

    // re-selectionne vertex buffer pour configurer les positions
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
    // format et organisation des donnees dans le vertex buffer selectionne sur array_buffer,
    glVertexAttribPointer(attribute, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, /* offset */ 0);
    glEnableVertexAttribArray(attribute);

    // on recommence pour les normales
    attribute= glGetAttribLocation(program, "normal");
    if(attribute < 0)
        INIT = false ;

    // re-selectionne normal_buffer pour configurer les normales
    glBindBuffer(GL_ARRAY_BUFFER, n_buffer);
    glVertexAttribPointer(attribute, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, /* offset */ 0);  // in vec3 normal;
    glEnableVertexAttribArray(attribute);


    // on recommence pour les textures
    attribute= glGetAttribLocation(program, "texture");
    if(attribute < 0)
        INIT = false ;

    // re-selectionne normal_buffer pour configurer les normales
    glBindBuffer(GL_ARRAY_BUFFER, t_buffer);
    glVertexAttribPointer(attribute, 3, GL_FLOAT, GL_FALSE, /* stride */ 0, /* offset */ 0);  // in vec3 texture;
    glEnableVertexAttribArray(attribute);
}



/*==============================
            QUIT
==============================*/

/**
* \brief libère les objets de la classe
*/
void Animation::release()
{
    mesh_panda.release() ;
    release_program(program) ;
    glDeleteBuffers(1, &v_buffer) ;
    glDeleteBuffers(1, &n_buffer) ;
    glDeleteBuffers(1, &t_buffer) ;
    glDeleteVertexArrays(1, &vao) ;
}


/*==============================
            DRAW
==============================*/

/**
* \brief anime les objets
* \param[in] map : la carte utile pour les collisions
* \param[in] timer : une indication du temps écoulé
*/
void Animation::animate( Map& map, Timer timer)
{
    vec3 sizePanda = vec3( 5, 3, 3 ) ;
    Point newP ;
    newP.x = pos_panda.x ;
    newP.y = pos_panda.y - GRAVITY_VALUE/15 ;
    newP.z = pos_panda.z ;

    bool collide = false ;
    int i_chunk = map.get_ichunk( newP ) ;
    if( i_chunk >= 0 ){
        collide = map.get_chunk( i_chunk )->check_collision( newP, sizePanda ) ;
    }
    if( !collide )
    {
        pos_panda.x = newP.x ;
        pos_panda.y = newP.y ;
        pos_panda.z = newP.z ;
    }


    collide = false ;

    newP.x = pos_panda.x + ((double) rand() / (RAND_MAX)) + 0.05 ;
    newP.y = pos_panda.y ;
    newP.z = pos_panda.z + ((double) rand() / (RAND_MAX)) + 0.05 ;

    i_chunk = map.get_ichunk( newP ) ;
    if( i_chunk >= 0 ){
        collide = map.get_chunk( i_chunk )->check_collision( newP, sizePanda ) ;
    }
    if( !collide )
    {
        pos_panda.x = newP.x ;
        pos_panda.z = newP.z ;
    }
}

/**
* \brief Dessine les différentes animations
* \param[in] camera : la caméra fps
* \param[in] light : la lumière principale
*/
void Animation::draw( Camera_FPS& camera, Light& light )
{

    glUseProgram(program);
    glBindVertexArray( vao ) ;


    /* MAJ */
    Transform model = Identity()*Translation(Vector(pos_panda))*Scale(2,2,2)*RotationX(5) ;
    Transform view = camera.view() ;
    Transform projection = camera.projection(window_width(), window_height()) ;
    Transform mv = view * model ;
    Transform mvp = projection * mv ;


    /* GL_DATA */
    program_uniform( program, "observer", camera.position() ) ;
    program_uniform( program, "light.position", light.position ) ;
    program_uniform( program, "light.diffuse", light.diffuse ) ;
    program_uniform( program, "light.ambient", light.ambient ) ;
    program_uniform( program, "light.specular", light.specular ) ;
    program_uniform( program, "mMatrix", model ) ;
    program_uniform( program, "mvpMatrix", mvp ) ;

    program_use_texture( program, "texture_panda", 1, texture_panda ) ;


    glDrawArrays(DRAW_TYPE, 0, v_count );
    
    glBindVertexArray(0);
    glUseProgram(0) ;
}