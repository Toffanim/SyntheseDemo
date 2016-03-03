#version 410 core

#define NUM_SAMPLES 4

in block
{
	vec2 Texcoord;
} In; 

uniform sampler2D Texture;
uniform vec2 ScreenLightPos;
uniform float Density = 1.f;
uniform float Weight = 1.f;
uniform float Decay = 1.f;
unfirom float Exposure = 1.f;

layout(location = 0, index = 0) out vec4  Color;

void main(void)
{
  // Calculate vector from pixel to light source in screen space.  
   vec2 deltaTexCoord = (In.Texcoord - ScreenLightPos.xy);  
  // Divide by number of samples and scale by control factor.  
  deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;  
  // Store initial sample.  
   vec2 color = texture(Texture, In.Texcoord);  
  // Set up illumination decay factor.  
   float illuminationDecay = 1.0f;  
  // Evaluate summation from Equation 3 NUM_SAMPLES iterations.  
   for (int i = 0; i < NUM_SAMPLES; i++)  
  {  
    // Step sample location along ray.  
    In.Texcoord -= deltaTexCoord;  
    // Retrieve sample at new location.  
   vec3 sample = texture(Texture, In.Texcoord);  
    // Apply sample attenuation scale/decay factors.  
    sample *= illuminationDecay * Weight;  
    // Accumulate combined color.  
    color += sample;  
    // Update exponential decay factor.  
    illuminationDecay *= Decay;  
  }  
  // Output final color with a further scale control factor.  
   return float4( color * Exposure, 1);  
}
}
