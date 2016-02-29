#version 410 core
in vec3 TexCoords;
in vec4 pos;
out vec4 color;

uniform samplerCube skybox;
uniform sampler2D DepthBuffer;
uniform vec3 sun;

//vec3 sunPos = vec3( 

void main()
{
	float screenWidth = 800.f;
	float screenHeight = 600.f;
	// Converting (x,y,z) to range [0,1]
	float x = gl_FragCoord.x/screenWidth;
	float y = gl_FragCoord.y/screenHeight;
	float z = gl_FragCoord.z; // Already in range [0,1]

    float depth = texture(DepthBuffer, vec2(x, y)).r;
    if ( depth >= 0.999 )
    {
        if ( distance( vec2(sun), vec2(x,y) ) <= 0.05 )
             color = vec4( 2.0, 2.0, 2.0, 1.0);
        else
             color = texture(skybox, TexCoords);    
    }
}
