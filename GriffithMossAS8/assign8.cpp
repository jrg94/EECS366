/**
 * EECS 366/466 COMPUTER GRAPHICS
 * Assignment 8-MAPPING
 * Fall 2015
 * Jeremy Griffith
 * Evelyn Moss
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <string.h>
#include <GL/glut.h>
#include <windows.h>
#include <vector>

#include "glprocs.h"
#include "assign8.h"
#include "read_tga.h"

// Define the pi constant
#define PI 3.14159265359

// Define the number of algorithms in this assignment
#define NUM_OF_ALGORITHMS 11

// Define an enum for mapping algorithms
#define TEXTURE_MAPPING 0
#define ENVIRONMENT_MAPPING 1
#define BUMP_MAPPING 2

// Define an enum for objects
#define PLANE 0
#define SPHERE 1
#define TEAPOT 2

// Define an enum for mapping objects
#define NONE -1
#define PLANE_MAP 0
#define SPHERE_MAP 1
#define CUBE_MAP 2

// Define some functions
#define PrintOpenGLError()::PrintOGLError(__FILE__, __LINE__)

using namespace std;

// Object related information
obj *objects;

// Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader, fragment_shader, p;
int illimunationMode = 0;
int shadingMode = 0;
int lightSource = 0;
int program = -1;

// Parameters for Copper (From: "Computer Graphics Using OpenGL" BY F.S. Hill, Jr.) 
//GLfloat ambient_cont[] = { 0.19125,0.0735,0.0225 };
GLfloat ambient_cont[] = { .1,.1,.1 };
GLfloat diffuse_cont[] = { 0.7038,0.27048,0.0828 };
//GLfloat specular_cont[] = { 0.256777,0.137622,0.086014 };
GLfloat specular_cont[] = { 1,1,1};
GLfloat exponent = 30;

// Projection, camera contral related declerations
int WindowWidth, WindowHeight;
bool LookAtObject = false;
bool ShowAxes = true;

// Camera declarations
float CameraRadius = 5;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;

// Mouse declarations
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;

// Program functionality variables
int algorithmIndex = 0;
int algorithmList[] = { TEXTURE_MAPPING, TEXTURE_MAPPING, TEXTURE_MAPPING, TEXTURE_MAPPING, TEXTURE_MAPPING, ENVIRONMENT_MAPPING, ENVIRONMENT_MAPPING, ENVIRONMENT_MAPPING, ENVIRONMENT_MAPPING, BUMP_MAPPING, BUMP_MAPPING }; // 11
int objectList[] = { PLANE, SPHERE, TEAPOT, SPHERE, TEAPOT, SPHERE, TEAPOT, SPHERE, TEAPOT, PLANE, SPHERE }; // 11
int mapList[] = { PLANE_MAP, PLANE_MAP, PLANE_MAP, SPHERE_MAP, SPHERE_MAP, SPHERE_MAP, SPHERE_MAP, CUBE_MAP, CUBE_MAP, PLANE_MAP, SPHERE_MAP }; // 11
GLuint texture_map, environment_map, bump_map;

/**
 * The display function for the GLUT Main Loop
 */
