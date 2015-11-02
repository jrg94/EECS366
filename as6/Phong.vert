attribute vec3 inputPosition;
attribute vec2 inputTextCoord;
attribute vec3 inputNormal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalMat;

varying vec3 normalInterp;
verying vec3 vertPos;

void main() {
	gl_Position = projection * modelview * vec4(inputPosition, 1.0);
	vec4 vertPos4 = modelview * vec4(inputPosition, 1.0);
	vertPos = vec3(vertPos4);
	normalInterp = vec3(normalMat * vec4(inputNormal, 0.0));
}