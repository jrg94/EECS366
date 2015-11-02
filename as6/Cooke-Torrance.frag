uniform vec3 eyePosition;
uniform vec3 lightPosition;

uniform float roughness, fresnel;

varying vec3 surfacePosition, surfaceNormal;

void main() {
  //Light and view geometry
  vec3 viewDirection = normalize(eyePosition - surfacePosition);
  vec3 lightDirection = normalize(lightPosition - surfacePosition);

  //Surface properties
  vec3 normal = normalize(surfaceNormal);

  //Compute specular power
  float power = cookTorranceSpec(
    lightDirection, 
    viewDirection, 
    normal, 
    roughness,
    fresnel);

  gl_FragColor = vec4(power,power,power,1.0);
}