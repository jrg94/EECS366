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
int PERSPECTIVE = OFF;

// Vertex and Face data structure sued in the mesh reader
// Feel free to change them
typedef struct _point {
  float x,y,z;
} point;

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

typedef float _4matrix[4][4]; // A 4x4 matrix for convenience

//Saves the origin of the object for use in figuring out translation and rotation
point objectOrigin;

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
 * Function for multiplying a matrix with a point/vector.  Requires
 * matrix and point/vector being multiplied, as well as an indicator
 * for whether the matrix is being multiplied by a point or a vector.  
 * For 4x4 * 4x1 multiplications.
 */
point matrixMult(_4matrix mat, point mult, bool isPoint) {
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
}



/**
 * The translation function.  It is called whenever the object 
 * is translated.  Requires a point to which the object is to be translated.
 */
void translation(point t) {
	//Form the translation matrix
	_4matrix translate;
	//Start with a 3x3 identity matrix
	for (int i = 0; i < 3; i++) { //row
		for (int j = 0; j < 3; j++) { //column
			if (i == j) {
				translate[j][i] = 1;
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
}

/**
 * The rotation function for the world coordinates.  It 
 * is called only when the object is to be rotated based on the
 * world coordinates.  Requires the axis to rotate around, the 
 * degrees to rotate.
 */
void worldRotation(int axis, double degrees) {
	//Form the rotation matrix
	_4matrix rotate;
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
}

/**
 * The scaling function.  It is called whenever the object in 
 * question needs to be scaled.  Requires a size to scale to.
 */
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
}



/**
 * The display function. It is called whenever the window needs
 * redrawing (ie: overlapping window moves, resize, maximize)
 * You should redraw your polygons here
 */
void display(void) {
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
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
	point t; //Used for translations
	t.x = objectOrigin.x;
	t.y = objectOrigin.y;
	t.z = objectOrigin.z;
    switch(key) {
    case '':                           /* Quit */
		exit(1);
		break;
    case 'P':
	// Toggle Projection Type (orthogonal, perspective)
		if (PERSPECTIVE) {
			// switch from perspective to orthogonal
			PERSPECTIVE = OFF;
		}
		else {
			// switch from orthogonal to perspective
			PERSPECTIVE = ON;
		}
		break;
	case '4':
		//Translation in the negative x direction
		t.x--;
		translation(t);
		break;
	case '6':
		//Translation in the positive x direction
		t.x++;
		translation(t);
		break;
	case '2':
		//Translation in the negative y direction
		t.y--;
		translation(t);
		break;
	case '8':
		//Translation in the positive y direction
		t.y++;
		translation(t);
		break;
	case '1':
		//Translation in the negative z direction
		t.z--;
		translation(t);
		break;
	case '9':
		//Translation in the positive z direction
		t.z++;
		translation(t);
		break;
	case '[':
		//Negative rotation around the world's x axis
		worldRotation(xaxis, -10);
		break;
	case ']':
		//Positive rotation around the world's x axis
		worldRotation(xaxis, 10);
		break;
	case ';':
		//Negative rotation around the world's y axis
		worldRotation(yaxis, -10);
		break;
	case '\'': //Make sure to test this thoroughly
		//Positive rotation around the world's y axis
		worldRotation(yaxis, 10);
		break;
	case '.':
		//Negative rotation around the world's z axis
		worldRotation(zaxis, -10);
		break;
	case '/':
		//Positive rotation around the world's z axis
		worldRotation(zaxis, 10);
		break;
	case '=':
		//Decrease the size of the object
		scale(.75);
		break;
	case '-':
		//Increase the size of the object
		scale(1.25);
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