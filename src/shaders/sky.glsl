#version 330

#ifdef VERTEX_SHADER

in vec3 position ;
in vec3 normal ;

uniform mat4 mvpMatrix ;
uniform mat4 mvMatrix ;
uniform mat4 normalsMatrix ;

out vec3 vertex_position ;
out vec3 vertex_normal ;

void main()
{
    float debug = 0 ;

    vertex_normal = mat3( normalsMatrix ) * normal ;
    vertex_position = position ;
    debug = dot(vec3( mvMatrix ),vec3( mvMatrix )) ;
    
    debug = debug - debug + 1 ;
    gl_Position = mvpMatrix * vec4( position, 1 ) * debug ;
    
}
#endif



#ifdef FRAGMENT_SHADER

struct Light {
    vec3 position ;

    vec3 ambient ;
    vec3 diffuse ;
    vec3 specular ;
};

in vec3 vertex_position ;
in vec3 vertex_normal ;

uniform vec3 observer ;
uniform Light light ;
uniform float weather;//mixing factor (0.5 to 1.0)

uniform sampler2D sky1;//the result_color of the sky on the half-sphere where the sun is. (time x height)
uniform sampler2D sky2;//the result_color of the sky on the opposite half-sphere. (time x height)
uniform sampler2D sun;//sun texture (radius x time)
uniform sampler2D moon;//moon texture (circular)
uniform sampler2D clouds1;//light clouds texture (spherical UV projection)
uniform sampler2D clouds2;//heavy clouds texture (spherical UV projection)


//---------NOISE GENERATION------------
//---------------STARS-----------------
//Noise generation based on a simple hash, to ensure that if a given point on the dome
//(after taking into account the rotation of the sky) is a star, it remains a star all night long
float Hash( float n ){
        return fract( (1.0 + sin(n)) * 415.92653);
}
float Noise3d( vec3 x ){
    float xhash = Hash(round(400*x.x) * 37.0);
    float yhash = Hash(round(400*x.y) * 57.0);
    float zhash = Hash(round(400*x.z) * 67.0);
    return fract(xhash + yhash + zhash);
}

void main()
{
    float debug = 0 ;
    vec3 result_color;


    vec3 sun_norm = normalize( light.position ) ;
    vec3 pos_norm = normalize( vertex_position ) ;
    float dist = dot( sun_norm, pos_norm ) ;

    //We read the sky texture according to the position of the sun and the weather factor
    vec3 color_wo_sun = texture(sky2, vec2((sun_norm.y + 1.0) / 2.0,max(0.01,pos_norm.y))).rgb;
    vec3 color_w_sun = texture(sky1, vec2((sun_norm.y + 1.0) / 2.0,max(0.01,pos_norm.y))).rgb;
    // result_color = weather*mix(color_wo_sun,color_w_sun,dist*0.5+0.5);
    result_color = weather*mix(color_wo_sun,color_w_sun,dist*0.5+0.5);

    //Computing u and v for the clouds textures (spherical projection)
    float pi2 = 2*3.14159265 ;
    // float time_rot = clamp(time,0,360);
    float u = -0.5 + atan(pos_norm.z, pos_norm.x)/pi2  ;
    float v = 0.5 + acos(pos_norm.y)/pi2;

    //Cloud result_color
    //result_color depending on the weather (shade of grey) *  (day or night) ?
    vec3 cloud_color = vec3(min(weather*3.0/2.0,1.0))*(sun_norm.y > 0.2 ? 0.95 : 0.50+sun_norm.y*1.8);
    //Reading from the clouds maps
    //mixing according to the weather (1.0 -> clouds1 (sunny), 0.5 -> clouds2 (rainy))
    //+ time translation along the u-axis (horizontal) for the clouds movement
    // float time = clamp(time,0.01,1);
    float transparency = mix(texture(clouds2,vec2(u+1,v)).r,texture(clouds1,vec2(u+1,v)).r,(weather-0.5)*2.0);


    // // Stars
    if(sun_norm.y<0.3){//Night or dawn
        float threshold = 0.99;
        //We generate a random value between 0 and 1
        float star_intensity = Noise3d(normalize(vertex_position));
        //And we apply a threshold to keep only the brightest areas
        if (star_intensity >= threshold){
            //We compute the star intensity
            star_intensity = pow((star_intensity - threshold)/(1.0 - threshold), 6.0)*(-sun_norm.y+0.1);
            result_color += vec3(star_intensity);
        }
    }

    //Sun
    float radius = length(pos_norm-sun_norm);
    if(radius < 0.05){//We are in the area of the sky which is covered by the sun
        float time_sun = clamp(sun_norm.y,0.01,1);
        radius = radius/0.05;
        if(radius < 1.0-0.001){//< we need a small bias to avoid flickering on the border of the texture
            //We read the alpha value from a texture where x = radius and y=height in the sky (~time)
            vec4 sun_color = texture(sun,vec2(radius,time_sun));
            result_color = mix(result_color,sun_color.rgb,sun_color.a);       
        }
    }


    //Moon
    float radius_moon = length(pos_norm+sun_norm);//the moon is at position -sun_pos
    if(radius_moon < 0.03){//We are in the area of the sky which is covered by the moon
        //We define a local plane tangent to the skydome at -sun_norm
        //We work in model space (everything normalized)
        vec3 n1 = normalize(cross(-sun_norm,vec3(0,1,0)));
        vec3 n2 = normalize(cross(-sun_norm,n1));
        //We project pos_norm on this plane
        float x = dot(pos_norm,n1);
        float y = dot(pos_norm,n2);
        //x,y are two sine, ranging approx from 0 to sqrt(2)*0.03. We scale them to [-1,1], then we will translate to [0,1]
        float scale = 23.57*0.5;
        //we need a compensation term because we made projection on the plane and not on the real sphere + other approximations.
        float compensation = 1.4;
        //And we read in the texture of the moon. The projection we did previously allows us to have an undeformed moon
        //(for the sun we didn't care as there are no details on it)
        result_color = mix(result_color,texture(moon,vec2(x,y)*scale*compensation+vec2(0.5)).rgb,clamp(-sun_norm.y*3,0,1));
        
    }

    //Final mix
    //mixing with the cloud result_color allows us to hide things behind clouds (sun, stars, moon)
    result_color = mix(result_color,cloud_color,clamp((2-weather)*transparency,0,1)) ;    

    debug += dot(result_color,result_color) ;
    debug += dot(observer,observer) ;
    debug += dot(vertex_normal,vertex_normal) ;    
    debug = debug - debug + 1;

    gl_FragColor = vec4( result_color, 1.0 ) * debug;
}
#endif