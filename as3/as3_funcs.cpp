/**
 * The solution to assignment 3
 * Author: Jeremy Griffith
 * Author: Evelyn Moss
 */

#include <Windows.h>
#include "as3.h"
#include <stdlib.h>
#include <stdio.h>
#include <gl/glut.h>

#define ON 1
#define OFF 0
//Define the axes for easy reference
#define xaxis 0
#define yaxis 1
#define zaxis 2

// Global variables
int window_width, window_height;    // Window dimensions
int PERSPECTIVE = OFF;				// The on/off flag for perspective viewing
int AXES = ON;						// The on/off flag for axes
int cam_rotate_flag = OFF;			// The on/off flag for camera rotation
int cam_translate_flag = OFF;		// The on/off flag for camera translation
int cam_zoom_flag = OFF;			// The on/off flag for camera zoom

//Saves the origin of the object for use in figuring out translation and rotation
point objectOrigin;
matrix4x4 transform;

int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List

/**
 * The mesh reader itself
 * It can read *very* simple obj files
 */
void meshReader (char *filename,int sign) {
  float x,y,z,len;
  int i;
  char letter;
  point v1,v2,crossP;
  int ix,iy,iz;
  int *normCount;
  FILE *fp;

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
	  verts++;
	}
      else
	{
	  faces++;
	}
    }

  fclose(fp);

  printf("verts : %d\n", verts);
  printf("faces : %d\n", faces);

  // Dynamic allocation of vertex and face lists
  faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
  vertList = (point *)malloc(sizeof(point)*verts);
  normList = (point *)malloc(sizeof(point)*verts);

  fp = fopen(filename, "r");

  // Read the vertices
  for(i = 0;i < verts;i++)
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      vertList[i].x = x;
      vertList[i].y = y;
      vertList[i].z = z;
    }

  // Read the faces
  for(i = 0;i < faces;i++)
    {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      faceList[i].v1 = ix - 1;
      faceList[i].v2 = iy - 1;
      faceList[i].v3 = iz - 1;
    }
  fclose(fp);


  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*verts);
  for (i = 0;i < verts;i++)
    {
      normList[i].x = normList[i].y = normList[i].z = 0.0;
      normCount[i] = 0;
    }

  for(i = 0;i < faces;i++)
    {
      v1.x = vertList[faceList[i].v2].x - vertList[faceList[i].v1].x;
      v1.y = vertList[faceList[i].v2].y - vertList[faceList[i].v1].y;
      v1.z = vertList[faceList[i].v2].z - vertList[faceList[i].v1].z;
      v2.x = vertList[faceList[i].v3].x - vertList[faceList[i].v2].x;
      v2.y = vertList[faceList[i].v3].y - vertList[faceList[i].v2].y;
      v2.z = vertList[faceList[i].v3].z - vertList[faceList[i].v2].z;

      crossP.x = v1.y*v2.z - v1.z*v2.y;
      crossP.y = v1.z*v2.x - v1.x*v2.z;
      crossP.z = v1.x*v2.y - v1.y*v2.x;

      len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

      crossP.x = -crossP.x/len;
      crossP.y = -crossP.y/len;
      crossP.z = -crossP.z/len;

      normList[faceList[i].v1].x = normList[faceList[i].v1].x + crossP.x;
      normList[faceList[i].v1].y = normList[faceList[i].v1].y + crossP.y;
      normList[faceList[i].v1].z = normList[faceList[i].v1].z + crossP.z;
      normList[faceList[i].v2].x = normList[faceList[i].v2].x + crossP.x;
      normList[faceList[i].v2].y = normList[faceList[i].v2].y + crossP.y;
      normList[faceList[i].v2].z = normList[faceList[i].v2].z + crossP.z;
      normList[faceList[i].v3].x = normList[faceList[i].v3].x + crossP.x;
      normList[faceList[i].v3].y = normList[faceList[i].v3].y + crossP.y;
      normList[faceList[i].v3].z = normList[faceList[i].v3].z + crossP.z;
      normCount[faceList[i].v1]++;
      normCount[faceList[i].v2]++;
      normCount[faceList[i].v3]++;
    }
  for (i = 0;i < verts;i++)
    {
      normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
      normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
      normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
    }

}

/**
 * Generates a 4x4 identity matrix
 */
void matrixIdentity(matrix4x4 identityMatrix) {
	//float temp[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
	int row, col;

	for (row = 0; row < 4; row++) {
		for (col = 0; col < 4; col++) {
			if (col == row) {
				identityMatrix[row][col] = 1.0;
			}
			else {
				identityMatrix[row][col] = 0.0;
			}
		}
	}
}

