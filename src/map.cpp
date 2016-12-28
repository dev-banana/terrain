/**
* \file map.cpp
* \brief Fichier d'implémentation des classes Map et Chunk
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#include "map.hpp"
//------------------------------



/*==============================
            INIT
==============================*/   

/**
* \brief Initialisation
*
*   Chargement des textures et des objets
*   Création des Chunks   
*
*/
void Map::init()
{

    nbBloc = 0 ;
    is_digging = false ;
    use_shadows = SHADOWS ;
    use_text = TEXTURE ;


    /*
        SHADER init
    */
    program = read_program( PATH_MAP_SHADER ) ;
    program_print_errors(program) ;
    program_depth = read_program( PATH_SHADOWS_SHADER ) ;
    program_print_errors(program_depth) ;



    /*
        HEIGHT MAP
    */
    heightmap = read_image( PATH_HEIGHTMAP );
    map_width = heightmap.width() ;
    map_height = heightmap.height() ;
    heightmap_text = read_texture( 7, PATH_TEXTURE ) ;


    /*
        OBJET
    */
    mesh_cube = read_mesh( CUBE_OBJ );


    /*
        INITIALISATION DES CHUNKS
    */
    std::cout << "chunks : création des maillage et des VAOs... " << std::endl ;

    posDepart = Point( (int)-map_width/2, 0 , (int)-map_height/2 ) ;
    chunks.resize( (map_width/CHUNK_SIZE )*(map_height/CHUNK_SIZE ) ) ;

    // on ne charge que la région autour de la caméra 
    load_new_region( CAMERA_POS ) ;



    /*
        SHADOW MAP
    */
    framebuffer_width = FRAMEBUFFER_WIDTH ;
    framebuffer_height = FRAMEBUFFER_HEIGHT ;
    
    // etape 1 : creer une texture couleur...
    glGenTextures(1, &color_buffer);
    glBindTexture(GL_TEXTURE_2D, color_buffer);      
    glTexImage2D(GL_TEXTURE_2D, 0,
        GL_RGBA, framebuffer_width, framebuffer_height, 0,
        GL_RGBA, GL_FLOAT, nullptr); 

    // creer aussi une texture depth, sinon pas de zbuffer...
    glGenTextures(1, &depth_buffer);
    glBindTexture(GL_TEXTURE_2D, depth_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0,
        GL_DEPTH_COMPONENT, framebuffer_width, framebuffer_height, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // etape 2 : creer et configurer un framebuffer object
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER,  /* attachment */ GL_COLOR_ATTACHMENT0,
     /* texture */ color_buffer, /* mipmap level */ 0);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER,  /* attachment */ GL_DEPTH_ATTACHMENT,
     /* texture */ depth_buffer, /* mipmap level */ 0);
    
    // le fragment shader ne declare qu'une seule sortie, indice 0
    GLenum buffers[]= { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);
    
    // nettoyage
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  
}


