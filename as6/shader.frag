varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;
varying vec4 vertColor;

uniform int light;
uniform int interp;
uniform int illumination;

void main() {

	gl_MaterialParameters material = gl_FrontMaterial;
	gl_LightSourceParameters lightSource = gl_LightSource[light];

	vec4 color;
	vec3 L = normalize(lightSource.position.xyz - viewVec);   
	vec3 E = normalize(-viewVec); 
	vec3 N = normalize(normal);
	vec3 R = normalize(-reflect(L,N));

	float NdotL = max(dot(N, L), 0.0);

	vec3 ambient; 
	vec3 diffuse; 
	vec3 specular;

	// Phong interpolation
	if (interp == 0) {
 
		// Calculate the ambient term:  
	    ambient = material.ambient.xyz * material.ambient.w * lightSource.ambient.xyz;    

		// Calculate the diffuse term:  
		diffuse = material.diffuse.xyz * material.diffuse.w * lightSource.diffuse.xyz * NdotL;

		color = vec4(ambient + diffuse, 1.0);

	} 
	// Gouraud interpolation
	else if (interp == 1) {
		color = vertColor;
	}

	// Phong illumination
	if (illumination == 0) {
   
		// Calculate the specular term:
		vec4 Ispec = material.specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
		Ispec = clamp(Ispec, 0.0, 1.0);
    
		// Calculate the total color per vertex  
		color += Ispec;
	}
	// Cooke illumination
	else if (illumination == 1) {
		// set important material values
		float roughnessValue = 0.3;			// 0 : smooth, 1: rough
		float F0 = 0.8;						// fresnel reflectance at normal incidence
		float k = 0.2;						// fraction of diffuse reflection (specular reflection = 1 - k)
        
		// do the lighting calculation for each fragment.
    
		float specularConst = 0.0;
		if(NdotL > 0.0)
		{

			// calculate intermediary values
			vec3 halfVector = normalize(L + E);
			float NdotH = max(dot(N, halfVector), 0.0); 
			float NdotV = max(dot(N, E), 0.0); // note: this could also be NdotL, which is the same value
			float VdotH = max(dot(E, halfVector), 0.0);
			float mSquared = roughnessValue * roughnessValue;
        
			// geometric attenuation
			float NH2 = 2.0 * NdotH;
			float g1 = (NH2 * NdotV) / VdotH;
			float g2 = (NH2 * NdotL) / VdotH;
			float geoAtt = min(1.0, min(g1, g2));
     
			// roughness (or: microfacet distribution function)
			// beckmann distribution function
			float r1 = 1.0 / ( 4.0 * mSquared * pow(NdotH, 4.0));
			float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
			float roughness = r1 * exp(r2);
        
			// fresnel
			// Schlick approximation
			float fresnel = pow(1.0 - VdotH, 5.0);
			fresnel *= (1.0 - F0);
			fresnel += F0;
        
			specularConst = (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14);
		}
    
		specular = material.specular.xyz * NdotL * (k + specularConst * (1.0 - k));
		color += vec4(specular, 1.0);
	}

	// Set the color for this fragment	
	gl_FragColor = color;
}