/**
 * The solution to assignment 3
 * Author: Jeremy Griffith
 * Author: Evelyn Moss
 */

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
int lastx = 0;	// Holds last x position
int lasty = 0;	// Holds last y position

//Saves the origin of the object for use in figuring out translation and rotation
point objectOrigin;
point origin;
matrix4x4 transform;
matrix4x4 viewTrans;

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
void translateObject(float tx, float ty, float tz, boolean world) {
	matrix4x4 transMat;

	// Setup matrix with identity
	matrixIdentity(transMat);

	transMat[0][3] = tx;
	transMat[1][3] = ty;
	transMat[2][3] = tz;

	mergeTransform(transMat, transform, world);
}

/**
 * Modifies current transform with a translation
 */
void rotateObject(point p1, point p2, float degrees, boolean world) {
	matrix4x4 rot;

	// Compute rotation variables
	float length = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z));
	float cosA = cosf(degrees);
	float oneC = 1 - cosA;
	float sinA = sinf(degrees);
	float x = (p2.x - p1.x) / length;
	float y = (p2.y - p1.y) / length;
	float z = (p2.z - p1.z) / length; 

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

	// Move object to origin
	if (world) {
		translateObject(-(p1.x), -(p1.y), -(p1.z), world);
	}
	else {
		translateObject(p1.x, p1.y, p1.z, world);
	}

	// Force changes on transform
	mergeTransform(rot, transform, world); 

	// Move object back
	if (world) {
		translateObject(p1.x, p1.y, p1.z, world);
	} 
	else {
		translateObject(-(p1.x), -(p1.y), -(p1.z), world);
	}
}

void scaleObject(float x, float y, float z, point p, boolean world) {
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
	mergeTransform(scale, transform, world);

	//checkTransform(transform, "TRANSFORM");

}

/**
 * Multiplies m2 by m1
 */
void mergeTransform(matrix4x4 m1, matrix4x4 m2, boolean world) {
	int row, col;
	matrix4x4 temp;

	if (world) {
		for (row = 0; row < 4; row++) {
			for (col = 0; col < 4; col++) {
				temp[row][col] = m1[row][0] * m2[0][col] + m1[row][1] * m2[1][col] + m1[row][2] * m2[2][col] + m1[row][3] * m2[3][col];
			}
		}
	}
	else {
		for (row = 0; row < 4; row++) {
			for (col = 0; col < 4; col++) {
				temp[row][col] = m2[row][0] * m1[0][col] + m2[row][1] * m1[1][col] + m2[row][2] * m1[2][col] + m2[row][3] * m1[3][col];
			}
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
		normList[j] = multiplyMatrix(transform, normList[j]);
	}
	objectOrigin = multiplyMatrix(transform, objectOrigin);
	origin = multiplyMatrix(viewTrans, origin);
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
	matrixIdentity(viewTrans);

	if (AXES) {
		drawAxes(origin);
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
		//gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
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

	int dirx;
	int diry;

	// dirx stores the difference between x and lastx
	dirx = lastx - x;
	// dir stores the differenc between y and last y
	diry = y - lasty;

	if (cam_rotate_flag) {
		camRotate(dirx, diry);
	}
	else if (cam_translate_flag) {
		camTranslate(dirx, diry);
	}
	else if (cam_zoom_flag) {
		camZoom(diry);
	}

	lastx = x;
	lasty = y;

	glutPostRedisplay();

	printf("Mouse is at %d, %d\n", x,y);
}

void camRotate(int x, int y) {
	matrix4x4 rot;
	//To rotate the camera, must rotate the scene by the inverse of the camera rotation instead.
	//The inverse of a rotaion matrix is its transpose.
	//Must be performed on "center of world" as well.
	
	// Compute rotation variables
	float length = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z));
	float cosA = cosf(degrees);
	float oneC = 1 - cosA;
	float sinA = sinf(degrees);
	float x = (p2.x - p1.x) / length;
	float y = (p2.y - p1.y) / length;
	float z = (p2.z - p1.z) / length;

	// Initialize rotation matrix
	matrixIdentity(rot);

	// Fill rotation matrix
	rot[0][0] = x*x*oneC + cosA;
	rot[1][0] = x*y*oneC - z*sinA;
	rot[2][0] = x*z*oneC + y*sinA;
	rot[0][1] = y*x*oneC + z*sinA;
	rot[1][1] = y*y*oneC + cosA;
	rot[2][1] = y*z*oneC - x*sinA;
	rot[0][2] = z*x*oneC - y*sinA;
	rot[1][2] = z*y*oneC + x*sinA;
	rot[2][2] = z*z*oneC + cosA;

	// Move object to origin
	if (world) {
		translateObject(-(p1.x), -(p1.y), -(p1.z), world);
	}
	else {
		translateObject(p1.x, p1.y, p1.z, world);
	}

	// Force changes on transform
	mergeTransform(rot, transform, world);

	// Move object back
	if (world) {
		translateObject(p1.x, p1.y, p1.z, world);
	}
	else {
		translateObject(-(p1.x), -(p1.y), -(p1.z), world);
	}
}