/**
* \brief Calcul une région à charger
* \param[in] center : centre de la région que l'on va charger (par exemple, on charge la région autour de la caméra)
*
*   A partir d'une perimetre autour du point, on recalcul la région qui inclu tous les points des chunks de la région
*
*/
void Map::load_new_region( const Point center )
{
    if( all_load )
        return ;

    int region_xmin = ( center.x - (int)std::min( LOAD_WIDTH, (int)map_width )/2 ) ;
    int region_xmax = ( center.x + (int)std::min( LOAD_WIDTH, (int)map_width )/2 )-1 ;
    int region_ymin = ( center.z - (int)std::min( LOAD_HEIGHT, (int)map_height )/2 ) ;
    int region_ymax = ( center.z + (int)std::min( LOAD_HEIGHT, (int)map_height )/2 )-1 ;


    int idChunkMin = get_ichunk( Point(region_xmin, 0, region_ymin) ) ;
    int idChunkMax = get_ichunk( Point(region_xmax, 0, region_ymax) ) ;


    unsigned int load_xmin, load_ymin, load_xmax, load_ymax ;

    load_xmin = ( idChunkMin % ( (int)map_width/CHUNK_SIZE ) ) * CHUNK_SIZE ;
    load_ymin = ( idChunkMin / ( (int)map_width/CHUNK_SIZE ) ) * CHUNK_SIZE ;

    load_xmax = ( ( idChunkMax % ( (int)map_width/CHUNK_SIZE ) ) * CHUNK_SIZE ) + CHUNK_SIZE + 1 ;
    load_ymax = ( ( idChunkMax / ( (int)map_width/CHUNK_SIZE ) ) * CHUNK_SIZE ) + CHUNK_SIZE + 1 ;

    if( load_xmax > map_width )
        load_xmax = map_width ;

    if( load_ymax > map_height )
        load_ymax = map_height ;
    

    load_heightmap( load_xmin, load_xmax, load_ymin, load_ymax ) ;


    /*
        CREATION DES BOUNDING BOX et des VAOS pour les chunks
    */
    all_load = true ;
    for ( unsigned int i = 0; i < chunks.size(); i++)
    {   
        if( !chunks[i].INIT && chunks[i].vertex_count() > 0 )
            chunks[i].init( program ) ;
        else if( !chunks[i].INIT )
            all_load = false ;
    }
    // nettoyage
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
* \brief Chargement des vertices à partir de la heightmap
* \param[in] load_xmin : l'indice X minimum pour charger la carte
* \param[in] load_xmax : l'indice X maximum pour charger la carte
* \param[in] load_ymin : l'indice Y minimum pour charger la carte
* \param[in] load_ymax : l'indice Y maximum pour charger la carte
*
*   initialisation d'une region entre load_min et load_max
*
*/
void Map::load_heightmap( unsigned int load_xmin, unsigned int load_xmax, unsigned int load_ymin, unsigned int load_ymax )
{

    std::vector<vec3> cube_v = mesh_cube.positions() ;
    std::vector<vec3> cube_n = mesh_cube.normals() ;

    float heightValue, x_pos, y_pos ;
    //on parcourt la heightmap
    for( unsigned int i = load_xmin ; ( i < load_xmax || i < map_width ) ; i++ )
    {
        for( unsigned int j = load_ymin ; ( j < load_ymax || j < map_height ) ; j++ )
        {
            float x = i/CHUNK_SIZE ;
            float y = j/CHUNK_SIZE ;
            int index_chunk = y*(map_width/CHUNK_SIZE)+x ;

            heightValue = floor( (float)heightmap(i,j).r * MAX_HEIGHT ) ;
            x_pos = posDepart.x + i ;
            y_pos = posDepart.z + j ;

            if( !chunks[index_chunk].INIT )
            {
                //on ajoute un nouveau cube (et tout ses vertex) à une nouvelle position 
                unsigned int iv ;
                vec3 ivv = vec3(x_pos,heightValue, y_pos) ;
                for( int k = 0 ; k < mesh_cube.vertex_count() ; k++ )
                {
                    chunks[index_chunk].normal( vec3(cube_n[k].x, cube_n[k].y, cube_n[k].z) ) ;
                    iv = chunks[index_chunk].vertex( vec3(cube_v[k].x+x_pos,cube_v[k].y+heightValue, cube_v[k].z+y_pos) ) ;

                    chunks[index_chunk].cubeToVertex[ivv].push_back(iv) ;
                }

                //on comble les trous entre les hauteurs différentes
                for( unsigned int k = i-1 ; k <= i+1 ; k++ )
                {
                    for ( unsigned int l = j-1 ; l <= j+1; l++ )
                    {
                        if( k != l && k != -l && k > 0 && l > 0 && k < map_width && l < map_height )
                        {
                            int heightVoisin = posDepart.y + floor((float)heightmap(k,l).r * MAX_HEIGHT) ;
                            int diff = heightValue - heightVoisin ;

                            if( diff > 1 )
                            {
                                for( int nc = 1 ; nc < diff ; nc++ )
                                {
                                    int newHeight = heightValue-nc ;
                                    
                                    for( int k = 0 ; k < mesh_cube.vertex_count() ; k++ )
                                    {
                                        chunks[index_chunk].normal( vec3(cube_n[k].x, cube_n[k].y, cube_n[k].z) ) ;
                                        iv = chunks[index_chunk].vertex( vec3(cube_v[k].x+x_pos,cube_v[k].y+newHeight, cube_v[k].z+y_pos) ) ;

                                        ivv = vec3(x_pos,newHeight, y_pos) ;
                                        chunks[index_chunk].cubeToVertex[vec3(x_pos,newHeight, y_pos)].push_back(iv) ;
                                    }
                                }
                            }
                        }
                    }
                }   
            }
        }
    }
}


/**
* \brief Initialisation des Chunks
* \param[in] program : la référence du shader principal
*
*   Initialise les VAOs des chunks et leur bounding box
*
*/
void Chunk::init( GLuint program )
{
    INIT = true ;

    Point pmin, pmax ;
    bounds(pmin, pmax) ;

    boundingbox.vertex( Point( pmin ) ) ;
    boundingbox.vertex( Point( pmax ) ) ;
    boundingbox.vertex( Point( pmax.x, pmin.y, pmin.z ) ) ;
    boundingbox.vertex( Point( pmax.x, pmax.y, pmin.z ) ) ;
    boundingbox.vertex( Point( pmin.x, pmax.y, pmin.z ) ) ;
    boundingbox.vertex( Point( pmin.x, pmax.y, pmax.z ) ) ;
    boundingbox.vertex( Point( pmin.x, pmin.y, pmax.z ) ) ;
    boundingbox.vertex( Point( pmax.x, pmin.y, pmax.z ) ) ;


    v_count = vertex_count() ;


    glGenBuffers(1, &v_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer);

    // dimensionne le buffer actif sur array_buffer, l'alloue et l'initialise avec les positions des sommets de l'objet
    glBufferData(GL_ARRAY_BUFFER,
        /* length */ vertex_buffer_size(),
        /* data */ vertex_buffer(),
        /* usage */ GL_DYNAMIC_DRAW);
    // GL_STATIC_DRAW decrit l'utilisation du contenu du buffer. dans ce cas, utilisation par draw, sans modifications

    // on recommence avec les normales
    glGenBuffers(1, &n_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, n_buffer);
    glBufferData(GL_ARRAY_BUFFER, normal_buffer_size(), normal_buffer(), GL_STATIC_DRAW);

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
* \brief Destructeur de la classe Chunk
*/
Chunk::~Chunk()
{
    glDeleteBuffers(1, &v_buffer) ;
    glDeleteBuffers(1, &n_buffer) ;
    glDeleteVertexArrays(1, &vao) ;
}

/**
* \brief libère les objets de la classe
*/
void Map::release()
{
    mesh_cube.release();
    release_program(program) ;
    release_program(program_depth) ;
}


/*==============================
        INPUTS
==============================*/

/**
* \brief Met à jour la liste des chunks visibles par la caméra
* \param[in] camera : la caméra qui observe la scène
*/
void Map::update_chunks( Camera_FPS& camera )
{
    Transform model = Identity() ;
    Transform view = camera.view() ;
    Transform projection = camera.projection(window_width(), window_height()) ;
    Transform mvp = projection * view * model ;

    chunks_visibles.clear() ;
    for( unsigned int i = 0 ; i < chunks.size() ; i++)
    {
        if( chunks[i].INIT && chunks[i].is_visible( mvp ) )
            chunks_visibles.push_back( &chunks[i] ) ;
    }
}



/*==============================
            CALCULS
==============================*/

/**
* \brief Test la visibilité du chunk par la caméra
* \param[in] mvp : La matrice MVP de la caméra
* \return TRUE si le chunk est dans le champs caméra
*/
bool Chunk::is_visible( const Transform& mvp )
{
    std::vector<vec3> pos = boundingbox.positions() ;

    std::vector<int> compteur(6,0) ;

    for( int i = 0 ; i < 8 ; i++ )
    {
        vec4 v = mvp( vec4( Point(pos[i]) ) ) ;

        if( v.x < -v.w) compteur[0]++ ;
        if( v.x > v.w) compteur[1]++ ;
        if( v.y < -v.w) compteur[2]++ ;
        if( v.y > v.w) compteur[3]++ ;
        if( v.z < -v.w) compteur[4]++ ;
        if( v.z > v.w) compteur[5]++ ;
    }
    for(int i = 0; i < 6; i++)
        if( compteur[i] >= 8 ) return false ;

    return true ;
}

/**
* \brief Test si un point est en collision avec un chunk
* \param[in] pos : la postition du point a tester
* \param[in] box : la taille de la boite englobante du point (son épaisseure)
* \return Vrai si le point (et les points à l'interieur de sa boite) intersecte le chunk
*/
bool Chunk::check_collision( const Point pos, vec3 box )
{
    if( !INIT )
        return false ;

    for(int i = 0; i < v_count; i++)
    {
        if( m_positions[i].x > pos.x-box.x && m_positions[i].x < pos.x+box.x
        && m_positions[i].y > pos.y-box.y && m_positions[i].y < pos.y+box.y
        && m_positions[i].z > pos.z-box.z && m_positions[i].z < pos.z+box.z )
        {
            return true ;
        }
    }
    return false ;
}

/**
* \brief Construit des nouveaux blocs sur la carte
* \param[in] camera : La caméra pour savoir où construire
*/
void Map::build( Camera_FPS& camera )
{
    if( nbBloc <= 0 )
        return ;
    
    nbBloc-- ;
}

/**
* \brief Supprime les vertex d'un chunk à une certaine position
* \param[in] pos : la postition du point à enlever
* \return Vrai si des vertex ont été enlevé
*/
bool Chunk::remove_vertex( const Point pos )
{
    vec3 cube = vec3( (int)pos.x, (int)pos.y, (int)pos.z ) ;
    std::map<vec3, std::vector<int>>::iterator it;
    it=cubeToVertex.find(cube);
    if( it!=cubeToVertex.end() )
    {
        std::vector<int> vertices = it->second ;
        // if( vertices.size() > 0 )
        // {
        for( unsigned int i = 0 ; i < vertices.size() ; i++ )
        {
            vec3 v = m_positions[vertices[i]] ;
            vertex( vertices[i], vec3(v.x,v.y-1000,v.z) ) ;
            // int min = vertices[0] ;
            // int max = vertices[vertices.size()-1] - vertices[0] ;
            // m_positions.erase( m_positions.begin()+min, m_positions.begin()+max );
            // m_normals.erase( m_normals.begin()+min, m_normals.begin()+max );
        }
        // }
        // cubeToVertex.erase( it ) ;

        // v_count = vertex_count() ;       
        return true ;
    }
    return false ;
}


/**
* \brief Creuse le terrain
* \param[in] camera : pour savoir où creuser
*/
void Map::dig( Camera_FPS& camera )
{
    Point devant = camera.getForward( 1 ) ;
    bool collide = false ;
    int i_chunk = get_ichunk( devant ) ;
    bool has_dig = false ;
    if( i_chunk >= 0 && i_chunk < (int)chunks.size() ){
        has_dig = get_chunk( i_chunk )->remove_vertex( devant ) ;
    }
    
    if(has_dig)
        nbBloc++ ;
}


/*==============================
            DRAW
==============================*/

/**
* \brief Dessine un chunk
*/
void Chunk::draw()
{
    //on active le vao du chunk
    glBindVertexArray( vao ) ;
    glDrawArrays(DRAW_TYPE, 0, v_count );
    glBindVertexArray(0);
}

/**
* \brief Construit la carte de profondeur
* \param[in] light : la lumière principale
*
*   La camera pour les ombres est à la position de la lumière et regarde au centre du monde.
*
*/
void Map::build_shadow_map( Light& light )
{
    camera_depth.position( Point(light.position) ) ;

    glCullFace(GL_FRONT);
            
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer) ;
    glViewport(0, 0, framebuffer_width, framebuffer_height) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
    /* MAJ */
    Transform m = Identity() ;
    Transform lv = camera_depth.view() ;
    Transform lp = camera_depth.projection_ortho(framebuffer_width, framebuffer_height) ;
    Transform lmvp = lp * lv * m ;
    

    /* GL_DATA */
    glUseProgram( program_depth ) ;
    program_uniform(program_depth, "lmvpMatrix", lmvp) ;
    

    for( unsigned int i = 0 ; i < chunks.size() ; i++ )
    {
        if( chunks[i].INIT && chunks[i].is_visible( lmvp ) )
            chunks[i].draw( ) ;
    }  
    glBindVertexArray(0) ; 
    glUseProgram(0) ;
}


/**
* \brief Dessine la shadowMap
*/
void Map::draw_shadow_map()
{
    /* 
        montrer le resultat de la passe 1
        copie le framebuffer sur la fenetre
     */
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClearColor(1, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT );
    
    glBlitFramebuffer(
        0, 0, framebuffer_width, framebuffer_height,        // rectangle origine dans READ_FRAMEBUFFER
        0, 0, framebuffer_width, framebuffer_height,        // rectangle destination dans DRAW_FRAMEBUFFER
        GL_COLOR_BUFFER_BIT, GL_LINEAR);                        // ne copier que la couleur (+ interpoler)
}

/**
* \brief Dessine la Map
* \param[in] camera : la caméra fps
* \param[in] light : la lumière principale
*
*   Dessine les différents chunks et les ombres associées.
*   L'utilisateur peut au choix afficher la carte en mode normal ou en mode profondeur.
*
*/
void Map::draw( Camera_FPS& camera, Light& light )
{
    build_shadow_map( light ) ;

    glViewport(0, 0, window_width(), window_height()) ;

    if(key_state('c'))
    { 
        draw_shadow_map() ;
    }
    else
    {
        glCullFace(GL_BACK);
        glUseProgram(program);


        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer) ;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0) ;


        /* MAJ */
        Transform model = Identity() ;
        Transform view = camera.view() ;
        Transform projection = camera.projection(window_width(), window_height()) ;
        Transform mv = view * model ;
        Transform mvp = projection * mv ;

        Transform lv= camera_depth.view();
        Transform lp= camera_depth.projection_ortho(framebuffer_width, framebuffer_height);
        Transform lmvp= lp * lv * model;


        /* GL_DATA */
        program_uniform( program, "max_height", (float)MAX_HEIGHT);
        program_uniform( program, "observer", camera.position() ) ;
        program_uniform( program, "light.position", light.position ) ;
        program_uniform( program, "light.diffuse", light.diffuse ) ;
        program_uniform( program, "light.ambient", light.ambient ) ;
        program_uniform( program, "light.specular", light.specular ) ;
        program_uniform( program, "mMatrix", model ) ;
        // program_uniform( program, "mvMatrix", mv ) ;
        program_uniform( program, "mvpMatrix", mvp ) ;
        // program_uniform( program, "normalsMatrix", mv.normal() ) ;

        program_uniform( program, "use_text", use_text ) ;
        program_use_texture( program, "heightmap_text", 7, heightmap_text ) ;
        program_uniform( program, "map_width", (float)map_width ) ;
        program_uniform( program, "map_height", (float)map_height ) ;

        //shadowMap
        program_uniform( program, "use_shadows", use_shadows ) ;
        program_uniform( program, "lmvpMatrix", lmvp ) ;
        program_use_texture( program, "shadowMap", 0, depth_buffer ) ;

        /*
            DRAW
        */
        for( unsigned int i = 0 ; i < chunks_visibles.size() ; i++ )
        {
            chunks_visibles[i]->draw( ) ;
        }


        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0) ;
    }
}


