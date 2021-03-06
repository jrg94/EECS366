//Shader for Phong Illuminations and Phong shading

uniform vec3 AmbientContribution,SpecularContribution;
uniform float exponent;
uniform int mapping_mode;
uniform int square;

uniform sampler2D texture_map;
uniform sampler2D bump_map;
uniform samplerCube cube_texture;

varying vec3 vNormal, vLight, vView, vHalfway;
varying vec2 texture_coordinate;
varying vec3 bump_coordinate;

varying vec3 vTangent;
varying vec3 vBitangent;

vec3 trueNorm;

vec3 AmbientComponent(void)
{
	return vec3(AmbientContribution + 0.1);
}

vec3 DiffuseComponent(vec2 texCoord)
{
	vec3 diffuse_color = texture2D(texture_map, texCoord).rgb;
	return vec3(diffuse_color * max(0.0, dot(trueNorm, vLight)));
}

vec3 SpecularComponent(void)
{   
	// Approximation to the specular reflection using the halfway vector
	return vec3(SpecularContribution * pow(max(0.0, dot(trueNorm, vHalfway)), exponent));  
}

/**
 * Produces the environment map coordinates
 */
vec2 EnvironmentMapping(void)
{
	// Normalize all vectors
	vec3 N = normalize(vNormal);
	vec3 L = normalize(vLight);

	// Calculate reflection ray
	vec3 R = reflect(L, N);
	
	float M = 2 * sqrt(pow(R.x, 2) + pow(R.y, 2) + pow(R.z, 2));
	
	vec2 vN = R.xy / M + .5;
	
	return -vN;
}

vec3 BumpMapping(vec2 texCoord, float offset) {
	
	//mat3 TBN = mat3(vTangent, vBitangent, vNormal);
	//vec3 N = normalize(texture2D(bump_map, gl_TexCoord[0].st).xyz * 2.0 - 1.0);
	
	float A = texture2D( bump_map, texCoord.xy ).x;
	float B = texture2D( bump_map, texCoord.xy + vec2( .01, 0 )).x;
	float C = texture2D( bump_map, texCoord.xy + vec2( 0, .01 )).x;
	
	float D = B - A;
	float E = C - A;
	
	vec3 N = vec3(D, E, .1);
	N = normalize(N);
	
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
	vec3 color;
	
	vec3 N = normalize(vNormal);
	vec3 H = normalize(vHalfway);
	vec3 V = normalize(vView);
	vec3 L = normalize(vLight);
	
	// Environment mapping      
	if (mapping_mode == 1) {
		trueNorm = V;
		if (square == 0) {
			color = texture2D(texture_map, EnvironmentMapping()).rgb;
		} 
		else if (square == 1) {
			color = texture(cube_texture, vec3(EnvironmentMapping(), 0.0)).rgb;
		}
	}
	// Bump Mapping
	else if (mapping_mode == 2) {
		trueNorm = N + BumpMapping(texCoord, .1);
		texCoord = texture_coordinate;
		color = (AmbientComponent() + DiffuseComponent(texCoord)) + SpecularComponent();
	}
	// Texture Mapping
	else {
		trueNorm = N;
		texCoord = texture_coordinate;
		color = (AmbientComponent() + DiffuseComponent(texCoord)) + SpecularComponent();
	}  
	
	// Final color
	gl_FragColor = vec4(color, 1.0);
}