/**
 * A debug function to peek at a matrix
 */
void checkTransform(matrix4x4 test, char * name) {
	int row, col;
	for (row = 0; row < 4; row++) {
		for (col = 0; col < 4; col++) {
			printf("%s:%d,%d: %f\n", name, row, col, test[row][col]);
		}
	}
}

/**
 * Modifies current transform with a translation
 */
void translateObject(float tx, float ty, float tz) {
	matrix4x4 transMat;

	// Setup matrix with identity
	matrixIdentity(transMat);

	transMat[0][3] = tx;
	transMat[1][3] = ty;
	transMat[2][3] = tz;

	// Multiply transform matrix by this matrix
	mergeTransform(transMat, transform);
}

/**
 * Modifies current transform with a translation
 */
void rotateObject(point p1, point p2, float degrees) {
	matrix4x4 rot;

	// Compute rotation variables
	float length = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z));
	float cosA = cosf(degrees);
	float oneC = 1 - cosA;
	float sinA = sinf(degrees);
	float x = (p2.x - p1.x) / length;
	float y = (p2.y - p1.y) / length;
	float z = (p2.z - p1.z) / length; 

	// Move object to origin
	translateObject(-(p1.x), -(p1.y), -(p1.z));

	// Initialize rotation matrix
	matrixIdentity(rot);

	// Fill rotation matrix
	rot[0][0] = x*x*oneC + cosA;
	rot[0][1] = x*y*oneC - z*sinA;
	rot[0][2] = x*z*oneC + y*sinA;
	rot[1][0] = y*x*oneC + z*sinA;
	rot[1][1] = y*y*oneC + cosA;
	rot[1][2] = y*z*oneC - x*sinA;
	rot[2][0] = z*x*oneC - y*sinA;
	rot[2][1] = z*y*oneC + x*sinA;
	rot[2][2] = z*z*oneC + cosA;

	// Force changes on transform
	mergeTransform(rot, transform);

	// Move object back 
	translateObject(p1.x, p1.y, p1.z);
}

void scaleObject(float x, float y, float z, point p) {
	matrix4x4 scale;

	// Initialize the scale matrix
	matrixIdentity(scale);

	checkTransform(transform, "TRANSFORM");

	// Populate scale matrix
	scale[0][0] = x;
	scale[0][3] = (1 - x) * p.x;
	scale[1][1] = y;
	scale[1][3] = (1 - y) * p.y;
	scale[2][2] = z;
	scale[2][3] = (1 - z) * p.z;

	printf("%f\n", p.z);

	// Force changes
	mergeTransform(scale, transform);

	checkTransform(transform, "TRANSFORM");

}

/**
 * Multiplies m2 by m1
 */
void mergeTransform(matrix4x4 m1, matrix4x4 m2) {
	int row, col;
	matrix4x4 temp;

	for (row = 0; row < 4; row++) {
		for (col = 0; col < 4; col++) {
			temp[row][col] = m1[row][0] * m2[0][col] + m1[row][1] * m2[1][col] + m1[row][2] * m2[2][col] + m1[row][3] * m2[3][col];
		}
	}

	for (row = 0; row < 4; row++) {
		for (col = 0; col < 4; col++) {
			m2[row][col] = temp[row][col];
		}
	}
}

/**
 * A function which performs the transformation on the vertex set
 */
void applyTransform() {
	for (int i = 0; i < verts; i++) {
		vertList[i] = multiplyMatrix(transform, vertList[i]);
	}
	for (int j = 0; j < norms; j++) {
		multiplyMatrix(transform, normList[j]);
	}
}

/**
 * Multiplies the input point v by the transformation matrix m
 */
point multiplyMatrix(matrix4x4 m, point v) {
	point temp;
	//printf("%f,%f,%f\n", v.x, v.y, v.z);
	temp.x = (m[0][0] * v.x) + (m[0][1] * v.y) + (m[0][2] * v.z) + m[0][3];
	temp.y = (m[1][0] * v.x) + (m[1][1] * v.y) + (m[1][2] * v.z) + m[1][3];
	temp.z = (m[2][0] * v.x) + (m[2][1] * v.y) + (m[2][2] * v.z) + m[2][3];
	//printf("%f,%f,%f\n", temp.x, temp.y, temp.z);
	return temp;
}

/**
 * The display function. It is called whenever the window needs
 * redrawing (ie: overlapping window moves, resize, maximize)
 * You should redraw your polygons here
 */
