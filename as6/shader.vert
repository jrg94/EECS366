uniform vec4 lightPos;
uniform int mode;

varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

void main() {

	// Ivory
	if (mode == 0) {
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
		vec4 vert = gl_ModelViewMatrix * gl_Vertex;

		normal = gl_NormalMatrix * gl_Normal;
		lightVec = vec3(lightPos - vert);
		viewVec = -vec3(vert);
	}
	// Phong interpolation
	else if (mode == 1) {
		//v = vec3(gl_ModelViewMatrix * gl_Vertex);       
		normal = normalize(gl_NormalMatrix * gl_Normal);
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
	}
	// Goroud interpolation
	else if (mode == 1) {
		//vec3 V = vec3(modelViewMat*vertPos);
		//vec3 N = vec3(modelViewMat*vec4(vertNorm,0.0));
		//float d = length(lightPos - V);

		//vec3 L = normalize(lightPos - V);

		//float NdotL = max(dot(N,L),0.1);
		//Account for distance
		//NdotL *= (1.0 / (1.0 + (0.25 * d * d)));

		//color = vertColor * NdotL;

		//gl_Position = modelViewProjectionMat * vertPos;
	}
}