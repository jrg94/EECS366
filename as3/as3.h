#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL\glut.h>
#include <math.h>

// Functions
void meshReader(char *filename, int sign);
void display(void);
void resize(int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);