void display(void) {
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	applyTransform();
	drawObject(faceList, vertList);
	matrixIdentity(transform);

	if (AXES) {
		point orig;
		orig.x = 0;
		orig.y = 0;
		orig.z = 0;
		drawAxes(orig);
	}
   
	if (PERSPECTIVE) {
		// Perpective Projection 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (GLdouble)window_width / window_height, 0.01, 10000);
		glutSetWindowTitle("Assignment 2 Template (perspective)");
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		// Set the camera position, orientation and target
		gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
	}
	else {
		// Orthogonal Projection 
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-2.5, 2.5, -2.5, 2.5, -10000, 10000);
		glutSetWindowTitle("Assignment 2 Template (orthogonal)");
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // (Note that the origin is lower left corner)
    // (Note also that the window spans (0,1) )
    // Finish drawing, update the frame buffer, and swap buffers
    glutSwapBuffers();
}


/**
 * This function is called whenever the window is resized. 
 * Parameters are the new dimentions of the window
 */
void resize(int x,int y) {
    glViewport(0,0,x,y);
    window_width = x;
    window_height = y;
    if (PERSPECTIVE) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
    }
    printf("Resized to %d %d\n",x,y);
}


/**
 * This function is called whenever the mouse is pressed or released
 * button is a number 0 to 2 designating the button
 * state is 1 for release 0 for press event
 * x and y are the location of the mouse (in window-relative coordinates)
 */
void mouseButton(int button,int state,int x,int y) {

	// If left click is pressed, set rotate flag to 1
	if (button == 0 && state == 0) {
		cam_rotate_flag = ON;
	}
	// If left click is released, set rotate flag to 0
	else if (button == 0 && state == 1) {
		cam_rotate_flag = OFF;
	}
	// If mid click is pressed, set translate flag to 1
	else if (button == 1 && state == 0) {
		cam_translate_flag = ON;
	}
	// If mid click is released, set translate flag to 0
	else if (button == 1 && state == 1) {
		cam_translate_flag = OFF;
	}
	// If right click is pressed, set zoom flag to 1
	else if (button == 2 && state == 0) {
		cam_zoom_flag = ON;
	}
	// If right click is released, set zoom flag to 0
	else if (button == 2 && state == 1) {
		cam_zoom_flag = OFF;
	}

    printf("Mouse click at %d %d, button: %d, state %d\n",x,y,button,state);
}


/**
 * This function is called whenever the mouse is moved with a mouse button held down.
 * x and y are the location of the mouse (in window-relative coordinates)
 */
void mouseMotion(int x, int y) {
	printf("Mouse is at %d, %d\n", x,y);
}


/**
 * This function is called whenever there is a keyboard input
 * key is the ASCII value of the key pressed
 * x and y are the location of the mouse
 */
