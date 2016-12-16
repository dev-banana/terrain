#version 330

#ifdef VERTEX_SHADER

in vec3 position ;
in vec3 normal ;

uniform mat4 mMatrix ;
uniform mat4 mvpMatrix ;
uniform mat4 lmvpMatrix ;
// uniform mat4 mvMatrix ;
// uniform mat4 normalsMatrix ;

out vec3 vertex_position ;
out vec3 vertex_normal ;
out float hauteur ;
out vec4 shadow_coord ;

void main( )
{
    gl_Position = mvpMatrix * vec4( position, 1 ) ;
    vertex_position = vec3( mMatrix * vec4( position, 1 ) ) ;
    vertex_normal = transpose(inverse(mat3(mMatrix))) * normal;
    hauteur = position.y ;
    shadow_coord = lmvpMatrix * vec4(vertex_position, 1.0);
}
#endif


#ifdef FRAGMENT_SHADER

struct Material {
    vec3 ambient ;
    vec3 diffuse ;
    vec3 specular ;
    float shininess ;
}; 
Material material_eau = Material( vec3(0,0,0), vec3( 0.1, 0.1, 1 ), vec3( 0.0, 0.0, 0.0 ), 32.0 ) ;
Material material_sable = Material( vec3(0,0,0), vec3( 1, 0.8, 0.2 ), vec3( 0, 0, 0 ), 12.0 ) ;
Material material_herbe = Material( vec3(0,0,0), vec3( 0, 0.8, 0 ), vec3( 0, 0, 0 ), 5.0 ) ;
Material material_terre = Material( vec3(0,0,0), vec3( 0.46, 0.26, 0 ), vec3( 0, 0, 0 ), 1.0 ) ;
Material material_neige = Material( vec3(0,0,0), vec3( 1, 1, 1 ),  vec3( 0.0, 0.0, 0.0 ),  25.0 ) ;

struct Light {
    vec3 position ;
    vec3 ambient ;
    vec3 diffuse ;
    vec3 specular ;
};


in vec3 vertex_position ;
in vec3 vertex_normal ;
in float hauteur ;
in vec4 shadow_coord ;

uniform float max_height ;
uniform float map_width ;
uniform float map_height ;

uniform vec3 observer ;
uniform Light light ;

uniform bool use_shadows ;
uniform sampler2D shadowMap ;

uniform bool use_text ;
uniform sampler2D heightmap_text ;


vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float calculShadow( vec4 frag_light )
{
    // perform perspective divide
    vec3 proj_coords = frag_light.xyz / frag_light.w;
    // Transform to [0,1] range
    proj_coords = proj_coords * 0.5 + 0.5 ;
    // Get closest depth value from light's perspective (using [0,1] range frag_light as coords)
    float closestDepth = texture(shadowMap, proj_coords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = proj_coords.z;
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(vertex_normal);
    vec3 light_dir = normalize( light.position - vertex_position ) ;
    
    // float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
    float bias = 0.005*tan( acos( dot(normal, light_dir) ) );
    bias = clamp( bias, 0, 0.01 ) ;
  
    // Check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, proj_coords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }

    //poisson
    for (int i=0;i<4;i++)
    {
        if ( texture( shadowMap, proj_coords.xy + poissonDisk[i]/700.0 ).z  <  proj_coords.z-bias ){
            shadow-=0.2;
        }
    }

    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(proj_coords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main( )
{
    float debug = 0 ;

    //color
    Material material ;
    if( hauteur < (28*max_height)/100 )
        material = material_eau ;
    else if( hauteur < (33*max_height)/100 )
        material = material_sable ;
    else if( hauteur < (53*max_height)/100 )
        material = material_herbe ;
    else if( hauteur < (88*max_height)/100 )
        material = material_terre ;
    else
        material = material_neige ;

    if(use_text)
    {
        float start_map_x = map_width / 2.0 ;
        float start_map_z = map_height / 2.0 ;
        vec2 v_map = vec2( (vertex_position.x+start_map_x) / map_width, (vertex_position.z+start_map_z) / map_height ) ;
        material.diffuse = texture( heightmap_text, v_map ).rgb ;
    }
    

    vec3 normal = normalize( vertex_normal ) ;
    vec3 light_dir = normalize( light.position - vertex_position ) ;
    
    // Ambient
    vec3 ambient = light.ambient * material.ambient ;

    // Diffuse
    float diff = max( dot( light_dir, normal ), 0.0 ) ;
    vec3 diffuse = light.diffuse * ( diff * material.diffuse ) ;

    // Specular
    vec3 view_dir = normalize( observer - vertex_position ) ;
    vec3 halfwayDir = normalize( light_dir + view_dir ) ;
    float spec = pow( max( dot( normal, halfwayDir ), 0.0 ), material.shininess ) ;
    vec3 specular = light.specular * ( spec * material.specular ) ;

    // Shadows
    float shadow = 0 ; 
    if( use_shadows )
    {
        shadow = calculShadow( shadow_coord ) ; 
    }

    vec3 result_color = ambient + (1.0 - shadow) * ( diffuse + specular ) ;

    float alpha = 0.0 ;

    debug = debug - debug + 1 ;
    gl_FragColor = vec4( result_color, alpha ) * debug ;
}
#endif