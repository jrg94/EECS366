//Shader for Phong Illuminations and Phong shading

uniform vec3 AmbientContribution,DiffuseContribution,SpecularContribution;
uniform float exponent;
uniform sampler2D texture;
varying vec3 vNormal, vLight, vView, vHalfway;
varying vec4 texture_coordinate;

vec3 AmbientComponent(void)
{
   return vec3(AmbientContribution + 0.1);
}

vec3 DiffuseComponent(void)
{
   vec3 diffuse_color = texture2D(texture, vec2(texture_coordinate.x, texture_coordinate.y)).rgb;
   return vec3((DiffuseContribution * 0) + diffuse_color * max(0.0, dot(vNormal, vLight)));
}

vec3 SpecularComponent(void)
{   
      // Approximation to the specular reflection using the halfway vector
      return vec3(SpecularContribution * pow(max(0.0, dot(vNormal, vHalfway)), exponent));  
}

vec2 EnvironmentMapping(void)
{
   float PI = 3.14159265359;

   // Normalize all vectors
   vec3 N = normalize(vNormal);
   vec3 L = normalize(vLight);

   // Calculate reflection ray
   vec3 R = normalize(N - dot(N, L) * 2 * N);

   // Compute texture coordinates - Assuming sphere

   // ? = atan2(-(z - cz), x - cx)
   float theta = atan(-R.z / R.x);

   // f = acos(-(y - cy) / r);
   float phi = acos(-R.y);

   // u = (? + p) / 2 p
   float texx = (theta + PI) / (2 * PI);

   // v = f / p;
   float texy = phi / PI;

   vec2 texture = vec2( texx, texy);

   return texture;
}

/*
===============================================================================
   Phong Shading: Fragment Program
===============================================================================
*/

void main(void)
{
      
   // Phong Illumination Model
   
   vec3 color = (AmbientComponent() + DiffuseComponent()) + SpecularComponent();  
   // Final color
   
   
   gl_FragColor = vec4(color, 1.0);
}