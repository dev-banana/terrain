/**
* \file map.hpp
* \brief Fichier de déclaration des classes Map et Chunk
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#ifndef MAP_HPP
#define MAP_HPP
//------------------------------
//------------------------------
//libs
#include <map>
//gkit
//tp2
#include "config.hpp"
#include "sky.hpp"
#include "camera_fps.hpp"
//------------------------------


/**
* \class Chunk
* \brief classe qui permet de définir une région de la carte
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*
*   Un chunk est une partie (carré) de la carte constituée de cube. 
*   Elle est définie comme un seul mesh, c'est à dire comme un ensemble de vertex.
*   Un chunk est dessiné seulement si il est visible par la caméra.
*
*/
class Chunk : public Mesh
{
    public:

        /*==============================
                    GLSL
        ==============================*/
        GLuint vao ;    //!< Vertex Array Object
        GLuint v_buffer ;   //!< Vertex Buffer
        GLuint n_buffer ;   //!< Normal Buffer



        /*==============================
                    PRIMITIVE
        ==============================*/
        Mesh boundingbox ;  //!< La boite englobante
        int v_count ;   //!< Le nombre de vertex du chunk
        std::map<vec3, std::vector<int>> cubeToVertex ; //!< Liste les vertices reliée à un cube (à une position)



        /**
        * \brief Constructeur de la classe Chunk
        */
        Chunk(){}
    	~Chunk() ;

        void draw() ;
        bool is_visible( const Transform& ) ;
        bool check_collision( const Point, vec3 ) ;
        bool remove_vertex( const Point ) ;
} ;


/**
* \class Map
* \brief classe qui permet de définir une carte
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*
*   La classe Map est un ensemble de Chunk.
*   Elle est initialisée à partir d'une image représentant les différents hauteurs.
*   Elle s'occupe également de dessiner les ombres.
*/
class Map
{
	protected:

		bool INIT = true ;    //!< true si initialisation = OK


        /*==============================
                    GLSL
        ==============================*/
        GLuint program ;    //!< Shader principal
        GLuint program_depth ;  //!< Shader pour les Ombres

        
        /*==============================
                    PRIMITIVE
        ==============================*/ 
        Mesh mesh_cube ;    //!< Définition d'un objet Cube
        

        /*==============================
                    CHUNKS
        ==============================*/ 
        std::vector<Chunk> chunks ; //!< La liste des chunks
        std::vector<Chunk*> chunks_visibles ;   //!< Liste des chunks visibles

        
        /*==============================
                    SHADOWS
        ==============================*/
        Camera_FPS camera_depth ;   //!< La caméra pour les ombres
        bool use_shadows ;  //!< Active ou Désactive les ombres
        GLuint color_buffer ;   //!< Texture de couleur
        GLuint depth_buffer ;   //!< Texture de profondeur
        GLuint framebuffer ;    //!< Framebuffer Object
        int framebuffer_width ; //!< largeur du framebuffer
        int framebuffer_height ;    //!< hauteur du framebuffer


        /*==============================
                    HEIGHTMAP
        ==============================*/
        Point posDepart ;   //!< La position (0,0,0) de la carte dans le repère global
        Image heightmap ;   //!< La carte des hauteurs pour l'initialisation du monde
        unsigned int map_width  ;   //!< Largeur de la carte
        unsigned int map_height ;   //!< Hauteur de la carte
        bool use_text ; //!< Active ou Désactive l'utilisation d'une texture
        GLuint heightmap_text ; //!< Texture qui va avec la heightmap


        /*==============================
                    ACTION
        ==============================*/ 
        bool is_digging ;   //!< True si l'on est en train de creuser
        int nbBloc ;    //!< Nombre de bloc en stock


        void build_shadow_map( Light& ) ;
        void draw_shadow_map() ;


    public:

        /**
        * \brief Constructeur de la classe Map
        */
        Map(){}
        /**
        * \brief Destructeur de la classe Map
        */
        ~Map(){}


        void init() ;
        void release() ;
        void draw( Camera_FPS&, Light& ) ;

        void update_chunks( Camera_FPS& ) ;

        void build( Camera_FPS& ) ;
        void dig( Camera_FPS& ) ;


        /*==============================
                GETTERs SETTERs
        ==============================*/ 
        Chunk * get_chunk( const unsigned int ) ;
        int get_ichunk( const Point ) ;
        bool digging() ;
        void digging( bool ) ;
        bool use_texture() ;
        void use_texture( bool ) ;
        bool use_shadow() ;
        void use_shadow( bool ) ;
        int getSizeMax() ;

        
        /**
        * \brief Surcharge de l'opérateur de test
        * \return TRUE si l'initialisation s'est bien passée.
        */
        operator bool() const 
        { 
            return INIT ; 
        }
} ;

#endif //MAP_HPP
