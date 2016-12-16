
#include <cstdio>
#include <algorithm>

#include "camera_fps.h"
#include "map.hpp"

Camera_FPS::Camera_FPS()
{
    // center = vec3( 0, 0, 0 ) ;
    pos = vec3( 0, 10, 0 ) ;
    up = vec3( 0.0, 1.0, 0.0 ) ;
    fov = 45.0f ;
    speed = 0.5f ;

    pitch = 0.0f ;
    yaw = -90.0f ;

    cameraFront = Vector(0.0f, 0.0f, -1.0f) ;
    cameraUp = Vector(up) ;

    zmin = 0.1 ;
    zmax = 1000 ;

    size_cam = vec3(0.5,1.5,0.5) ;

    lookat( Point(0,0,0) ) ;
}


Point Camera_FPS::position( )
{
    return Point(pos) ;
}

void Camera_FPS::position( const Point _pos )
{
    pos = vec3(_pos) ;
    cameraFront = Vector( center.x - pos.x, center.y - pos.y, center.z - pos.z ) ;
    update() ;
}

Point Camera_FPS::lookat( )
{
    return Point(center) ;
}

void Camera_FPS::lookat( const Point& _center )
{
    center = vec3(_center) ;
    cameraFront = Vector( center.x - pos.x, center.y - pos.y, center.z - pos.z ) ;
    update() ;
}


void Camera_FPS::avance( Map& map )
{
    Point newP ;
    newP.x = pos.x + (speed * cameraFront.x) ;
    newP.y = pos.y + (speed * cameraFront.y) ;
    newP.z = pos.z + (speed * cameraFront.z) ;

    if( ! canMove( map, newP ))
    {
        rotate( 0, -20 ) ;
        
        newP.x = pos.x + (speed * cameraFront.x) ;
        newP.y = pos.y + (speed * cameraFront.y) ;
        newP.z = pos.z + (speed * cameraFront.z) ;

        canMove( map, newP ) ;
        
        rotate( 0, 20 ) ;
    }
}

void Camera_FPS::recule( Map& map )
{
    Point newP ;
    newP.x = pos.x - (speed * cameraFront.x) ;
    newP.y = pos.y - (speed * cameraFront.y) ;
    newP.z = pos.z - (speed * cameraFront.z) ;

    canMove( map, newP ) ;
}

void Camera_FPS::droite( Map& map )
{
    Point newP ;
    newP.x = pos.x + (speed * right.x) ;
    newP.y = pos.y + (speed * right.y) ;
    newP.z = pos.z + (speed * right.z) ;

    canMove( map, newP ) ;
}

void Camera_FPS::gauche( Map& map )
{
    Point newP ;
    newP.x = pos.x - (speed * right.x) ;
    newP.y = pos.y - (speed * right.y) ;
    newP.z = pos.z - (speed * right.z) ;

    canMove( map, newP ) ;
}

void Camera_FPS::jump( Map& map, float height )
{
    Point newP ;
    newP.x = pos.x ;
    newP.y = pos.y + height ;
    newP.z = pos.z ;

    canMove( map, newP ) ;
}

void Camera_FPS::gravite( Map& map, float gravity )
{
    Point newP ;
    newP.x = pos.x ;
    newP.y = pos.y - gravity ;
    newP.z = pos.z ;

    canMove( map, newP ) ;
}

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

void Camera_FPS::rotate( float x, float y )
{
    // yaw   += x ;
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
    cameraFront = normalize(front);

    update() ;
}

void Camera_FPS::zoomIn()
{
    fov-=1 ;
    if(fov <= 1.0f)
        fov = 1.0f;

    update() ;
}

void Camera_FPS::zoomOut()
{
    fov+=1 ;
    if(fov >= 45.0f)
        fov = 45.0f;

    update() ;
}

Point Camera_FPS::getForward( int distance )
{
    Point newP ;
    newP.x = pos.x + (speed * cameraFront.x)*distance ;
    newP.y = pos.y + (speed * cameraFront.y)*distance ;
    newP.z = pos.z + (speed * cameraFront.z)*distance ;
    return newP ;
}

void Camera_FPS::update()
{
    center = vec3(pos.x + cameraFront.x, pos.y + cameraFront.y, pos.z + cameraFront.z) ;
    direction = normalize( Vector(pos.x - center.x, pos.y - center.y, pos.z - center.z) ) ;
    right = normalize(cross(normalize(Vector(up)), direction));
    cameraUp = cross(direction, right);

}

Transform Camera_FPS::view( ) const
{
    return  Transform (
        right.x, right.y, right.z, 0.f,
        cameraUp.x, cameraUp.y, cameraUp.z, 0.f,
        direction.x, direction.y, direction.z, 0.f,
        0.0f, 0.0f, 0.0f, 1.0f 
    ) * Translation( -pos.x, -pos.y, -pos.z ) ;    
}

//! renvoie la projection reglee pour une image d'aspect width / height, et une ouverture de fov degres.
Transform Camera_FPS::projection( const float width, const float height ) const
{
    return Perspective(fov, width / height, zmin, zmax) ;
}

Transform Camera_FPS::projection_ortho( const float width, const float height ) const
{
    return  Transform (
        1.0/(width/2.0), 0.f, 0.f, 0.f,
        0.f, 1.0/(height/2.0), 0.f, 0.f,
        0.f, 0.f, -2.0/(zmax-zmin), -(zmax+zmin)/(zmax-zmin),
        0.0f, 0.0f, 0.0f, 1.0f 
    ) ;
}