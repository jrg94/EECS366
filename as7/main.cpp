/**
 * Author: Jeremy Griffith
 * Author: Evelyn Moss
 */

#include <stdio.h>
#include <stdlib.h>
#include <gl\glut.h>
#include <GL\GLU.h>
#include <GL\GL.h>
#include <math.h>

#include "frame_buffer.h"
#include "primitives.h"
#include "color.h"
#include "elements.h"
#include <vector>

#include <iostream>
#include <fstream>

#define ON 1
#define OFF 0

using namespace std;


// Global variables
int window_width, window_height;    // Window dimensions
const int INITIAL_RES = 400;		// Stores the initial resolution
FrameBuffer* fb;					// The framebuffer
int lights, spheres, meshes;		// Number of lights, spheres, and meshes in the system
Light *lightList;					// List of lights
Sphere *sphereList;					// List of spheres
Mesh *meshList;						// List of meshes

// Functions
void shootRay(Ray *r);
void layoutReader(char *filename);
void drawRect(double x, double y, double w, double h);
junction findJunctions(Ray *r);

/**
 * Reads a scene layout file
 */
void layoutReader(char *filename) {

	FILE *fp;

	// Opens the layout file
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open %s\n!", filename);
		exit(0);
	}

	// Reads the first line of the file (Format: < number of lights > < number of spheres > < number of triangle meshes >)
	if (!feof(fp)) {
		fscanf(fp, "%d %d %d\n", &lights, &spheres, &meshes);
	}

	// Initialize lists
	lightList = new Light[lights];
	sphereList = new Sphere[spheres];
	meshList = new Mesh[meshes];

	int i = 0;		// Holds index of light array
	char letter;	// Holds letter from line
	int light_type;	// Holds light_type
	float x, y, z;	// Holds coordinates of light
	float r, g, b;	// Holds color of light

	// Read lights: L < light type > < x y z > < R G B >
	while (i < lights && !feof(fp)) {
		fscanf(fp, "%c %d %f %f %f %f %f %f\n", &letter, &light_type, &x, &y, &z, &r, &g, &b);

		// If the letter is L -> assign the current light
		if (letter == 'L') {
			lightList[i].light_type = light_type;
			lightList[i].x = x;
			lightList[i].y = y;
			lightList[i].z = z;
			lightList[i].r = r;
			lightList[i].g = g;
			lightList[i].b = b;
		}
		// Otherwise, report an error
		else {
			printf("FileFormatException:%s - Missing an L\n!", filename);
			exit(0);
		}
		i++;
	}

	i = 0;
	float radius;					// Holds the radius of the sphere
	float amb_r, amb_g, amb_b;		// Holds the ambient rgb values
	float dif_r, dif_g, dif_b;		// Holds the diffuse rgb values
	float spec_r, spec_g, spec_b;	// Holds the specular rgb values
	float amb_k, dif_k, spec_k;		// Holds the k values for the lighting
	float spec_ex, ind_ref;			// Holds some lighting constants  
	float refl_k, refr_k;			

	// Read spheres: S < x y z > < radius > < R G B ambient > < R G B diffuse > < R G B specular > 
	// < k_ambient > < k_diffuse > < k_specular > < specular_exponent > < index of refraction > < k_reflective > < k_refractive >
	while (i < spheres && !feof(fp)) {
		fscanf(fp, "%c %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
			&letter, &x, &y, &z, &radius, &amb_r, &amb_g, &amb_b, &dif_r, &dif_g, &dif_b,
			&spec_r, &spec_g, &spec_b, &amb_k, &dif_k, &spec_k, &spec_ex, &ind_ref, &refl_k, &refr_k);

		// Initialize sphere traits
		if (letter == 'S') {
			Sphere s = Sphere(point(x, y, x), radius, amb_r, amb_g, amb_b, dif_r, dif_g, dif_b, spec_r,
								spec_g, spec_b, amb_k, dif_k, spec_k, spec_ex, ind_ref, refl_k, refr_k);
			sphereList[i] = s;
		}
		// Otherwise, spit out exception
		else {
			printf("FileFormatException:%s - Missing an S\n!", filename);
			exit(0);
		}
		i++;
	}

	i = 0;
	char meshFilename[255];
	float scale;
	float rotx, roty, rotz;

	// M < file.obj > < scale > < rotX rotY rotZ > < x y z > < R G B ambient > < R G B diffuse > < R G B specular > 
	// < k_ambient > < k_diffuse > < k_specular > < specular_exponent > < index of refraction > < k_reflective > < k_refractive >
	while (i < meshes && !feof(fp)) {
		fscanf(fp, "%c %s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
			&letter, &meshFilename, &scale, &rotx, &roty, &rotz, &x, &y, &z, &amb_r, &amb_g, &amb_b,
			&dif_r, &dif_g, &dif_b, &spec_r, &spec_g, &spec_b, &amb_k, &dif_k, &spec_k, &spec_ex, &ind_ref, &refl_k, &refr_k);

		if (letter == 'M') {
			Mesh m = Mesh(amb_r, amb_g, amb_b, dif_r, dif_g, dif_b, spec_r,
			spec_g, spec_b, amb_k, dif_k, spec_k, spec_ex, ind_ref, refl_k, refr_k);
			m.Load(meshFilename, 1);
			meshList[i] = m;
		}
		else {
			printf("FileFormatException:%s - Missing an M\n!", filename);
			exit(0);
		}
		i++;
	}
	fclose(fp);
}

