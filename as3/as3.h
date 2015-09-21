#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL\glut.h>
#include <math.h>

// Structs

// Vertex and Face data structure sued in the mesh reader
// Feel free to change them
typedef struct _point {
	float x, y, z;
} point;

typedef struct _faceStruct {
	int v1, v2, v3;
	int n1, n2, n3;
} faceStruct;

// A 4x4 matrix for convenience
typedef float matrix4x4[4][4];

// Functions
void meshReader(char *filename, int sign);
void display(void);
void resize(int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void drawAxes(point origin);
void drawObject(faceStruct * faceList, point * vertList);
void mergeTransform(matrix4x4 m1, matrix4x4 m2, boolean world);
point multiplyMatrix(matrix4x4 m, point v);
void applyTransform();
void matrixIdentity(matrix4x4 identityMatrix);
void camRotate(int x, int y);
void camTranslate(int x, int y);
void camZoom(int y);



