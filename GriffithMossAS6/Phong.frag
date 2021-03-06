varying vec3 N;
varying vec3 v;
varying vec3 viewVec;

uniform int light;
uniform vec3 test;
 
// Per-fragment calculations
void main (void)  
{  
   //gl_LightSource[0].position.xyz = test;
   vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
   vec3 E = normalize(-v); 
   vec3 R = normalize(-reflect(L,N));  
 
   // Calculate the ambient term:  
   vec4 Iamb = gl_FrontLightProduct[0].ambient;    

   // Calculate the diffuse term:  
   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
   Idiff = clamp(Idiff, 0.0, 1.0);     
   
   // Calculate the specular term:
   vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
   Ispec = clamp(Ispec, 0.0, 1.0);
    
   // Calculate the total color per vertex  
   gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;     
}