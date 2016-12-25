/**
* \file sky.cpp
* \brief Fichier d'implémentation des classes Sky et Light
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#include "sky.hpp"
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
void Sky::init( int size )
{
    dome_radius = size*3 ; //3x la taille de la heightmap

    /*
        SUN
    */
    sun_height = dome_radius ;
    sun.position = vec3( 0.0, -sun_height, 0.0 ) ;
    sun.ambient = vec3( 0.0f, 0.0f, 0.0f ) ;
    sun.diffuse = vec3( 1.0f, 1.0f, 1.0f ) ;
    sun.specular = vec3( 0.08f, 0.08f, 0.08f ) ;

    meteo = 1.0 ;
    changeMeteo = -1 ;
    

    /*
        OBJET SKYDOME
    */
    mesh_sphere = read_mesh( SPHERE_OBJ ) ;
    v_count = mesh_sphere.vertex_count() ;

    clouds1_text = read_texture( 1, "data/sky/clouds1.tga" ) ;
    clouds2_text = read_texture( 2, "data/sky/clouds2.tga" ) ;
    moon_text = read_texture( 3, "data/sky/moon.tga" ) ;
    sun_text = read_texture( 4, "data/sky/sun.tga" ) ;
    sky1_text = read_texture( 5, "data/sky/sky1.tga" ) ;
    sky2_text = read_texture( 6, "data/sky/sky2.tga" ) ;



    /*
        SHADER init
    */
    program = read_program( PATH_SKY_SHADER ) ;
    program_print_errors(program) ;


    glGenBuffers(1, &v_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer);

    // dimensionne le buffer actif sur array_buffer, l'alloue et l'initialise avec les positions des sommets de l'objet
    glBufferData(GL_ARRAY_BUFFER,
        /* length */ mesh_sphere.vertex_buffer_size(),
        /* data */ mesh_sphere.vertex_buffer(),
        /* usage */ GL_STATIC_DRAW);
    // GL_STATIC_DRAW decrit l'utilisation du contenu du buffer. dans ce cas, utilisation par draw, sans modifications

    // on recommence avec les normales
    glGenBuffers(1, &n_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, n_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh_sphere.normal_buffer_size(), mesh_sphere.normal_buffer(), GL_STATIC_DRAW);

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

}



/*==============================
            QUIT
==============================*/

/**
* \brief libère les objets et textures de la classe
*/
void Sky::release()
{
    glDeleteTextures( 1, &clouds1_text ) ;
    glDeleteTextures( 2, &clouds2_text ) ;
    glDeleteTextures( 3, &moon_text ) ;
    glDeleteTextures( 4, &sun_text ) ;
    glDeleteTextures( 5, &sky1_text ) ;
    glDeleteTextures( 6, &sky2_text ) ;
    mesh_sphere.release() ;
    release_program( program ) ;
}


/*==============================
            DRAW
==============================*/

/**
* \brief anime le ciel, change la météo
* \param[in] minutes : une indication du temps écoulé
*/
void Sky::animate( int minutes )
{
    float angle = minutes*360/(24*60)-90 ;
    float angle_r = 2 * M_PI * (angle / 360) ;
    sun.position.x = sun_height * cos( angle_r ) ;
    sun.position.y = sun_height * sin( angle_r ) ;

    int step = 6 ; //change la météo toutes les 6h

    if( (int)minutes/60 % step == 0 && (changeMeteo == -1 || changeMeteo != (int)minutes/60 ) )
    {
        changeMeteo = (int)minutes/60 ;

        if( meteo <= 0.01 )
            meteo = 0.1 ;
        else if( meteo == 0.9 )
            meteo = 1 ;
        else if( (int)(meteo*10) % 2 == 0 ){
            meteo -= 0.2 ;
            if( meteo == 0.0 )
                meteo = 0.01 ;
        }
        else{
            meteo += 0.2 ;
            if( meteo > 0.9 )
                meteo = 1 ;
        }
    }
}

/**
* \brief Dessine les différents éléments du ciel
* \param[in] camera : la caméra fps
*/
void Sky::draw( Camera_FPS& camera )
{
    glCullFace(GL_FRONT);
    glUseProgram( program ) ;

    
    Transform model = Identity()*Scale( dome_radius, dome_radius, dome_radius ) ;
    Transform view = camera.view() ;
    Transform projection = camera.projection(window_width(), window_height()) ;
    Transform mv = view * model ;
    Transform mvp = projection * mv ;

    /*
        GL DATA
    */
    program_uniform( program, "observer", camera.position() ) ;
    program_uniform( program, "light.position", sun.position ) ;
    program_uniform( program, "light.diffuse", sun.diffuse ) ;
    program_uniform( program, "light.ambient", sun.ambient ) ;
    program_uniform( program, "light.specular", sun.specular ) ;
    program_uniform( program, "mvMatrix", mv ) ;
    program_uniform( program, "mvpMatrix", mvp ) ;
    program_uniform( program, "normalsMatrix", mv.normal() ) ;
    program_uniform( program, "weather", meteo ) ;

    
    program_use_texture( program, "clouds1", 1, clouds1_text ) ;
    program_use_texture( program, "clouds2", 2, clouds2_text ) ;
    program_use_texture( program, "moon", 3, moon_text ) ;
    program_use_texture( program, "sun", 4, sun_text ) ;
    program_use_texture( program, "sky1", 5, sky1_text ) ;
    program_use_texture( program, "sky2", 6, sky2_text ) ;

    glBindVertexArray( vao ) ;
    glDrawArrays(DRAW_TYPE, 0, v_count );
    
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0) ;
}


/*==============================
        GETTERs SETTERs
==============================*/

/**
* \brief Accès au soleil
* \return Light le soleil
*/
Light& Sky::getSun()
{
    return sun ;
}

/**
* \brief Donne la météo actuelle
* \return la valeur entre 0 et 1 de la météo
*/
float Sky::weather()
{
    return meteo ;
}

/**
* \brief Défini la valeur météo
* \param[in] _w : la nouvelle valeur de la météo
*/
void Sky::weather( float _w )
{ 
    meteo = _w ;
    if( meteo < 0.01 )
        meteo = 0.01 ;
    else if( meteo > 1.0 )
        meteo = 1.0 ;
}