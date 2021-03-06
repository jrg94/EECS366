/*

EECS 366/466 COMPUTER GRAPHICS
Template for Assignment 6-Local Illumination and Shading

*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <string.h>
#include <GL/glut.h>
#include <windows.h>
#include "glprocs.h"


#define PI 3.14159265359

using namespace std;

//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
void TestCompilation(GLuint shader);
GLuint vertex_shader, fragment_shader;
GLuint p;
const char *vv1, *ff1;
int illimunationMode = 0;
int shadingMode = 0;
int lightSource = 0;
int color_counter = 0;


//Projection, camera contral related declerations
int WindowWidth,WindowHeight;
bool LookAtObject = false;
bool ShowAxes = true;



float CameraRadius = 10;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;



void DisplayFunc(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLint illum = glGetUniformLocationARB(p, "illumination");
	if (illum != -1) {
		glUniform1iARB(illum, illimunationMode);
		printf("IlluminationMode: %d\n", illimunationMode);
	}
	else {
		printf("Illumination does not exist\n");
	}

	GLint interp = glGetUniformLocationARB(p, "interp");
	if (interp != -1) {
		glUniform1iARB(interp, shadingMode);
		printf("Shading Mode: %d\n", shadingMode);
	}
	else {
		printf("Interp does not exist\n");
	}

	GLint light = glGetUniformLocationARB(p, "light");
	if (light != -1) {
		glUniform1iARB(light, lightSource);
		printf("Active light: %d\n", lightSource);
	}
	else {
		printf("light is not a valid uniform value\n");
	}

	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  0,0,0,
			  0,0,1);
	glEnable(GL_DEPTH_TEST);
	glutSolidTeapot(1);
	if (lightSource == 1) {
		GLfloat g_lightPos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glGetLightfv(GL_LIGHT1, GL_POSITION, g_lightPos);
		glTranslatef(g_lightPos[0], g_lightPos[1], g_lightPos[2]);
		glutSolidCube(0.5);
	}

	glutSwapBuffers();
}

void ReshapeFunc(int x,int y)
{
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}


void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

void MotionFunc(int x, int y)
{
	if(MouseLeft)
	{
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
	}
	if(MouseRight)
	{
        CameraRadius += 0.2*(MouseY-y);
		if(CameraRadius <= 0)
			CameraRadius = 0.2;
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}


void setShaders() {

	char *vs1 = NULL, *fs1 = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	
	//read the shader files and store the strings in corresponding char. arrays.
	vs1 = shaderFileRead("shader.vert");			// Gouraud interpolation
	fs1 = shaderFileRead("shader.frag");			// Cooke-Torrance illumination

	vv1 = vs1;
	ff1 = fs1;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSourceARB(vertex_shader, 1, &vv1,NULL);
	glShaderSourceARB(fragment_shader, 1, &ff1,NULL);

	free(vs1); free(fs1);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);
	
	// Shows if shaders compiled
	TestCompilation(vertex_shader);
	TestCompilation(fragment_shader);

	//create an empty program object to attach the shader objects
	p = glCreateProgramObjectARB();

	//attach the shader objects to the program object
	// Gouraud interp & Phong Lighting
	glAttachObjectARB(p,vertex_shader);
	glAttachObjectARB(p,fragment_shader);

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteObjectARB(vertex_shader);
	glDeleteObjectARB(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgramARB(p);

	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p);

}

void TestCompilation(GLuint shader) {
	GLint compiled;
	glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

	if (compiled) {
		printf("%d shader compiled\n", shader);
	}
	else {
		printf("Compilation failed\n");
	}
}


//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	// Toggle Phong and Cooke-Torrance local illumination
	case 'w':
	case 'W':
		if (illimunationMode == 0)
		{
			illimunationMode = 1;
			printf("Toggled Cooke-Torrance illumination mode\n");
		}
		else
		{
			illimunationMode = 0;
			printf("Toggled Phong illumination mode\n");
		}
		break;
	// Toggle Gouraud and Phong interpolative shading
	case 'e':
	case 'E':
		if (shadingMode == 0)
		{
			shadingMode =1;
			printf("Toggled Gouraud interpolation mode\n");
		}
		else
		{
			shadingMode =0;
			printf("Toggled Phong interpolation mode\n");
		}
		break;
	// Toggle primary and secondary light
	case 'd':
	case 'D':
		if (lightSource == 0)
		{
			lightSource =1;
		}
		else
		{
			lightSource =0;
		}
		break;
	// Cycle color of secondary light source
	case 'f':
	case 'F':
		if (lightSource == 1)
		{
			//change color of the secondary light source at each key press, 
			//light color cycling through pure red, green, blue, and white.

			GLfloat color_cycle[4][4] = { { 1.0f, 0.0f, 0.0f, 1.0f },
										{ 0.0f, 1.0f, 0.0f, 1.0f },
										{ 0.0f, 0.0f, 1.0f, 1.0f },
										{ 1.0f, 1.0f, 1.0f, 1.0f } };
			glLightfv(GL_LIGHT1, GL_DIFFUSE, color_cycle[color_counter % 4]);
			color_counter++;
		}
		break;

    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char **argv) 
{			  

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(500,500);
	glutCreateWindow("Assignment 6");

	

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);


	const GLubyte *temp;
	temp = glGetString(GL_VERSION);
	printf("%s\n", temp);
	temp = glGetString(GL_VENDOR);
	printf("%s\n", temp);
	temp = glGetString(GL_EXTENSIONS);
	printf("%s\n", temp);
	
	// Initialize light 0

	// Initialize light 1
	GLfloat g_lightPos[4] = { 7.0f, 7.0f, 7.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, g_lightPos);
	GLfloat g_amb[4] = { 0.02f, 0.02f, 0.02f, 1.0f};
	glLightfv(GL_LIGHT1, GL_AMBIENT, g_amb);
	GLfloat g_dif[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, g_dif);
	GLfloat g_spec[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_SPECULAR, g_spec);
	setShaders();

	// Initialize Materials - Copper
	GLfloat mat_dif[4] = { 0.755f, 0.49f, 0.095f , 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_dif);
	GLfloat mat_spec[4] = { 0.755f, 0.49f, 0.095f , 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec);

	glutMainLoop();

	return 0;
}


//Read the shader files, given as parameter.
char *shaderFileRead(char *fn) {


	FILE *fp = fopen(fn,"r");
	if(!fp)
	{
		cout<< "Failed to load " << fn << endl;
		return " ";
	}
	else
	{
		cout << "Successfully loaded " << fn << endl;
	}
	
	char *content = NULL;

	int count=0;

	if (fp != NULL) 
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);

		if (count > 0) 
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
	return content;
}