void DisplayFunc(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, (GLdouble)WindowWidth / WindowHeight, 0.01, 10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
		CameraRadius*cos(CameraPhi),
		CameraRadius*sin(CameraTheta)*sin(CameraPhi),
		0, 0, 0,
		0, 1, 0);

	glEnable(GL_DEPTH_TEST);
	if (mapList[algorithmIndex] == CUBE_MAP) {
		glEnable(GL_TEXTURE_CUBE_MAP);
	}
	else {
		glEnable(GL_TEXTURE_2D);
	}

	setParameters(program);

	obj curr = objects[objectList[algorithmIndex]];

	for (int i = 0; i < curr.faces; i++) {

		glBegin(GL_TRIANGLES);
		// Store face information here - vertex, normal, texture
		point v1, v2, v3, n1, n2, n3, t1, t2, t3;
		v1 = curr.vertList[curr.faceList[i].v1];
		v2 = curr.vertList[curr.faceList[i].v2];
		v3 = curr.vertList[curr.faceList[i].v3];
		n1 = curr.vertList[curr.faceList[i].v1];
		n2 = curr.vertList[curr.faceList[i].v2];
		n3 = curr.vertList[curr.faceList[i].v3];
		t1 = getTextureCoordinates(v1);
		t2 = getTextureCoordinates(v2);
		t3 = getTextureCoordinates(v3);

		// GL Functions for displaying this face
		glNormal3f(n1.x, n1.y, n1.z);
		glTexCoord2f(t1.x, t1.y);
		glVertex3f(v1.x, v1.y, v1.z);
		glNormal3f(n2.x, n2.y, n2.z);
		glTexCoord2f(t2.x, t2.y);
		glVertex3f(v2.x, v2.y, v2.z);
		glNormal3f(n3.x, n3.y, n3.z);
		glTexCoord2f(t3.x, t3.y);
		glVertex3f(v3.x, v3.y, v3.z);
		glEnd();

	}

	//glutSolidTeapot(1);
	//setParameters(program);
	glutSwapBuffers();
}

/**
 * Produces texture coordinates
 */
point getTextureCoordinates(point p) {
	point ret;

	// Plane mapping
	if (mapList[algorithmIndex] == PLANE_MAP) {
		ret.x = p.x/2 + 0.5;
		ret.y = p.y/2 + 0.5;
		ret.z = 0;
	}

	// Sphere mapping - Assume center is 0,0,0
	else if (mapList[algorithmIndex] == SPHERE_MAP) {

		// ? = atan2(-(z - cz), x - cx)
		float theta = atan(-p.z / p.x);

		// ? = acos(-(y - cy) / r);
		float phi = acos(-p.y);

		// u = (? + ?) / 2 ?
		ret.x = (theta + PI) / (2 * PI);

		// v = ? / ?;
		ret.y = phi / PI;

		ret.z = 0;
	}

	// Covers cube mapping
	else {
		// printf("This type of mapping is not implemented\n");
		ret = p;
	}

	return ret;
}

/**
 * The reshape function for the GLUT Main Loop
 */
void ReshapeFunc(int x,int y) {
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}

/**
 * The mouse function for the GLUT Main Loop
 */
