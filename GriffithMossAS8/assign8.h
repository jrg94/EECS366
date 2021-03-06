#ifndef __assign8_temp_h__
#define __assign8_temp_h__


#include <stdio.h>
#include <GL/glut.h>


// Vertex and Face data structure sued in the mesh reader
// Feel free to change them
typedef struct _point {
  float x,y,z;
} point;

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

typedef struct _obj {
	int verts, faces, norms;    // Number of vertices, faces and normals in the system
	point *vertList, *normList; // Vertex and Normal Lists
	faceStruct *faceList;
} obj;

point getTextureCoordinates(point p);

int getUniformVariable(GLuint program,char *name);
void update_Light_Position();
void setParameters(GLuint program);

GLuint LoadTexture(int numOfTextures, char* filename...);
void SetScene();

void error_exit(int status, char *text);
int PrintOGLError(char *file, int line);
void setShaders() ;
void meshReader (char *filename, int sign, obj *object);

#endif 