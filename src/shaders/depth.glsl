// Benneton, Bergmann
#version 330

#ifdef VERTEX_SHADER

layout(location = 0) in vec3 position ;

out vec4 pos;

uniform mat4 lmvpMatrix ;

void main( )
{
    pos = lmvpMatrix * (vec4(position  , 1));
    gl_Position = pos ;
}
#endif


#ifdef FRAGMENT_SHADER

in vec4 pos;

void main( )
{
    float z = ( (pos.z / pos.w) + 1 )*0.5;
	gl_FragColor = vec4(z , z, z, 1.0);
}
#endif
