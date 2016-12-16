#version 330

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 position;
out vec3 vertex_position;

#ifdef USE_TEXCOORD
    layout(location= 1) in vec2 texcoord;
    out vec2 vertex_texcoord;
#endif

#ifdef USE_NORMAL
    layout(location= 2) in vec3 normal;
    out vec3 vertex_normal;
#endif

#ifdef USE_COLOR
    layout(location= 3) in vec4 color;
    out vec4 vertex_color;
#endif

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 normalMatrix;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_position= vec3(mvMatrix * vec4(position, 1));

#ifdef USE_TEXCOORD
    vertex_texcoord = texcoord;
#endif

#ifdef USE_NORMAL
    vertex_normal= mat3(normalMatrix) * normal;
#endif

#ifdef USE_COLOR
    vertex_color= color;
#endif
}
#endif


#ifdef FRAGMENT_SHADER

in vec3 vertex_position;

#ifdef USE_TEXCOORD
    in vec2 vertex_texcoord;
    uniform sampler2D diffuse_color;
#endif

#ifdef USE_NORMAL
    in vec3 vertex_normal;
#endif

#ifdef USE_COLOR
    in vec4 vertex_color;
#endif

#ifdef USE_LIGHT
uniform vec3 light;
uniform vec4 light_color;
#endif

uniform vec4 mesh_color= vec4(1, 1, 1, 0);

out vec4 fragment_color;

void main( )
{
    vec4 color= mesh_color;
#ifdef USE_COLOR
    color= vertex_color;
#endif

#ifdef USE_TEXCOORD
    color= color * texture(diffuse_color, vertex_texcoord);

    if(color.r + color.g + color.b == 0)    // noir
    {
        // fixer une couleur debug pour indiquer qu'il faut utiliser une texture avec cet objet
        vec3 p= vertex_position * 8;
	float d= length( p - (floor(p) + 0.5));
	if(d > 1) d= 0;
        color=  vec4(d*0.8*2, d*0.4*2, 0, 1);
    }
#endif

    vec3 normal;
#ifdef USE_NORMAL
    normal= normalize(vertex_normal);
#else
    vec3 t= normalize(dFdx(vertex_position));
    vec3 b= normalize(dFdy(vertex_position));
    normal= normalize(cross(t, b));
#endif

    float cos_theta;
#ifdef USE_LIGHT
    //~ cos_theta= abs(dot(normal, normalize(light - vertex_position)));
    cos_theta= max(0, dot(normal, normalize(light - vertex_position)));
    color= color * light_color;
#else
    cos_theta= abs(normal.z);
#endif

    color.rgb= color.rgb * cos_theta;

    // hachure les triangles mal orientes
    if(gl_FrontFacing == false) // if(!gl_FrontFacing) bug sur mac ?!
    {
        ivec2 pixel= ivec2(gl_FragCoord.xy / 4) % ivec2(2, 2);
        if((pixel.x ^ pixel.y) == 0)
            color= vec4(0.8, 0.4, 0, 1);
    }

    //fragment_color= vec4(color.rgb, 1);
    fragment_color= vec4(color);
}
#endif