/**
 * Treats the camera like a person walking side-to-side, up/down
 */
void camTranslate(int x, int y) {

	matrix4x4 transMat;

	// Setup matrix with identity
	matrixIdentity(transMat);

	transMat[0][3] = x/100.0;
	transMat[1][3] = y/100.0;
	transMat[2][3] = 0;

	mergeTransform(transMat, transform, false);
	mergeTransform(transMat, viewTrans, false);
}

/**
 * Treats the camera like a person turning their head
 */
void camZoom(int y) {
	matrix4x4 scale;
	float zoom;

	zoom = 1 + y / 1000.0;

	// Initialize the scale matrix
	matrixIdentity(scale);

	// Populate scale matrix
	scale[0][0] = zoom;
	scale[0][3] = (1 - zoom) * origin.x;
	scale[1][1] = zoom;
	scale[1][3] = (1 - zoom) * origin.y;
	scale[2][2] = zoom;
	scale[2][3] = (1 - zoom) * origin.z;

	// Force changes
	mergeTransform(scale, transform, true);
	//mergeTransform(scale, viewTrans, true);
}

/**
 * This function is called whenever there is a keyboard input
 * key is the ASCII value of the key pressed
 * x and y are the location of the mouse
 */
void keyboard(unsigned char key, int x, int y) {
	point t;
	t.x = objectOrigin.x;
	t.y = objectOrigin.y;
	t.z = objectOrigin.z;

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
		// World -x translation
		translateObject(-1, 0, 0, true);
		break;
	case '6':
		// World +x translation
		translateObject(1, 0, 0, true);
		break;
	case '2':
		// World -y translation
		translateObject(0, -1, 0, true);
		break;
	case '8':
		// World +y translation
		translateObject(0, 1, 0, true);
		break;
	case '1':
		// World -z translation
		translateObject(0, 0, -1, true);
		break;
	case '9':
		// World +z translation
		translateObject(0, 0, 1, true);
		break;
	case '[':
		// World -x rotation
		ux.x = origin.x + 1;
		ux.y = origin.y;
		ux.z = origin.z;
		rotateObject(origin, ux, -.1, true);
		break;
	case ']':
		// World +x rotation
		ux.x = origin.x + 1;
		ux.y = origin.y;
		ux.z = origin.z;
		rotateObject(origin, ux, .1, true);
		break;
	case ';':
		// World -z rotation
		uy.x = origin.x;
		uy.y = origin.y + 1;
		uy.z = origin.z;
		rotateObject(origin, uy, -.1, true);
		break;
	case '\'': 
		// World +y rotation
		uy.x = origin.x;
		uy.y = origin.y + 1;
		uy.z = origin.z;
		rotateObject(origin, uy, .1, true);
		break;
	case '.':
		// World -z rotation
		uz.x = origin.x;
		uz.y = origin.y;
		uz.z = origin.z + 1;
		rotateObject(origin, uz, -.1, true);
		break;
	case '/':
		// World +z rotation
		uz.x = origin.x;
		uz.y = origin.y;
		uz.z = origin.z + 1;
		rotateObject(origin, uz, .1, true);
		break;
	case '=':
		// Decrease the size of the object
		scaleObject(.5, .5, .5, origin, true);
		break;
	case '-':
		// Increase the size of the object
		scaleObject(1.5, 1.5, 1.5, origin, true);
		break;
	case 'i':
		// Local -x rotation
		ux.x = objectOrigin.x + 1;
		ux.y = objectOrigin.y;
		ux.z = objectOrigin.z;
		rotateObject(objectOrigin, ux, -.1, false);
		break;
	case 'o':
		// Local +x rotation
		ux.x = objectOrigin.x + 1;
		ux.y = objectOrigin.y;
		ux.z = objectOrigin.z;
		rotateObject(objectOrigin, ux, .1, false);
		break;
	case 'k':
		// Local -y rotation
		uy.x = objectOrigin.x;
		uy.y = objectOrigin.y + 1;
		uy.z = objectOrigin.z;
		rotateObject(objectOrigin, uy, -.1, false);
		break;
	case 'l':
		// Local +y rotation
		uy.x = objectOrigin.x;
		uy.y = objectOrigin.y + 1;
		uy.z = objectOrigin.z;
		rotateObject(objectOrigin, uy, .1, false);
		break;
	case 'm':
		// Local -z rotation
		uz.x = objectOrigin.x;
		uz.y = objectOrigin.y;
		uz.z = objectOrigin.z + 1;
		rotateObject(objectOrigin, uz, -.1, false);
		break;
	case ',':
		// Local +z rotation
		uz.x = objectOrigin.x;
		uz.y = objectOrigin.y;
		uz.z = objectOrigin.z + 1;
		rotateObject(objectOrigin, uz, .1, false);
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