#version 330

#ifdef VERTEX_SHADER

in vec3 position ;
in vec3 normal ;
in vec2 texture ;

uniform mat4 mMatrix ;
uniform mat4 mvpMatrix ;

out vec3 vertex_position ;
out vec3 vertex_normal ;
out vec2 vertex_texture ;


void main( )
{
    gl_Position = mvpMatrix * vec4( position, 1 ) ;
    vertex_position = vec3( mMatrix * vec4( position, 1 ) ) ;
    vertex_normal = transpose(inverse(mat3(mMatrix))) * normal;
    vertex_texture = texture ;
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
in vec2 vertex_texture ;

uniform vec3 observer ;
uniform Light light ;

uniform sampler2D texture_panda ;

void main( )
{
    float debug = 0 ;

    vec3 color = vec3( 1, 1, 1) ;
    
    // float start_map_x = 186 ;
    // float start_map_z = 186 ;
    // vec2 v_map = vec2( (-vertex_position.x*start_map_x), (vertex_position.y*start_map_z) ) ;
    // color = color * texture(texture_panda, vec2(1/vertex_position.x*186, 1/vertex_position.y*186)).rgb ;

    debug = dot(vertex_texture,vertex_texture) ;
    

    vec3 normal = normalize( vertex_normal ) ;
    vec3 light_dir = normalize( light.position - vertex_position ) ;
    
    // Ambient
    vec3 ambient = light.ambient * color;

    // Diffuse
    float diff = max( dot( light_dir, normal ), 0.0 ) ;
    vec3 diffuse = light.diffuse * diff * color;

    // Specular
    vec3 view_dir = normalize( observer - vertex_position ) ;
    vec3 halfwayDir = normalize( light_dir + view_dir ) ;
    float spec = pow( max( dot( normal, halfwayDir ), 0.0 ), 15 ) ;
    vec3 specular = light.specular * spec ;


    vec3 result_color = ambient + diffuse + specular ;

    debug = debug - debug + 1;
    gl_FragColor = vec4( result_color, 1.0 ) * debug;
}
#endif