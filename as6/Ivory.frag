const vec3 ambient = vec3(0.02, 0.02, 0.02);

uniform vec3 lightColor; //x=red, y=green, z=blue

varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

void main() {
	vec3 norm = normalize(normal);
	vec3 cam = normalize(viewvec);

	vec3 L = normalize(lightVec);
	vec3 V = normalize(viewVec);
	vec3 halfAngle = normalize(L + V);
	vec3 specColor = min(lightColor + 0.5, 1.0);

	float NdotL = dot(L, norm);
	float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);

	float diffuse = 0.5 * NdotL + 0.5;
	float specular = specColor * pow(NdotH, 64.0);

	float result = diffuse + specular + ambient;

	gl_FragColor = vec4(result);
}