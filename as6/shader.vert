uniform vec4 lightPos;
uniform int interp;

varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

void main() {

	// Ivory
	if (interp == 0) {
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
		vec4 vert = gl_ModelViewMatrix * gl_Vertex;

		normal = gl_NormalMatrix * gl_Normal;
		lightVec = vec3(lightPos - vert);
		viewVec = -vec3(vert);
	}
	// Phong interpolation
	else if (interp == 1) {
		//v = vec3(gl_ModelViewMatrix * gl_Vertex);       
		normal = normalize(gl_NormalMatrix * gl_Normal);
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
	}
}