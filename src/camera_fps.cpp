/**
* \file camera_fps.cpp
* \brief Fichier d'implémentation de la classe Camera_FPS
* \date 22/12/2016
* \author Anna Benneton
* \author Anna-Katharina Bergmann
*/
//------------------------------
#include "camera_fps.hpp"
#include "map.hpp"
//------------------------------


/**
* \brief Constructeur
*
*   La camera regarde en (0,0,0) du dessus.
*
*/
Camera_FPS::Camera_FPS()
{   
    pos = vec3( 0, 10, 0 ) ;
    vecFront = Vector( 0.0f, 0.0f, -1.0f ) ;
    vecUp = Vector( 0.0, 1.0, 0.0 ) ;
    pitch = 0.0f ;
    yaw = -90.0f ;


    zmin = 0.1 ;
    zmax = 1000 ;
    fov = 45.0f ;

    size_cam = vec3( 0.5, 1.5, 0.5 ) ;
    speed = CAMERA_SPEED ;

    lookat( Point( 0, 0, 0 ) ) ;
}


/*==============================
            DÉPLACEMENTS
==============================*/   

/**
* \brief La caméra se déplace dans la direction de l'objet regardé
* \param[in] map : la carte pour gérérer les collisions
*/
void Camera_FPS::avance( Map& map )
{
    Point newP ;
    newP.x = pos.x + (speed * vecFront.x) ;
    newP.y = pos.y + (speed * vecFront.y) ;
    newP.z = pos.z + (speed * vecFront.z) ;

    if( ! canMove( map, newP ))
    {
        rotate( 0, -20 ) ;
        
        newP.x = pos.x + (speed * vecFront.x) ;
        newP.y = pos.y + (speed * vecFront.y) ;
        newP.z = pos.z + (speed * vecFront.z) ;

        canMove( map, newP ) ;
        
        rotate( 0, 20 ) ;
    }
}

/**
* \brief La caméra se déplace dans la direction inverse de l'objet regardé
* \param[in] map : la carte pour gérérer les collisions
*/
void Camera_FPS::recule( Map& map )
{
    Point newP ;
    newP.x = pos.x - (speed * vecFront.x) ;
    newP.y = pos.y - (speed * vecFront.y) ;
    newP.z = pos.z - (speed * vecFront.z) ;

    canMove( map, newP ) ;
}

/**
* \brief La caméra se déplace dans la direction perpendiculaire à l'objet regardé
* \param[in] map : la carte pour gérérer les collisions
*/
void Camera_FPS::droite( Map& map )
{
    Point newP ;
    newP.x = pos.x + (speed * vecRight.x) ;
    newP.y = pos.y + (speed * vecRight.y) ;
    newP.z = pos.z + (speed * vecRight.z) ;

    canMove( map, newP ) ;
}

/**
* \brief La caméra se déplace dans la direction perpendiculaire à l'objet regardé
* \param[in] map : la carte pour gérérer les collisions
*/
void Camera_FPS::gauche( Map& map )
{
    Point newP ;
    newP.x = pos.x - (speed * vecRight.x) ;
    newP.y = pos.y - (speed * vecRight.y) ;
    newP.z = pos.z - (speed * vecRight.z) ;

    canMove( map, newP ) ;
}

/**
* \brief La caméra se déplace vers le haut
* \param[in] map : la carte pour gérérer les collisions
* \param[in] height : la hauteur du saut
*/
void Camera_FPS::jump( Map& map, float height )
{
    Point newP ;
    newP.x = pos.x ;
    newP.y = pos.y + height ;
    newP.z = pos.z ;

    canMove( map, newP ) ;
}

/**
* \brief Rotation de la caméra sur elle même
* \param[in] x : Angle de rotation sur l'axe X
* \param[in] y : Angle de rotation sur l'axe Y
*/
void Camera_FPS::rotate( float x, float y )
{
    yaw = (int)(yaw + x) % 360 ;
    pitch -= y ;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;


    Vector front;
    front.x = cos(radians(yaw)) * cos(radians(pitch));
    front.y = sin(radians(pitch));
    front.z = sin(radians(yaw)) * cos(radians(pitch));
    vecFront = normalize(front);

    update() ;
}

/**
* \brief Déplace la caméra vers le bas
* \param[in] map : la carte pour gérérer les collisions
* \param[in] gravity : la distance de déplacement vers le bas
*/
void Camera_FPS::gravite( Map& map, float gravity )
{
    Point newP ;
    newP.x = pos.x ;
    newP.y = pos.y - gravity ;
    newP.z = pos.z ;

    canMove( map, newP ) ;
}

/**
* \brief Vérifie si un mouvement est possible, si oui, l'applique
* \param[in] map : la carte pour gérérer les collisions
* \param[in] newP : la nouvelle position
* \return TRUE si le mouvement a été possible
*/
bool Camera_FPS::canMove( Map& map, Point newP )
{
    if( !active_collision )
    {
        pos.x = newP.x ;
        pos.y = newP.y ;
        pos.z = newP.z ;
        update() ;
        return true ;
    }

    bool collide = false ;
    int i_chunk = map.get_ichunk( newP ) ;
    if( i_chunk >= 0 ){
        collide = map.get_chunk( i_chunk )->check_collision( newP, size_cam ) ;
    }

    if( !collide )
    {
        pos.x = newP.x ;
        pos.y = newP.y ;
        pos.z = newP.z ;
        update() ;  
    }
    return !collide ;
}

