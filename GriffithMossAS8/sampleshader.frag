//Shader for Phong Illuminations and Phong shading

uniform vec3 AmbientContribution,SpecularContribution;
uniform float exponent;
uniform int mapping_mode;

uniform sampler2D tex;
uniform samplerCube cube_texture;

varying vec3 vNormal, vLight, vView, vHalfway;
varying vec2 texture_coordinate;

vec3 trueNorm;

vec3 AmbientComponent(void)
{
	return vec3(AmbientContribution + 0.1);
}

vec3 DiffuseComponent(vec2 texCoord)
{
	vec3 diffuse_color = texture2D(tex, texCoord).rgb;
	return vec3(diffuse_color * max(0.0, dot(trueNorm, vLight)));
}

vec3 SpecularComponent(void)
{   
	// Approximation to the specular reflection using the halfway vector
	return vec3(SpecularContribution * pow(max(0.0, dot(trueNorm, vHalfway)), exponent));  
}

vec2 EnvironmentMapping(void)
{
	float PI = 3.14159265359f;

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

	vec2 tex = vec2( texx, texy);

	return tex;
}

vec3 BumpMapping(vec2 texCoord, float offset) {
	
	vec2 coords = texCoord;
	float A = texture2D(tex, coords.xy).x;
	float B = texture2D(tex, coords.xy + vec2(offset, 0)).x;
	float C = texture2D(tex, coords.xy + vec2(0, offset)).x;
	
	vec3 N = vec3(B-A, C-A, 0.1);
	
	return N;
}

/*
===============================================================================
   Phong Shading: Fragment Program
===============================================================================
*/

void main(void)
{
	vec2 texCoord;
	
	// Environment mapping      
	if (mapping_mode == 1) {
		trueNorm = vNormal;
		texCoord = EnvironmentMapping();
	}
	// Bump Mapping
	else if (mapping_mode == 2) {
		trueNorm = trueNorm + BumpMapping(texCoord, .01);
		texCoord = texture_coordinate;
	}
	// Texture Mapping
	else {
		trueNorm = vNormal;
		texCoord = texture_coordinate;
	}

	// Phong Illumination Model   
	vec3 color = (AmbientComponent() + DiffuseComponent(texCoord)) + SpecularComponent();  
	
	// Final color
	gl_FragColor = vec4(color, 1.0);
}