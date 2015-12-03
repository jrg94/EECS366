// Some code

uniform vec3 AmbientContribution,SpecularContribution;
uniform float exponent;

attribute float tang;
attribute vec3 tangent;
attribute vec3 bitangent;

varying vec3 vNormal, vLight, vView, vHalfway;
varying vec2 texture_coordinate;
varying vec3 bump_coordinate;

varying vec3 vTangent;
varying vec3 vBitangent;

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
	
	// Compute tangent and bitangent vectors
	vTangent = normalize(gl_NormalMatrix * tangent);
	vBitangent = normalize(gl_NormalMatrix * bitangent);

	texture_coordinate = gl_MultiTexCoord0.xy;
	bump_coordinate = gl_MultiTexCoord1.xyz;

	float data_from_opengl = tang;
	gl_Position = ftransform();
}