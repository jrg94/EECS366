   
uniform vec3 AmbientContribution,DiffuseContribution,SpecularContribution;
uniform float exponent;

attribute float tang;

varying vec3 vNormal, vLight, vView, vHalfway;

/*
===============================================================================
   Phong Shading: Vertex Program
===============================================================================
*/

void main(void)
{
   // Transform vertex position to view space
   
   vec3 pos = vec3( gl_ModelViewMatrix * gl_Vertex );
   
   // Compute normal, light and view vectors in view space
   
   vNormal   = normalize(gl_NormalMatrix * gl_Normal);
   vLight    = normalize(vec3(gl_LightSource[0].position)- pos);
   vView     = normalize(-pos);
   
   // Compute the halfway vector if the halfway approximation is used   
   
   vHalfway  = normalize(vLight + vView );
	
   float data_from_opengl = tang;
   gl_Position = ftransform();
   


}