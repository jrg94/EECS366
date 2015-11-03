uniform int light;
uniform int illumination;
uniform int interp;

varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

void main() {


	// Phong interpolation
	if (interp == 0) {
		//v = vec3(gl_ModelViewMatrix * gl_Vertex);       
		normal = normalize(gl_NormalMatrix * gl_Normal);
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
	}
	// Goroud interpolation
	else if (interp == 1) {
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

	// Phong illumination
	if (illumination == 0) {
		vec3 normal, lightdir;
		vec4 color;
		float NdotL;

		color = gl_FrontMaterial.ambient * gl_LightSource[light].ambient;

		normal = normalize(gl_NormalMatrix * gl_Normal);
		lightdir = normalize(vec3(gl_LightSource[light].position));
		NdotL = max(dot(normal,lightdir), 0.0);

		color += NdotL * (gl_FrontMaterial.diffuse * gl_LightSource[light].diffuse);

		if (NdotL > 0.0) {
			vec3 view, reflection;
			float RdotV;

			view = vec3(-normalize(gl_ModelViewMatrix * gl_Vertex));
			reflection = normalize(reflect(-lightdir, normal));
			RdotV = max(dot(reflection, view), 0.0);

			color += gl_FrontMaterial.specular *
					 gl_LightSource[light].specular *
					 pow(RdotV, gl_FrontMaterial.shininess);
		}

		gl_FrontColor = color;
		gl_Position = ftransform();
	} else if (illumination == 1){
		// Do nothing
	}
}