void MouseFunc(int button,int state,int x,int y) {
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

/**
 * The motion function for the GLUT Main Loop
 */
void MotionFunc(int x, int y) {

	if(MouseLeft)
	{
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
		if (CameraPhi > (PI - 0.01))
			CameraPhi = PI - 0.01;
		if (CameraPhi < 0.01)
			CameraPhi = 0.01;
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

/**
 * Prints algorithm information about the
 * current algorithm settings
 */
void PrintAlgorithm(int alg) {
	printf("Algorithm #%d\n", algorithmIndex);
	switch (alg) {
	case 0:
		printf("texture mapping - plane, planar mapping\n");
		break;
	case 1:
		printf("texture mapping - sphere, planar mapping\n");
		break;
	case 2:
		printf("texture mapping - teapot, planar mapping\n");
		break;
	case 3:
		printf("texture mapping - sphere, spherical mapping\n");
		break;
	case 4:
		printf("texture mapping - teapot, spherical mapping\n");
		break;
	case 5:
		printf("environment mapping - sphere, sphere map\n");
		break;
	case 6:
		printf("environment mapping - teapot, sphere map\n");
		break;
	case 7:
		printf("environment mapping - sphere, cube map [EXTRA CREDIT]\n");
		break;
	case 8:
		printf("environment mapping - teapot, cube map [EXTRA CREDIT]\n");
		break;
	case 9:
		printf("bump mapping - plane\n");
		break;
	case 10:
		printf("bump mapping - sphere [EXTRA CREDIT]\n");
		break;
	default:
		break;
	}
}

/**
 * Motion and camera controls
 */
void KeyboardFunc(unsigned char key, int x, int y) {

    switch(key) {
	// Cycles through the different algorithms
	case 'A':
	case 'a':
		algorithmIndex = (algorithmIndex + 1) % NUM_OF_ALGORITHMS;
		PrintAlgorithm(algorithmIndex);
		break;
	// Quits the program
	case 'Q':
	case 'q':
		exit(1);
		break;
	// Changes the illumination mode
	case 'w':
	case 'W':
		if (illimunationMode == 0)
		{
			illimunationMode = 1;
		}
		else
		{
			illimunationMode = 0;
		}
		break;
	// Changes the shading mode
	case 'e':
	case 'E':
		if (shadingMode == 0)
		{
			shadingMode =1;
		}
		else
		{
			shadingMode =0;
		}
		break;
	// Changes the light source
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
	// Changes the color of the light source
	case 'f':
	case 'F':
		if (lightSource == 1)
		{
			//change color of the secondary light source at each key press, 
			//light color cycling through pure red, green, blue, and white.
		}
		break;
	// If none of the above, break
    default:
		break;
    }

	glutPostRedisplay();
}

/**
 * The main function
 */
int main(int argc, char **argv) {			  

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Assignment 8");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);
	
	setShaders();

	// Generate object list
	objects = (obj *)malloc(sizeof(obj)*3);

	// Initialize objects
	for (int i = 0; i < 3; i++) {
		objects[i].verts = objects[i].faces = objects[i].norms = 0;
	}

	// Create objects from files
	meshReader("plane.obj", 1, &objects[PLANE]);
	meshReader("sphere.obj", 1, &objects[SPHERE]);
	meshReader("teapot.obj", 1, &objects[TEAPOT]);

	// Print current algorithm to console
	PrintAlgorithm(algorithmIndex);

	glutMainLoop();

	return 0;
}

/*************************************************************
Shader related methods,
Setting the shader files
Setting the shader variables
*************************************************************/

/**
 * A helper method for reporting an error and exiting
 */
void error_exit(int status, char *text) {

	// Print error message
	fprintf(stderr,"Internal Error %i: ", status);
	fprintf(stderr,text);
	printf("\nTerminating as Result of Internal Error.\nPress Enter to exit.\n");

	// Keep the terminal open
	int anyKey = getchar();

	// Exit program
	exit(status);
}

/**
 * Reports a glError
 */
int PrintOGLError(char *file, int line) {
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

/**
 * A function for setting up the shaders
 */
void setShaders() {

	char *vs = NULL,*fs = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	

	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("sampleshader.vert");
	fs = shaderFileRead("sampleshader.frag");

	const char * vv = vs;
	const char * ff = fs;

	GLint       vertCompiled, fragCompiled;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSourceARB(vertex_shader, 1, &vv,NULL);
	glShaderSourceARB(fragment_shader, 1, &ff,NULL);

	free(vs);free(fs);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);

	glGetObjectParameterivARB(fragment_shader, GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
	glGetObjectParameterivARB(vertex_shader, GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);

    if (vertCompiled == GL_FALSE)
	{
        cout<<"vert not compiled"<<endl;

		GLint maxLog;
		glGetObjectParameterivARB(vertex_shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLog);

		//std::vector<char> errorLog(maxLog);
		char *errorLog;
		errorLog = (char*)malloc(maxLog);
		glGetInfoLogARB(vertex_shader, maxLog, &maxLog, &errorLog[0]);

		printf("%s", errorLog);
		glDeleteObjectARB(vertex_shader);
	}

	if (fragCompiled == GL_FALSE)
	{
		cout << "frag not compiled" << endl;

		GLint maxLog;
		glGetObjectParameterivARB(fragment_shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLog);

		//std::vector<char> errorLog(maxLog);
		char *errorLog;
		errorLog = (char*)malloc(maxLog);
		glGetInfoLogARB(fragment_shader, maxLog, &maxLog, &errorLog[0]);

		printf("%s", errorLog);
		glDeleteObjectARB(fragment_shader);
	}
	
	//create an empty program object to attach the shader objects
	p = glCreateProgramObjectARB();

	//attach the shader objects to the program object
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

	GLint result;
	glGetObjectParameterivARB(p, GL_OBJECT_LINK_STATUS_ARB, &result);

	if (!result) {
		printf("Failed to link file.\n");
	}

	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p);

	setParameters(p);

	program = p;
}

/**
 * Gets the location of the uniform variable given with "name" in the memory
 * and tests whether the process was successfull.
 * Returns the location of the queried uniform variable
 */
int getUniformVariable(GLuint program,char *name) {
	int location = glGetUniformLocationARB(program, name);
	
	if (location == -1)
	{
 		error_exit(1007, "No such uniform variable");
	}
	PrintOpenGLError();
	return location;
}

/**
 * A function for updating the light position
 */
void update_Light_Position() {
	
	// Create light components
	GLfloat light_position[] = { CameraRadius*cos(CameraTheta)*sin(CameraPhi),			  
			  CameraRadius*cos(CameraPhi), 
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi) - 1, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

/**
 * Sets the light positions, etc. parameters for the shaders
 */
void setParameters(GLuint program) {
	int light_loc;
	int ambient_loc, specular_loc;
	int exponent_loc;
	int mapping_loc;

	//sample variable used to demonstrate how attributes are used in vertex shaders.
	//can be defined as gloabal and can change per vertex
	float tangent = 0.0;
	float tangent_loc;

	// Compute tangents and bitangents

	// Loads the current texture based on algorithmIndex
	SetScene();
	update_Light_Position();

	//Access uniform variables in shaders
	ambient_loc = getUniformVariable(program, "AmbientContribution");	
	glUniform3fvARB(ambient_loc,1, ambient_cont);

	specular_loc = getUniformVariable(program, "SpecularContribution");
	glUniform3fvARB(specular_loc,1,specular_cont);

	exponent_loc = getUniformVariable(program, "exponent");
	glUniform1fARB(exponent_loc,exponent);

	GLint mapping = algorithmList[algorithmIndex];
	mapping_loc = getUniformVariable(program, "mapping_mode");
	glUniform1fARB(mapping_loc, mapping);
	//printf("%d\n", algorithmList[algorithmIndex]);

	//Access attributes in vertex shader
	tangent_loc = glGetAttribLocationARB(program,"tang");
	glVertexAttrib1fARB(tangent_loc,tangent);
	

}

/**
 * Determines the state of the scene
 */
void SetScene() {

	// If we are texture mapping
	if (algorithmList[algorithmIndex] == TEXTURE_MAPPING) {

		// And if we are using a plane map
		if (mapList[algorithmIndex] == PLANE_MAP) {
			texture_map = LoadTexture(1, "./planartexturemap/abstract2.tga");
		}
		// And if we are using a sphere map
		else if (mapList[algorithmIndex] == SPHERE_MAP) {
			texture_map = LoadTexture(1, "./sphericaltexturemap/earth2.tga");
		}
		// Otherwise, this is not a valid scene
		else {
			printf("Not a valid scene\n");
		}
	}

	// If we are environment mapping
	else if (algorithmList[algorithmIndex] == ENVIRONMENT_MAPPING) {

		// And if we are using a sphere map
		if (mapList[algorithmIndex] == SPHERE_MAP) {
			environment_map = LoadTexture(1, "./sphericalenvironmentmap/house2.tga");
		}
		// And if we are using a cube map
		else if (mapList[algorithmIndex] == CUBE_MAP) {
			// Requires 6 pieces
			// "./cubicenvironmentmap/cm_back2.tga"
			environment_map = LoadTexture(6, "./cubicenvironmentmap/cm_back2.tga", "./cubicenvironmentmap/cm_bottom2.tga", "./cubicenvironmentmap/cm_front2.tga",
											"./cubicenvironmentmap/cm_left2.tga", "./cubicenvironmentmap/cm_right2.tga", "./cubicenvironmentmap/cm_top2.tga");
		}
		// Otherwise, this is not a valid scene
		else {
			printf("Not a valid scene\n");
		}
	}

	// If we are bump mapping
	else {

		// And if we are using a plane map
		if (mapList[algorithmIndex] == PLANE_MAP) {
			// Requires 3 pieces
			bump_map = LoadTexture(3, "./planarbumpmap/abstract2.tga", "./planarbumpmap/abstract_gray2.tga", "./planarbumpmap/blue_base2.tga");
		}
		// And if we are using a sphere map
		else if (mapList[algorithmIndex] == SPHERE_MAP) {
			// Requires 2 pieces
			bump_map = LoadTexture(2, "./sphericalbumpmap/earth2.tga", "./sphericalbumpmap/earth_gray2.tga");
		}
		// Otherwise, this is not a valid scene
		else {
			printf("Not a valid scene\n");
		}
	}
}

/****************************************************************
Utility methods:
texture file reader
shader file reader
mesh reader for object
****************************************************************/

void LoadCubeTexture(GLuint* id, uint* width, uint* height, char* filename, TGA** TGAImage, int index) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, *id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLenum side;

	switch (index) {
		// back
	case 0:
		side = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		break;
		// bottom
	case 1:
		side = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		break;
		// front
	case 2:
		side = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		break;
		// left
	case 3:
		side = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		break;
		// right
	case 4:
		side = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		break;
		// top
	case 5:
		side = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		break;
	}

	glTexImage2D(side, 0, GL_RGBA, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, (*TGAImage)->GetPixels());
	//gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP, 4, *width, *height, GL_RGBA, GL_UNSIGNED_BYTE, (*TGAImage)->GetPixels());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, *id);
}

void Load2DColorTexture(GLuint* id, uint* width, uint* height, char* filename, TGA** TGAImage, int index) {
	glBindTexture(GL_TEXTURE_2D, *id);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, *width, *height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (*TGAImage)->GetPixels());
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, *width, *height, GL_RGBA, GL_UNSIGNED_BYTE, (*TGAImage)->GetPixels());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, *id);
}

