varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

uniform int mode;

void main() {

	// Ivory
	if (mode == 0) {
		vec3 norm = normalize(normal);

		vec3 L = normalize(lightVec);
		vec3 V = normalize(viewVec);
		vec3 halfAngle = normalize(L + V);

		float NdotL = dot(L, norm);
		float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);

		float diffuse = 0.5 * NdotL + 0.5;
		float specular = pow(NdotH, 64.0);

		float result = diffuse + specular;

		gl_FragColor = vec4(result);
	}
	// Phong
	else if (mode == 1) {
		//gl_LightSource[0].position.xyz = test;
	   //vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
	   //vec3 E = normalize(-v); 
	  // vec3 R = normalize(-reflect(L,N));  
 
	   // Calculate the ambient term:  
	  // vec4 Iamb = gl_FrontLightProduct[0].ambient;    

	   // Calculate the diffuse term:  
	  // vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
	//   Idiff = clamp(Idiff, 0.0, 1.0);     
   
	   // Calculate the specular term:
	   //vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
	   //Ispec = clamp(Ispec, 0.0, 1.0);
    
	   // Calculate the total color per vertex  
	   //gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;     
	}
	// Goroud
	else if (mode == 2) {
		//gl_FragColor = color;
	}
	else if (mode == 3) {
		
	}
}