/**
 * Draws a rectangle
 */
void drawRect(double x, double y, double w, double h)
{
	glVertex2f(x,y);
	glVertex2f(x+w,y);
	glVertex2f(x+w,y+h);
	glVertex2f(x, y+h);
}

/**
 * Computes the intersection of this ray 
 * with an object in the scene
 */
junction findJunctions(Ray *r) {
	// 1 billion is the magnitude of the travel distance for one ray
	double magnitude = 1000000000; 

	// The return object
	junction ret;
	
	// Initialize ret

	// Run through entire scene for spheres
	int i;
	for (i = 0; i < spheres; i++) {
		junction next = sphereList[i].junctions(*r);

		// If the intersection type something other than none
		if (next.type != NONE) {

			// If intersection distance is less than max distance & intersection distance > 0
			if (next.magnitude < magnitude && next.magnitude > 0.00001) {
				magnitude = next.magnitude;
				ret = next;
			}
		}
	}

	for (i = 0; i < meshes; i++) {
		// TODO: Write intersects function for meshes
	}
	return ret;
}

/**
 * The ray tracing algorithm
 */
void rayTrace() {
	Color c;
	Ray *r;

	for (int y = 0; y < fb->GetHeight(); y++) {
		for (int x = 0; x < fb->GetWidth(); x++) {
			// Initialize ray
			r = new Ray();
			

			// Fire the ray
			shootRay(r);
			
			// Compute colors of the ray
			r->computeVariables();

			// Store colors from recursive calculations
			c.r = r->r;
			c.g = r->g;
			c.b = r->b;

			// Load the current pixel with this color
			fb->SetPixel(y, x, c);
		}
	}
}

/**
 * Fires a ray
 */
void shootRay(Ray *r) {
	// Intersection test
	junction test = findJunctions(r);

	// if ray intersects an object
	if (test.type == NONE) {
		r = NULL;
		return;
	}

	// Assign constants to ray
	r->refl_k = test.element.refl_k;
	r->refr_k = test.element.refr_k;

	// TODO: calculate local intensity

	// decrement current depth of trace
	r->depth = r->depth - 1;

	//	if depth > 0
	if (r->depth > 0) {
		// TODO: calculate and shoot the reflected ray
		// TODO: calculate and shoot the refracted ray
	}
	else {
		r = NULL;
	}
}

// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void	display(void)
{
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	double w = 10/double(fb->GetWidth());
	double h = 10/double(fb->GetHeight());

	Color cl;
	glColor3f(0,0,1);

	glBegin(GL_QUADS);

	printf("width %d, height %d\n", fb->GetWidth(), fb->GetHeight());

	for(int y = 0; y < fb->GetHeight(); y++)
	{
		for(int x = 0; x < fb->GetHeight(); x++)
		{
			cl = fb->buffer[x][y].color;
			glColor3f(cl.r, cl.g, cl.b);

			drawRect(w*x, h*y, w, h);
		}
	}

	glEnd();
    glutSwapBuffers();
}


// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void	resize(int x,int y)
{
    glViewport(0,0,x,y);
    window_width = x;
    window_height = y;
    
    printf("Resized to %d %d\n",x,y);
}


// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseButton(int button,int state,int x,int y)
{
   ;
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y)
{
	;
}


// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case 'q':                           /* Quit */
		exit(1);
		break;
	case '-':
		fb->Resize(fb->GetHeight()/2, fb->GetWidth()/2);
		//BresenhamLine(fb, fb->GetWidth()*0.1, fb->GetHeight()*0.1, fb->GetWidth()*0.9, fb->GetHeight()*0.9, Color(1,0,0));
		break;
	case '=':
		fb->Resize(fb->GetHeight()*2, fb->GetWidth()*2);
		//BresenhamLine(fb, fb->GetWidth()*0.1, fb->GetHeight()*0.1, fb->GetWidth()*0.9, fb->GetHeight()*0.9, Color(1,0,0));
		break;
	case 'r':
		rayTrace();
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}


int main(int argc, char* argv[])
{    

	fb = new FrameBuffer(INITIAL_RES, INITIAL_RES);

	//BresenhamLine(fb, fb->GetWidth()*0.1, fb->GetHeight()*0.1, fb->GetWidth()*0.9, fb->GetHeight()*0.9, Color(1,0,0));

	layoutReader("red_sphere_and_teapot.rtl");

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Raytracer");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,10,0,10,-10000,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	rayTrace();

    // Switch to main loop
    glutMainLoop();
    return 0;        
}