void keyboard(unsigned char key, int x, int y) {
	//point t; //Used for translations
	//t.x = objectOrigin.x;
	//t.y = objectOrigin.y;
	//t.z = objectOrigin.z;

	point orig;
	orig.x = 0;
	orig.y = 0;
	orig.z = 0;

	point ux;
	ux.x = 1;
	ux.y = 0;
	ux.z = 0;

	point uy;
	uy.y = 1;
	uy.x = 0;
	uy.z = 0;

	point uz;
	uz.z = 1;
	uz.x = 0;
	uz.y = 0;

    switch(key) {
    case '':
		// Exit failure
		exit(1);
		break;
	case 'q':								
		// Exit success
		exit(0);
		break;
	case 'a':
		// Toggle axes
		if (AXES) {
			AXES = OFF;
		} else {
			AXES = ON;
		}
		break;
	case 'p':
		// Toggle Projection Type (orthogonal, perspective)
		if (PERSPECTIVE) {
			// Switch from perspective to orthogonal
			PERSPECTIVE = OFF;
		} else {
			// Switch from orthogonal to perspective
			PERSPECTIVE = ON;
		}
		break;
	case '4':
		//Translation in the negative x direction
		translateObject(-1, 0, 0);
		break;
	case '6':
		//Translation in the positive x direction
		translateObject(1, 0, 0);
		break;
	case '2':
		//Translation in the negative y direction
		translateObject(0, -1, 0);
		break;
	case '8':
		//Translation in the positive y direction
		translateObject(0, 1, 0);
		break;
	case '1':
		//Translation in the negative z direction
		translateObject(0, 0, -1);
		break;
	case '9':
		//Translation in the positive z direction
		translateObject(0, 0, 1);
		break;
	case '[':
		//Negative rotation around the world's x axis
		rotateObject(orig, ux, -.1);
		break;
	case ']':
		//Positive rotation around the world's x axis
		rotateObject(orig, ux, .1);
		break;
	case ';':
		//Negative rotation around the world's y axis
		rotateObject(orig, uy, -.1);
		break;
	case '\'': //Make sure to test this thoroughly
		//Positive rotation around the world's y axis
		rotateObject(orig, uy, .1);
		break;
	case '.':
		//Negative rotation around the world's z axis
		//worldRotation(zaxis, -1);
		rotateObject(orig, uz, -.1);
		break;
	case '/':
		//Positive rotation around the world's z axis
		rotateObject(orig, uz, .1);
		break;
	case '=':
		//Decrease the size of the object
		scaleObject(.5, .5, .5, orig);
		break;
	case '-':
		//Increase the size of the object
		scaleObject(1.5, 1.5, 1.5, orig);
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}

/**
 * A function for drawing a red rectangle
 */
void drawRedRectangle() {
	// Draw a red rectangle
	glColor3f(1, 0, 0);
	glBegin(GL_POLYGON);
		glVertex3f(0.8, 0.8, -0.8);
		glVertex3f(0.8, -0.8, -0.8);
		glVertex3f(-0.8, -0.8, -0.0);
		glVertex3f(-0.8, 0.8, -0.0);
	glEnd();
}

/**
 * A function for drawing a blue tetrahedron
 */
void drawBlueTetrahedron() {
	// Draw a blue tetraheadron
	glColor3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
		glVertex3f(0.0, 1.6, 0.0);
		glVertex3f(0.8, -0.4, 0.8);
		glVertex3f(-0.8, -0.4, 0.8);

		glVertex3f(0.0, 1.6, 0.0);
		glVertex3f(0.8, -0.4, 0.8);
		glVertex3f(0.0, -0.4, -0.8);

		glVertex3f(0.0, 1.6, 0.0);
		glVertex3f(0.0, -0.4, -0.8);
		glVertex3f(-0.8, -0.4, 0.8);

		glVertex3f(-0.8, -0.4, 0.8);
		glVertex3f(0.8, -0.4, 0.8);
		glVertex3f(0.0, -0.4, -0.8);
	glEnd();
}

/**
 * A function for drawing a green line
 */
void drawGreenLine() {
	// Draw a green line
	glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		glVertex3f(1.8, 1.8, 0.0);
		glVertex3f(0.1, 0.1, 0.0);
	glEnd();
}

/**
* A convenience function for drawing the axes
*/
void drawAxes(point origin) {
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_LINES);
		glColor3f(0.5, 0.0, 0.0);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x + 2.0, origin.y, origin.z);

		glColor3f(0.0, 0.5, 0.0);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x, origin.y + 2.0, origin.z);

		glColor3f(0.0, 0.0, 0.5);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x, origin.y, origin.z + 2.0);
	glEnd();
}

/**
 * A function for drawing the vertex list
 */
void drawObject(faceStruct * faceList, point * vertList) {
	int i;

	glColor3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
		// Uses the face list to generate triangular faces
		for (i = 0; i < faces; i++) {
			glVertex3f(vertList[faceList[i].v1].x, vertList[faceList[i].v1].y, vertList[faceList[i].v1].z);
			glVertex3f(vertList[faceList[i].v2].x, vertList[faceList[i].v2].y, vertList[faceList[i].v2].z);
			glVertex3f(vertList[faceList[i].v3].x, vertList[faceList[i].v3].y, vertList[faceList[i].v3].z);
		}
	glEnd();

	if (AXES) {
		// Draws the axes based on the object origin
		drawAxes(objectOrigin);
	}
}

/**
* The main function
*/
/*int main(int argc, char* argv[]) {
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Assignment 3");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);

	// Initialize GL
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.5, 2.5, -2.5, 2.5, -10000, 10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	//Initialize origin
	objectOrigin.x = 0;
	objectOrigin.y = 0;
	objectOrigin.z = 0;

	//Test object
	drawBlueTetrahedron();

	// Switch to main loop
	glutMainLoop();
	return 0;
}*/

