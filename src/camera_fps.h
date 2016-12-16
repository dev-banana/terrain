#ifndef _CAMERA_FPS_H
#define _CAMERA_FPS_H

#include "vec.h"
#include "mat.h"

class Map ;

//! \addtogroup objet3D
///@{

//! \file 
//! camera pour observer un objet.

//! representation de la camera FPS, déplacement libre
class Camera_FPS
{
public:

    //! cree une camera par defaut.
    Camera_FPS( ) ;


    void avance( Map& ) ;
    void recule( Map& ) ;
    void gauche( Map& ) ;
    void droite( Map& ) ;
    void rotate( float x, float y ) ;
    void jump( Map&, float ) ;
    void gravite( Map&, float ) ;

    void zoomIn() ;
    void zoomOut() ;
    
    Point getForward( int ) ;
    
    //! renvoie la transformation vue.
    Transform view() const ;
    //! renvoie la projection perspective reglee pour une image d'aspect width / height, et une ouverture de fov degres.
    Transform projection( const float width, const float height ) const ;
    //! renvoie la projection orthonormée
    Transform projection_ortho( const float width, const float height ) const ;

    
    Point position() ;
    void position( const Point _pos ) ;
    Point lookat() ;
    void lookat( const Point& center ) ;
    void set_zmin( float _zmin ){ zmin = _zmin ;}
    void set_zmax( float _zmax ){ zmax = _zmax ;}
    void set_collision( bool _c ){ active_collision = _c ;}

protected:

    vec3 pos ;
    vec3 center ;
    vec3 up ;

    Vector direction ;
    Vector right ;
    Vector cameraUp ;
    Vector cameraFront ;
    
    float pitch, yaw ;

    float zmin, zmax ;
    float fov ;

    float speed ;

    vec3 size_cam ;

    bool active_collision ;


    void update() ;

    bool canMove( Map&, Point ) ;
};

///@}
#endif