/**
 * Stores the bump map
 */
void Load2DBumpMap(GLuint* id, uint* width, uint* height, char* filename, TGA** TGAImage, int index) {
	glBindTexture(GL_TEXTURE_2D, *id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, *width, *height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, (*TGAImage)->GetPixels());
	glBindTexture(GL_TEXTURE_2D, *id);
}

/**
 * A helper method for building textures within LoadTexture
 */
void BuildTexture(GLuint* id, uint* width, uint* height, char* filename, TGA** TGAImage, int index) {

	// Load image from tga file
	*TGAImage = new TGA(filename);

	// Use to dimensions of the image as the texture dimensions
	*width = (*TGAImage)->GetWidth();
	*height = (*TGAImage)->GetHeigth();

	glGenTextures(1, id);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	// Bump Mapping
	if (algorithmList[algorithmIndex] == BUMP_MAPPING) {
		switch (index) {
		case 0:
			glActiveTextureARB(GL_TEXTURE0);
			Load2DColorTexture(id, width, height, filename, TGAImage, index);
			break;
		case 1:
			glActiveTextureARB(GL_TEXTURE1);
			Load2DBumpMap(id, width, height, filename, TGAImage, index);
			break;
		case 2:
			// Do nothing
			break;
		}
	}

	// Txture and environment mapping
	else {
		// Plane or sphere mapping
		if (mapList[algorithmIndex] == PLANE_MAP || mapList[algorithmIndex] == SPHERE_MAP) {
			glActiveTextureARB(GL_TEXTURE0);
			Load2DColorTexture(id, width, height, filename, TGAImage, index);
		}
		// Cube mapping
		else {
			glActiveTextureARB(GL_TEXTURE0);
			LoadCubeTexture(id, width, height, filename, TGAImage, index);
		}
	}
}

