#version 410 core

#define COLOR	    0
#define NORMAL		1
#define TOBLEND    2

const float PI = 3.14159265359;
const float TWOPI = 6.28318530718;
const float PI_2 = 1.57079632679;
const float DEG2RAD = TWOPI / 360.0;

precision highp int;

uniform float Time;
uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform float SpecularPower;
uniform float ColorMultiplier;
uniform vec3 PixColor;
uniform bool UsePixColor;

layout(location = COLOR ) out vec4 Color;
layout(location = NORMAL) out vec4 Normal;
layout(location = TOBLEND) out vec4 ToBlend;

in block
{
	vec2 Texcoord;
	vec3 CameraSpacePosition;
	vec3 CameraSpaceNormal;
} In; 

void main()
{
	vec3 n = normalize(In.CameraSpaceNormal);
	/*
	// gl_FrontFacing is not working on MacOSX Intel cards
	// Try this instead :
#if 0	
	if (dot(n, normalize(-In.CameraSpacePosition)) < 0.)
 #else
	if (!gl_FrontFacing)
#endif
		n = -n;
		*/
	vec3  diffuseColor = texture(Diffuse, In.Texcoord).rgb * ColorMultiplier;
	if(UsePixColor)
	    diffuseColor = PixColor * ColorMultiplier;
	float specularColor = texture(Specular, In.Texcoord).r;
	Color = vec4(diffuseColor, specularColor);
	ToBlend = vec4(vec3(0.f), 1.f);
	Normal = vec4( n, SpecularPower);
}
