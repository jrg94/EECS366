#pragma once

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

// Functions
void meshReader(char *filename, int sign);
void display(void);
void resize(int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void draw(faceStruct * faceList, point * vertList);


