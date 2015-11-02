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
GLuint vertex_shader1, fragment_shader1;
GLuint vertex_shader2, fragment_shader2;
GLuint p1, p2, p3, p4;
const char *vv1, *ff1, *vv2, *ff2;
int illimunationMode = 0;
int shadingMode = 0;
int lightSource = 0;


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
	glDeleteObjectARB(p1);
	p1 = glCreateProgramObjectARB();

	// Gouraud interp
	if (shadingMode == 0) {
		glAttachObjectARB(p1, vertex_shader1);
		glLinkProgramARB(p1);
		glUseProgramObjectARB(p1);
	}
	// Phong interp
	else if (shadingMode == 1) {
		glAttachObjectARB(p1, vertex_shader2);
		glLinkProgramARB(p1);
		glUseProgramObjectARB(p1);
	}

	// Cook-Torrance illumination
	if (illimunationMode == 0) {
		glAttachObjectARB(p1, fragment_shader1);
		glLinkProgramARB(p1);
		glUseProgramObjectARB(p1);

	}
	// Phong illumination
	else if (illimunationMode == 1) {
		glAttachObjectARB(p1, fragment_shader2);
		glLinkProgramARB(p1);
		glUseProgramObjectARB(p1);
	}

	if (lightSource == 1) {
		glutSolidCube(1);
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
	char *vs2 = NULL, *fs2 = NULL;

	//create the empty shader objects and get their handles
	vertex_shader1 = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader1 = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	vertex_shader2 = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader2 = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	
	//read the shader files and store the strings in corresponding char. arrays.
	vs1 = shaderFileRead("Ivory.vert");				// Gouraud interpolation
	fs1 = shaderFileRead("Ivory.frag");				// Cooke-Torrance illumination
	vs2 = shaderFileRead("Phong.vert");				// Phong interpolation
	fs2 = shaderFileRead("Phong.frag");				// Phong illumination

	vv1 = vs1;
	ff1 = fs1;
	vv2 = vs2;
	ff2 = fs2;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSourceARB(vertex_shader1, 1, &vv1,NULL);
	glShaderSourceARB(fragment_shader1, 1, &ff1,NULL);
	glShaderSourceARB(vertex_shader2, 1, &vv2, NULL);
	glShaderSourceARB(fragment_shader2, 1, &ff2, NULL);

	free(vs1); free(fs1);
	free(vs2); free(fs2);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader1);
	glCompileShaderARB(fragment_shader1);
	glCompileShaderARB(vertex_shader2);
	glCompileShaderARB(fragment_shader2);
	
	// Shows if shaders compiled
	TestCompilation(vertex_shader1);
	TestCompilation(vertex_shader2);
	TestCompilation(fragment_shader1);
	TestCompilation(fragment_shader2);

	//create an empty program object to attach the shader objects
	p1 = glCreateProgramObjectARB();

	//attach the shader objects to the program object
	// Gouraud interp & Phong Lighting
	glAttachObjectARB(p1,vertex_shader1);
	glAttachObjectARB(p1,fragment_shader1);

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	//glDeleteObjectARB(vertex_shader1);
	//glDeleteObjectARB(fragment_shader1);
	//glDeleteObjectARB(vertex_shader2);
	//glDeleteObjectARB(fragment_shader2);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgramARB(p1);

	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p1);

}

void TestCompilation(GLuint shader) {
	GLint compiled;
	glGetObjectParameterivARB(vertex_shader1, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

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
			printf("Toggled Phong illumination mode\n");
		}
		else
		{
			illimunationMode = 0;
			printf("Toggled Cooke-Torrance illumination mode\n");
		}
		break;
	// Toggle Gouraud and Phong interpolative shading
	case 'e':
	case 'E':
		if (shadingMode == 0)
		{
			shadingMode =1;
			printf("Toggled Phong interpolation mode\n");
		}
		else
		{
			shadingMode =0;
			printf("Toggled Gouraud interpolation mode\n");
		}
		break;
	// Toggle primary and secondary light
	case 'd':
	case 'D':
		if (lightSource == 0)
		{
			lightSource =1;
			GLint loc = glGetUniformLocationARB(p1, "lightPos");
			if (loc != -1) {
				glUniform4fARB(loc, 7.0, 7.0, 7.0, 1.0);
				printf("I just switched lights\n");
			}
			else {
				printf("lightPosition is not a valid uniform value\n");
			}
		}
		else
		{
			lightSource =0;
			GLint loc = glGetUniformLocationARB(p1, "lightPos");
			if (loc != -1) {
				glUniform4fARB(loc, 0.0, 0.0, 0.0, 1.0);
				printf("I just switched lights\n");
			}
			else {
				printf("lightPosition is not a valid uniform value\n");
			}
		}
		break;
	// Cycle color of secondary light source
	case 'f':
	case 'F':
		if (lightSource == 1)
		{
			//change color of the secondary light source at each key press, 
			//light color cycling through pure red, green, blue, and white.
			GLint loc = glGetUniformLocationARB(p1, "lightColor");
			if (loc != -1) {
				glUniform3fARB(loc, 1.0, 1.0, 0.0);
			}
			else {
				printf("lightColor is not a valid uniform value\n");
			}
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
	glutInitWindowSize(320,320);
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
	

	setShaders();

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