uniform int light;
uniform int interp;

varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;
varying vec4 vertColor;

void main() {

	// Phong interpolation
	if (interp == 0) {
		viewVec = vec3(gl_ModelViewMatrix * gl_Vertex);       
		normal = normalize(gl_NormalMatrix * gl_Normal);
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
	}
	// Goroud interpolation
	else if (interp == 1) {
		vec4 color;
		float NdotL;

		color = gl_FrontMaterial.ambient * gl_LightSource[light].ambient;

		viewVec = vec3(gl_ModelViewMatrix * gl_Vertex);
		normal = normalize(gl_NormalMatrix * gl_Normal);
		lightVec = normalize(vec3(gl_LightSource[light].position));

		float d = length(lightVec);

		NdotL = max(dot(normal,lightVec),0.1);

		//Account for distance
		NdotL *= (1.0 / (1.0 + (0.25 * d * d)));

		color += (gl_FrontMaterial.diffuse * gl_LightSource[light].diffuse) * NdotL;

		vertColor = color;
		gl_Position = ftransform();
	}
}