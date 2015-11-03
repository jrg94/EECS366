uniform int light;
uniform int interp;

varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

void main() {

	// Phong interpolation
	if (interp == 0) {
		viewVec = vec3(gl_ModelViewMatrix * gl_Vertex);       
		normal = normalize(gl_NormalMatrix * gl_Normal);
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
	}
	// Goroud interpolation
	else if (interp == 1) {
		viewVec = vec3(gl_ModelViewMatrix * gl_Vertex);
		normal = normalize(gl_NormalMatrix * gl_Normal);
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	}
}