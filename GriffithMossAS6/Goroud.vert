uniform mat4 modelViewProjectionMat;
uniform mat4 modelViewMat;
uniform vec3 lightPos;

attribute vec4 vertPos;
attribute vec4 vertColor;
attribute vec3 vertNorm;

varying vec4 color;

void main(){

	vec3 V = vec3(modelViewMat*vertPos);
	vec3 N = vec3(modelViewMat*vec4(vertNorm,0.0));
	float d = length(lightPos - V);

	vec3 L = normalize(lightPos - V);

	float NdotL = max(dot(N,L),0.1);
	//Account for distance
	NdotL *= (1.0 / (1.0 + (0.25 * d * d)));

	color = vertColor * NdotL;

	gl_Position = modelViewProjectionMat * vertPos;
}