/*==============================
        GETTERs SETTERs
==============================*/

/**
* \brief Trouve le chunk auquel appartient un point 
* \param[in] pos : Un point de la carte
* \return l'indice dans la liste des chunks du chunk auquel appartient le point 
*/
int Map::get_ichunk( const Point pos )
{
    int x = pos.x-posDepart.x ;
    int z = pos.z-posDepart.z ;

    if( x < 0 || x >= (int)map_width || z < 0 || z >= (int)map_height )
        return -1 ;

    int i = x/CHUNK_SIZE ;
    int j = z/CHUNK_SIZE ;
    int index_chunk = j * ( map_width / CHUNK_SIZE ) + i ;

    return index_chunk ;
}

/**
* \brief Retourne un chunk a partir de son indice
* \param[in] pos : l'indice du chunk dans la liste
* \return Un pointeur sur le chunk
*/
Chunk * Map::get_chunk( const unsigned int pos )
{
    if( pos >= chunks.size() )
        return NULL ;

    return &chunks[pos] ;
}

/**
* \brief Retourne l'action de l'utilisateur (creuse ou pas)
* \return Vrai si l'on est en train de creuser
*/
bool Map::digging()
{
    return is_digging ;
}

/**
* \brief Active/Désactive l'action de creuser
* \param[in] d : true pour creuser
*/
void Map::digging( bool d )
{
    is_digging = d ;
}

/**
* \brief Retourne l'utilisation ou non d'une texture
* \return Vrai si une texture est utilisée
*/
bool Map::use_texture()
{
    return use_text ;
}

/**
* \brief Active/Désactive l'utilisation d'une texture
* \param[in] t : true pour utiliser une texture
*/
void Map::use_texture( bool t )
{
    use_text = t ;
}

/**
* \brief Retourne l'utilisation ou non des ombres 
* \return Vrai si l'on affiche les ombres
*/
bool Map::use_shadow()
{
    return use_shadows ;
}

/**
* \brief Active/Désactive l'affichage des ombres
* \param[in] s : true pour afficher les ombres
*/
void Map::use_shadow( bool s )
{
    use_shadows = s ;
}

/**
* \brief Retourne le plus grand côté de la carte
* \return Max(largeur,hauteur)
*/
int Map::getSizeMax()
{
    return std::max(map_width, map_height ) ;
}