/**
* \brief zoom avant
*/
void Camera_FPS::zoomIn()
{
    fov-=1 ;
    if(fov <= 1.0f)
        fov = 1.0f;

    update() ;
}

/**
* \brief zoom arrière
*/
void Camera_FPS::zoomOut()
{
    fov+=1 ;
    if(fov >= 45.0f)
        fov = 45.0f;

    update() ;
}


/*==============================
            VUE
==============================*/ 

/**
* \brief Met à jour la vue en fonctions des différents vecteurs de position
*/
void Camera_FPS::update()
{    
    target = vec3( pos.x + vecFront.x, pos.y + vecFront.y, pos.z + vecFront.z ) ;
    direction = normalize( Vector( pos.x - target.x, pos.y - target.y, pos.z - target.z ) ) ;
    vecRight = normalize( cross( normalize( Vector(0.0, 1.0, 0.0) ), direction ) ) ;
    vecUp = cross( direction, vecRight ) ;
}

/**
* \brief Renvoie la transformation Vue de la caméra.
* \return La Matrice Vue
*/
Transform Camera_FPS::view( ) const
{
    return  Transform (
        vecRight.x, vecRight.y, vecRight.z, 0.f,
        vecUp.x, vecUp.y, vecUp.z, 0.f,
        direction.x, direction.y, direction.z, 0.f,
        0.0f, 0.0f, 0.0f, 1.0f 
    ) * Translation( -pos.x, -pos.y, -pos.z ) ;    
}

/**
* \brief Renvoie la projection selon l'ouverture (fov) et la profondeur de la caméra.
* \param[in] width : la largeur de l'image projetée
* \param[in] height : la hauteur de l'image projetée
* \return La Matrice Projection
*/
Transform Camera_FPS::projection( const float width, const float height ) const
{
    return Perspective(fov, width / height, zmin, zmax) ;
}

/**
* \brief Renvoie la projection Orthonormée selon la profondeur de la caméra.
* \param[in] width : la largeur de l'image
* \param[in] height : la hauteur de l'image
* \return La Matrice Projection Orthonormée
*/
Transform Camera_FPS::projection_ortho( const float width, const float height ) const
{
    return  Transform (
        1.0/(width/2.0), 0.f, 0.f, 0.f,
        0.f, 1.0/(height/2.0), 0.f, 0.f,
        0.f, 0.f, -2.0/(zmax-zmin), -(zmax+zmin)/(zmax-zmin),
        0.0f, 0.0f, 0.0f, 1.0f 
    ) ;
}


/*==============================
        GETTERs SETTERs
==============================*/   
/**
* \brief Retourne la position de la caméra
* \return Point : Position actuelle de la caméra
*/
Point Camera_FPS::position()
{
    return Point(pos) ;
}

/**
* \brief Définie la position de la caméra
* \param[in] _pos : la nouvelle position
*/
void Camera_FPS::position( const Point _pos )
{
    pos = vec3(_pos) ;
    vecFront = Vector( target.x - pos.x, target.y - pos.y, target.z - pos.z ) ;
    update() ;
}

/**
* \brief Retourne le point regardé par la caméra
* \return Point : Le point devant la caméra
*/
Point Camera_FPS::lookat()
{
    return Point(target) ;
}

/**
* \brief Définie le point regardé par la caméra
* \param[in] _target : la nouvelle cible regardée
*/
void Camera_FPS::lookat( const Point& _target )
{
    target = vec3(_target) ;
    vecFront = Vector( target.x - pos.x, target.y - pos.y, target.z - pos.z ) ;
    update() ;
}

/**
* \brief Défini la profondeur minimale
* \param[in] _zmin : la nouvelle profondeure min
*/
void Camera_FPS::set_zmin( float _zmin )
{
    zmin = _zmin ;
}

/**
* \brief Défini la profondeur maximale
* \param[in] _zmax : la nouvelle profondeure max
*/
void Camera_FPS::set_zmax( float _zmax )
{
    zmax = _zmax ;
}

/**
* \brief Active/Désactive les collisions pour la caméra
* \param[in] _c : true pour activer les collisions, false sinon
*/
void Camera_FPS::set_collision( bool _c )
{
    active_collision = _c ;
}

/**
* \brief Retourne le point regardé par la caméra, à une certaine distance
* \param[in] distance : la distance du point a la caméra
* \return Point devant la caméra
*/
Point Camera_FPS::getForward( int distance )
{
    Point newP ;
    newP.x = pos.x + (speed * vecFront.x)*distance ;
    newP.y = pos.y + (speed * vecFront.y)*distance ;
    newP.z = pos.z + (speed * vecFront.z)*distance ;
    return newP ;
}

/**
* \brief Retourne la vitesse de déplacement de la caméra
* \return Speed : la vitesse
*/
float Camera_FPS::get_speed()
{
    return speed ;
}

/**
* \brief Définie la vitesse de déplacement de la caméra
* \param[in] _speed : la nouvelle vitesse
*/
void Camera_FPS::set_speed( const float _speed )
{
    speed = _speed ;
}