/**
 * Stores all the textures
 */
GLuint LoadTexture(int numOfTextures, char* filename...) {

	GLuint id;
	TGA *TGAImage;
	uint width, height;

	// The parameters for actual textures are changed
	for (int i = 0; i < numOfTextures; i++) {
		BuildTexture(&id, &width, &height, filename+i, &TGAImage, i);
	}

	delete TGAImage;

	return id;
}

/**
 * Read the shader files, given as parameter.
 */
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

/**
 * Reads in the vertices/faces for an object from a .obj file
 */
void meshReader (char *filename, int sign, obj *object) {
  float x,y,z,len;
  int i;
  char letter;
  point v1,v2,crossP;
  int ix,iy,iz;
  int *normCount;
  FILE *fp;

  //object = (obj *)malloc(sizeof(obj));

  fp = fopen(filename, "r");
  if (fp == NULL) { 
    printf("Cannot open %s\n!", filename);
    exit(0);
  }

  // Count the number of vertices and faces
  while(!feof(fp))
   {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      if (letter == 'v')
	  {
		  object->verts++;
	  }
      else
	  {
		  object->faces++;
	  }
   }

  fclose(fp);

  printf("verts : %d\n", object->verts);
  printf("faces : %d\n", object->faces);

  // Dynamic allocation of vertex and face lists
  object->faceList = (faceStruct *)malloc(sizeof(faceStruct)*object->faces);
  object->vertList = (point *)malloc(sizeof(point)*object->verts);
  object->normList = (point *)malloc(sizeof(point)*object->verts);

  fp = fopen(filename, "r");

  // Read the veritces
  for(i = 0;i < object->verts;i++)
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      object->vertList[i].x = x;
      object->vertList[i].y = y;
      object->vertList[i].z = z;
    }

  // Read the faces
  for(i = 0;i < object->faces;i++)
    {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      object->faceList[i].v1 = ix - 1;
      object->faceList[i].v2 = iy - 1;
      object->faceList[i].v3 = iz - 1;
    }
  fclose(fp);


  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*object->verts);
  for (i = 0;i < object->verts;i++)
    {
      object->normList[i].x = object->normList[i].y = object->normList[i].z = 0.0;
      normCount[i] = 0;
    }

  for(i = 0;i < object->faces;i++)
    {
      v1.x = object->vertList[object->faceList[i].v2].x - object->vertList[object->faceList[i].v1].x;
      v1.y = object->vertList[object->faceList[i].v2].y - object->vertList[object->faceList[i].v1].y;
      v1.z = object->vertList[object->faceList[i].v2].z - object->vertList[object->faceList[i].v1].z;
      v2.x = object->vertList[object->faceList[i].v3].x - object->vertList[object->faceList[i].v2].x;
      v2.y = object->vertList[object->faceList[i].v3].y - object->vertList[object->faceList[i].v2].y;
      v2.z = object->vertList[object->faceList[i].v3].z - object->vertList[object->faceList[i].v2].z;

      crossP.x = v1.y*v2.z - v1.z*v2.y;
      crossP.y = v1.z*v2.x - v1.x*v2.z;
      crossP.z = v1.x*v2.y - v1.y*v2.x;

      len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

      crossP.x = -crossP.x/len;
      crossP.y = -crossP.y/len;
      crossP.z = -crossP.z/len;

	  object->normList[object->faceList[i].v1].x = object->normList[object->faceList[i].v1].x + crossP.x;
	  object->normList[object->faceList[i].v1].y = object->normList[object->faceList[i].v1].y + crossP.y;
	  object->normList[object->faceList[i].v1].z = object->normList[object->faceList[i].v1].z + crossP.z;
	  object->normList[object->faceList[i].v2].x = object->normList[object->faceList[i].v2].x + crossP.x;
	  object->normList[object->faceList[i].v2].y = object->normList[object->faceList[i].v2].y + crossP.y;
	  object->normList[object->faceList[i].v2].z = object->normList[object->faceList[i].v2].z + crossP.z;
	  object->normList[object->faceList[i].v3].x = object->normList[object->faceList[i].v3].x + crossP.x;
	  object->normList[object->faceList[i].v3].y = object->normList[object->faceList[i].v3].y + crossP.y;
	  object->normList[object->faceList[i].v3].z = object->normList[object->faceList[i].v3].z + crossP.z;
	  normCount[object->faceList[i].v1]++;
	  normCount[object->faceList[i].v2]++;
	  normCount[object->faceList[i].v3]++;
    }
  for (i = 0;i < object->verts;i++)
    {
      object->normList[i].x = (float)sign*object->normList[i].x / (float)normCount[i];
	  object->normList[i].y = (float)sign*object->normList[i].y / (float)normCount[i];
	  object->normList[i].z = (float)sign*object->normList[i].z / (float)normCount[i];
    }

}