/**
* Function for multiplying a matrix with a point/vector.  Requires
* matrix and point/vector being multiplied, as well as an indicator
* for whether the matrix is being multiplied by a point or a vector.
* For 4x4 * 4x1 multiplications.
*/
/*
point matrixMult(matrix4x4 mat, point mult, bool isPoint) {
	//Performs matrix multiplication of the matrix and the point/vector.
	point store; //stores result until it can be returned.
	float x, y, z, w; //result in homogeneous coordinates
	if (isPoint) {
		w = 1;
	}
	else {
		w = 0;
	}
	x = (mat[0][0] * mult.x) + (mat[1][0] * mult.y) + (mat[2][0] * mult.z) + (mat[3][0] * w);
	y = (mat[0][1] * mult.x) + (mat[1][1] * mult.y) + (mat[2][1] * mult.z) + (mat[3][1] * w);
	z = (mat[0][2] * mult.x) + (mat[1][2] * mult.y) + (mat[2][2] * mult.z) + (mat[3][2] * w);
	store.x = x;
	store.y = y;
	store.z = z;
	return store;
}*/

/**
* The translation function.  It is called whenever the object
* is translated.  Requires a point to which the object is to be translated.
*/
/*
void translation(point t) {
	//Form the translation matrix
	matrix4x4 translate;
	//Start with a 3x3 identity matrix
	for (int i = 0; i < 3; i++) { //row
		for (int j = 0; j < 3; j++) { //column
			if (i == j) {
				translate[j][i] = 0;
			}
			else {
				translate[j][i] = 0;
			}
		}
	}
	//Then add the translation vector to the last column
	translate[3][0] = t.x;
	translate[3][1] = t.y;
	translate[3][2] = t.z;
	//Finally fill in the bottom row with all 0s except in the last column, which is a 1
	translate[0][3] = 0;
	translate[1][3] = 0;
	translate[2][3] = 0;
	translate[3][3] = 1;

	//Then multiply every point by the translation matrix (vectors cannot be translated)
	for (int i = 0; i < verts; i++) {
		vertList[i] = matrixMult(translate, vertList[i], true);
	}

	//Reset the object origin to match the new origin
	objectOrigin.x = t.x;
	objectOrigin.y = t.y;
	objectOrigin.z = t.z;
}*/

/**
* The rotation function for the world coordinates.  It
* is called only when the object is to be rotated based on the
* world coordinates.  Requires the axis to rotate around, the
* degrees to rotate.
*/
/*
void worldRotation(int axis, double degrees) {
	//Form the rotation matrix
	matrix4x4 rotate;
	//Start with 3x3 rotation matrix
	if (axis == xaxis) {
		rotate[0][0] = 1;
		rotate[0][1] = 0;
		rotate[0][2] = 0;
		rotate[1][0] = 0;
		rotate[1][1] = cos(degrees);
		rotate[1][2] = sin(degrees);
		rotate[2][0] = 0;
		rotate[2][1] = -sin(degrees);
		rotate[2][2] = cos(degrees);
	}
	else if (axis == yaxis) {
		rotate[0][0] = cos(degrees);
		rotate[0][1] = 0;
		rotate[0][2] = -sin(degrees);
		rotate[1][0] = 0;
		rotate[1][1] = 1;
		rotate[1][2] = 0;
		rotate[2][0] = sin(degrees);
		rotate[2][1] = 0;
		rotate[2][2] = cos(degrees);
	}
	else {
		rotate[0][0] = cos(degrees);
		rotate[0][1] = sin(degrees);
		rotate[0][2] = 0;
		rotate[1][0] = -sin(degrees);
		rotate[1][1] = cos(degrees);
		rotate[1][2] = 0;
		rotate[2][0] = 0;
		rotate[2][1] = 0;
		rotate[2][2] = 1;
	}
	//Then fill in the rest with 0s, with a 1 in the bottom right
	rotate[0][3] = 0;
	rotate[1][3] = 0;
	rotate[2][3] = 0;
	rotate[3][0] = 0;
	rotate[3][1] = 0;
	rotate[3][2] = 0;
	rotate[3][3] = 1;

	//Then, multiply all points and normals by the rotation matrix
	for (int i = 0; i < verts; i++) {
		vertList[i] = matrixMult(rotate, vertList[i], true);
	}
	for (int j = 0; j < norms; j++) {
		normList[j] = matrixMult(rotate, normList[j], false);
	}
}*/

/**
* The scaling function.  It is called whenever the object in
* question needs to be scaled.  Requires a size to scale to.
*/
/*
void scale(int size) {
	//Scale vertices and normals by the given size
	for (int i = 0; i < verts; i++) {
		vertList[i].x *= size;
		vertList[i].y *= size;
		vertList[i].z *= size;
	}
	for (int j = 0; j < norms; j++) {
		normList[j].x *= size;
		normList[j].y *= size;
		normList[j].z *= size;
	}
}*/