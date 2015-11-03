uniform int light;
uniform int illumination;
uniform int interp;

varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

void main() {
	// Interpolation handles position
	// Illumination handles color

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

		float d = length(gl_LightSource[light].position - viewVec);

		vec3 L = normalize(gl_LightSource[light].position.xyz - viewVec);
		vec3 E = normalize(-viewVec);
		vec3 R = normalize(reflect(-L, normal));

	}

	// Phong illumination
	if (illumination == 0) {
		
	} 
	// Cooke-Torrance
	else if (illumination == 1){
		// Do nothing
	}
}