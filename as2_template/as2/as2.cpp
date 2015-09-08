// Author: Jeremy Griffith 
// Author: Evelyn Moss

#include <stdio.h>
#include <stdlib.h>
#include <GL\glut.h>
#include <math.h>

#define ON 1
#define OFF 0

// Global variables
int window_width, window_height;    // Window dimensions

int PERSPECTIVE = OFF;	
int AXES = ON;
int OBJECT = ON;

int lastx = 0;	// Holds last x position
int lasty = 0;	// Holds last y position

int rotx = 0;	// Holds the x rotation
int roty = 0;	// Holds the y rotation

double zoom_distance = 1;		// Holds the zoom distance

int zoom_flag = OFF;		// Holds the zoom state
int rotate_flag = OFF;		// Holds the rotate state

// Functions
void drawAxes(void);
void drawRedRectangle(void);
void drawBlueTetrahedron(void);
void drawGreenLine(void);

/**
 * Vertex and Face data structure used in the mesh reader
 * Feel free to change them
 */
typedef struct _point {
  float x,y,z;
} point;

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List

/**
 * The mesh reader itself
 * It can read *very* simple obj files
 */
void meshReader (char *filename,int sign)
{
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

  // Read the veritces
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
 * The display function. It is called whenever the window needs
 * redrawing (ie: overlapping window moves, resize, maximize)
 * You should redraw your polygons here
 */
void	display(void)
{
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glTranslatef(0, 0, -3);
	glRotatef(rotx, 0, 1, 0); 
	glRotatef(roty, 1, 0, 0); 
	glScalef(zoom_distance, zoom_distance, zoom_distance);

	if (AXES) {
		//Axes are on
		drawAxes();
	}

	if (OBJECT) {
		// Object is turned on

		// Draw a blue tetrahedron
		drawBlueTetrahedron();
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
void	resize(int x,int y)
{
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
 * Rotates the camera
 */
void rotation(int dirx, int diry)
{
	rotx = rotx + dirx;
	roty = roty + diry;

	// Limits x rotation to 180 degrees
	if (rotx > 180) {
		rotx = 180;
	}
	else if (rotx < 0) {
		rotx = 0;
	}

	// Limits y rotation to 180 degrees
	if (roty > 180) {
		roty = 180;
	}
	else if (roty < 0) {
		roty = 0;
	}

	//Redisplay object
	glutPostRedisplay();
}

/**
 * Zooms the camera
 */
void zoom(int direction)
{
	
	zoom_distance = zoom_distance + (direction / 20.0);

	// Limits zoom to half the size of the original object
	if (zoom_distance < .5) {
		zoom_distance = .5;
	}
	// Limits zoom to twice the size of the original object
	else if (zoom_distance > 2) {
		zoom_distance = 2;
	}

	//Redisplay object
	glutPostRedisplay();
}

/**
 * This function is called whenever the mouse is pressed or released
 * button is a number 0 to 2 designating the button
 * state is 1 for release 0 for press event
 * x and y are the location of the mouse (in window-relative coordinates)
 *
 * Button 0 - Left click
 * Button 1 - Scroll wheel click
 * Button 2 - Right click
 */
void	mouseButton(int button, int state, int x, int y)
{
	lastx = x;
	lasty = y;

	// If left click is pressed, set rotate flag to 1
	if (button == 0 && state == 0) {
		rotate_flag = ON;
	// If left click is released, set rotate flag to 0
	} else if (button == 0 && state == 1) {
		rotate_flag = OFF;
	// If right click is pressed, set zoom flag to 1
	} else if (button == 2 && state == 0) {
		zoom_flag = ON;
	// If right click is released, set zoom flag to 0
	} else if (button == 2 && state == 1) {
		zoom_flag = OFF;
	}

	//When button 0 is in state 0, use mouseMotion to figure out the direction to rotate
	//When button 2 is in state 0, use mouseMotion to figure out the direction to zoom
    printf("Mouse click at %d %d, button: %d, state %d\n",x,y,button,state);
	printf("Rotate state: %d\nZoom state: %d\n", rotate_flag, zoom_flag);
}


/**
 * This function is called whenever the mouse is moved with a mouse button held down.
 * x and y are the location of the mouse (in window-relative coordinates)
 *
 * Saves position data based on drag events
 */
void	mouseMotion(int x, int y)
{
	int dirx;
	int diry;

	// dirx stores the difference between x and lastx
	dirx = lastx - x;
	// dir stores the differenc between y and last y
	diry = y - lasty;
	

	// If in the rotate state, rotate
	if (rotate_flag == 1) {
		rotation(dirx, diry);
	}
	// If in the zoom state, zoom
	else if (zoom_flag == 1) {
		zoom(diry);
	}

	lastx = x;
	lasty = y;

	//When moving left, rotate clockwise
	//When moving right, rotate counterclockwise
	//When moving up, zoom in
	//When moving down, zoom out
	printf("Mouse is at %d, %d\n", x,y);
}


/**
 * This function is called whenever there is a keyboard input
 * key is the ASCII value of the key pressed
 * x and y are the location of the mouse
 */
void	keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case 'q':                           /* Quit */
		exit(1);
		break;
    case 'p':
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
	case 'a':
		//Toggle Axes Display
		if (AXES) {
			//switch axes off
			AXES = OFF;
		}
		else {
			//switch axes on
			AXES = ON;
		}
		break;
	case 's':
		//Toggle Object Display
		if (OBJECT) {
			//switch object off
			OBJECT = OFF;
		}
		else {
			//switch object back on
			OBJECT = ON;
		}
		break;
    default:
		break;
    }

    //  Schedulea new display event
    glutPostRedisplay();
}

/**
 * A convenience function for drawing the axes
 */
void drawAxes(void)
{
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_LINES);
		glColor3f(0.5, 0.0, 0.0);
		glVertex3f(0, 0.0, 0.0);
		glVertex3f(2.0, 0.0, 0.0);

		glColor3f(0.0, 0.5, 0.0);
		glVertex3f(0.0, 2.0, 0.0);
		glVertex3f(0.0, 0, 0.0);

		glColor3f(0.0, 0.0, 0.5);
		glVertex3f(0.0, 0.0, 0);
		glVertex3f(0.0, 0.0, 2.0);
	glEnd();
}

/**
 * A convenience function for drawing a red rectangle
 */
void drawRedRectangle(void)
{
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
 * A convenience function for drawing a blue tetrahedron
 */
void drawBlueTetrahedron(void)
{
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
 * A convenience function for drawing a green line
 */
void drawGreenLine(void)
{
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
int main(int argc, char* argv[])
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Assignment 2 (orthogonal)");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);

    // Switch to main loop
    glutMainLoop();
    return 0;        
}
