#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
//out float gl_ClipDistance[1]

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec4 LightFragPosition;
    vec3 Normal;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 invert;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 clipPlane;

void main()
{

gl_Position = invert * projection * view * model * vec4(position, 1.0f);
vs_out.FragPos = vec3(model * vec4(position, 1.0f));
vs_out.LightFragPosition = projection * view * model * vec4(position, 1.0f);
vs_out.Normal = mat3(transpose(inverse(model))) * normal;
vs_out.TexCoords = texCoords;


vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
// re-orthogonalize T with respect to N
T = normalize(T - dot(T, N) * N);
// then retrieve perpendicular vector B with the cross product of T and N
vec3 B = cross(T, N);

mat3 TBN = mat3(-T, B, N);
vs_out.TangentLightPos = TBN * lightPos;
vs_out.TangentViewPos  = TBN * viewPos;
vs_out.TangentFragPos  = TBN * vs_out.FragPos;

gl_ClipDistance[0] = dot(model * vec4(position, 1.0f) , clipPlane);
//gl_ClipDistance[0] = 0;
}
