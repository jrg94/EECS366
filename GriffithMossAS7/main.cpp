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

#include <iostream>
#include <fstream>

#define ON 1
#define OFF 0

using namespace std;

// Global variables
int window_width, window_height;    // Window dimensions
float image_plane_size;				// Image plane size
float image_plane_distance;			// Image plane distance
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
void rayTrace();
void display(void);

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
			printf("A light has been added to the scene at %f, %f, %f\n", lightList[i].x, lightList[i].y, lightList[i].z);
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
			// Sphere constructor works but you can't assign element to array directly -> This solved it
			sphereList[i].center.x = x;
			sphereList[i].center.y = y;
			sphereList[i].center.z = z;
			sphereList[i].radius = radius;
			sphereList[i].amb_r = amb_r;
			sphereList[i].amb_g = amb_g;
			sphereList[i].amb_b = amb_b;
			sphereList[i].dif_r = dif_r;
			sphereList[i].dif_g = dif_g;
			sphereList[i].dif_b = dif_b;
			sphereList[i].spec_r = spec_r;
			sphereList[i].spec_g = spec_g;
			sphereList[i].spec_b = spec_b;
			sphereList[i].amb_k = amb_k;
			sphereList[i].dif_k = dif_k;
			sphereList[i].spec_k = spec_k;
			sphereList[i].spec_ex = spec_ex;
			sphereList[i].ind_ref = ind_ref;
			sphereList[i].refl_k = refl_k;
			sphereList[i].refr_k = refr_k;
			printf("A sphere has been added to the scene at %f, %f, %f\n", sphereList[i].center.x, sphereList[i].center.y, sphereList[i].center.z);
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
			meshList[i].amb_r = amb_r;
			meshList[i].amb_g = amb_g;
			meshList[i].amb_b = amb_b;
			meshList[i].dif_r = dif_r;
			meshList[i].dif_g = dif_g;
			meshList[i].dif_b = dif_b;
			meshList[i].spec_r = spec_r;
			meshList[i].spec_g = spec_g;
			meshList[i].spec_b = spec_b;
			meshList[i].amb_k = amb_k;
			meshList[i].dif_k = dif_k;
			meshList[i].spec_k = spec_k;
			meshList[i].spec_ex = spec_ex;
			meshList[i].ind_ref = ind_ref;
			meshList[i].refl_k = refl_k;
			meshList[i].refr_k = refr_k;
			meshList[i].Load(meshFilename, 1, x, y, z);
			printf("A mesh has been added to the scene at %f, %f, %f\n", x, y, z);
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
	double magnitude = 1000000000000; 

	// The return object
	junction ret;
	
	// Initialize ret
	ret.type = NONE;

	// Run through entire scene for spheres
	int i;
	for (i = 0; i < spheres; i++) {
		junction next = sphereList[i].junctions(*r);

		// If the intersection type is something other than none
		if (next.type != NONE) {
			// If intersection distance is less than max distance & intersection distance > 0
			if (next.magnitude < magnitude && next.magnitude > 0.0001) {
				magnitude = next.magnitude;
				ret = next;
			}
		}
	}

	// Run through entire scene for meshes
	for (i = 0; i < meshes; i++) {
		junction next = meshList[i].junctions(*r);

		// If the intersection type is something other than none
		if (next.type != NONE) {
			// If intersection distance is less than max distance & intersection distance > 0
			if (next.magnitude < magnitude && next.magnitude > 0.00001) {
				magnitude = next.magnitude;
				ret = next;
			}
		}
	}
	return ret;
}

/**
 * The ray tracing algorithm
 */
void rayTrace() {
	printf("Drawing the scene!\n");

	Color c;
	Ray *r;

	float width = fb->GetWidth() / 2.0;
	float height = fb->GetHeight() / 2.0;

	for (int y = 0; y < fb->GetHeight(); y++) {
		for (int x = 0; x < fb->GetWidth(); x++) {
			// Initialize ray
			r = new Ray();
			r->depth = 10;
			r->inside = false;
			r->direction.x = image_plane_size * (x - width + 0.5) / width;
			r->direction.y = image_plane_size * (y - height + 0.5) / height;
			r->direction.z = -image_plane_distance;

			// Fire the ray
			shootRay(r);

			// Compute colors of the ray
			r->computeVariables();

			// Store colors from recursive calculations
			c.r = r->r;
			c.g = r->g;
			c.b = r->b;

			// Load the current pixel with this color
			fb->SetPixel(x, y, c);
			delete r;
		}
	}
	printf("Finished drawing scene!\n");
}

/**
 * Produces a new reflected ray from an intersection
 */
void calcAndShootReflectedRay(junction intersect, Ray *r) {
	// if object is reflecting object
	if (intersect.element.refl_k > 0.0) {
		Ray *refl = new Ray();
		refl->depth = r->depth;
		intersect.normal.Normalize();

		// Handle negative sign based on location of ray
		double sign = 1.0;
		if (r->inside) {
			sign = -1.0;
		}

		// Calculate reflection vector and include in ray structure
		double RdotN = r->direction.Dot(intersect.normal);

		refl->direction.x = r->direction.x - (2.0 * RdotN * intersect.normal.x * sign);
		refl->direction.y = r->direction.y - (2.0 * RdotN * intersect.normal.y * sign);
		refl->direction.z = r->direction.z - (2.0 * RdotN * intersect.normal.z * sign);

		// Ray origin is assigned intersection
		refl->origin = intersect.origin;

		// Attenuate ray
		refl->krg = refl->krg * r->krg;

		// Assign reflected to this new ray
		r->reflected = refl;

		// Shoot ray
		shootRay(refl);
	}
}

/**
 * Produces a new refracted ray from an intersection
 */
void calcAndShootRefractedRay(junction intersect, Ray *r) {
	// If object is a refractng object
	if (intersect.element.refr_k > 0.0) {
		// Normalize vectors
		intersect.normal.Normalize();
		r->direction.Normalize();

		Ray *refr = new Ray();

		double sign = 1.0;
		double ind_ref;

		// If the ray is inside an object, change sign and assign index of refraction
		if (r->inside) {
			sign = -1.0;
			ind_ref = intersect.element.ind_ref;
		}
		// Otherwise, index of refraction is the inverse of the objects index
		else {
			ind_ref = 1.0 / intersect.element.ind_ref;
		}

		// Compute r dot n
		double RdotN = r->direction.Dot(intersect.normal);

		// Compute refraction constant
		double k = 1.0 - (ind_ref * ind_ref * (1.0 - (RdotN * RdotN)));

		// If refraction constant is greater than or equal to 0.0, build refracted ray
		if (k >= 0.0) {
			double j = sign * ind_ref * RdotN - sqrt(k);

			refr->direction.x = (j * sign * intersect.normal.x) - (ind_ref * r->direction.x);
			refr->direction.y = (j * sign * intersect.normal.y) - (ind_ref * r->direction.y);
			refr->direction.z = (j * sign * intersect.normal.z) - (ind_ref * r->direction.z);
			refr->direction.Normalize();

			refr->origin = intersect.origin;
			refr->inside = !r->inside;

			r->refracted = refr;
			refr->depth = r->depth;

			// Attenuate the ray
			refr->krg = refr->krg * r->krg;

			shootRay(refr);
		}
		// Otherwise, there is no refraction
		else {
			delete refr;
		}
	}
}

/**
 * Performs the local illumination color calculation
 */
void localColorCalc(float &r, float &g, float &b, junction intersect, Ray *ray) {
	// Ambient lighting constant
	double amb = 0.3;

	r = amb * intersect.element.amb_r * intersect.element.amb_k;
	g = amb * intersect.element.amb_g * intersect.element.amb_k;
	b = amb * intersect.element.amb_b * intersect.element.amb_k;

	int i;
	float light_r, light_g, light_b;
	// Accumulate color for every light source
	for (i = 0; i < lights; i++) {
		// Assign rgb colors of this light to temp variables
		light_r = lightList[i].r;
		light_g = lightList[i].g;
		light_b = lightList[i].b;
		
		// Incoming light
		Point L;
		// If lightsource is a directional light
		if (lightList[i].light_type == DIRECTIONAL_SOURCE) {
			L.x = -lightList[i].x;
			L.y = -lightList[i].y;
			L.z = -lightList[i].z;
		}
		// Otherwise, it's a Pointsource
		else {
			L.x = lightList[i].x - intersect.origin.x;
			L.y = lightList[i].y - intersect.origin.y;
			L.z = lightList[i].z - intersect.origin.z;
		}

		// Normalize L
		L.Normalize();

		// Create a new light ray for this light source
		Ray *temp = new Ray();
		temp->origin.x = lightList[i].x;
		temp->origin.y = lightList[i].y;
		temp->origin.z = lightList[i].z;
		temp->direction.x = intersect.origin.x - lightList[i].x;
		temp->direction.y = intersect.origin.y - lightList[i].y; 
		temp->direction.z = intersect.origin.z - lightList[i].z;

		junction temp1 = findJunctions(temp); // Having trouble with this TODO: fix
		// If junction type is not none, and junction is close to this junction, attenuate the light
		if (temp1.type != NONE &&
		   (temp1.origin.x - intersect.origin.x > 0.1 ||
			temp1.origin.y - intersect.origin.y > 0.1 ||
			temp1.origin.z - intersect.origin.z > 0.1)) {

			light_r = light_r * temp1.element.refr_k;
			light_g = light_g * temp1.element.refr_k;
			light_b = light_b * temp1.element.refr_k;
		}

		// Normalize normal
		intersect.normal.Normalize();

		// Compute NdotL
		float NdotL = L.Dot(intersect.normal);

		if (NdotL < 0) {
			NdotL = 0.0;
		}

		// Perfect Reflection
		Point R;
		// Normalize R
		R.x = L.x - (2.0 * NdotL * intersect.normal.x);
		R.y = L.y - (2.0 * NdotL * intersect.normal.y);
		R.z = L.z - (2.0 * NdotL * intersect.normal.z);
		R.Normalize();

		Point V;
		// Normalize V
		V.x = -ray->direction.x;
		V.y = -ray->direction.y;
		V.z = -ray->direction.z;
		V.Normalize();

		float RdotV = R.Dot(V);
		float RdotV_exp = pow(RdotV, intersect.element.spec_ex);

		// If no intersection occurs, all intersection terms are zero
		r = r + light_r * ((intersect.element.dif_k * intersect.element.dif_r * NdotL) + (intersect.element.spec_k * intersect.element.spec_r * RdotV_exp));
		g = g + light_g * ((intersect.element.dif_k * intersect.element.dif_g * NdotL) + (intersect.element.spec_k * intersect.element.spec_g * RdotV_exp));
		b = b + light_b * ((intersect.element.dif_k * intersect.element.dif_b * NdotL) + (intersect.element.spec_k * intersect.element.spec_b * RdotV_exp));
		delete temp;
	}
}

/**
 * Fires a ray
 */
void shootRay(Ray *r) {	
	// Normalize this ray's direction
	r->direction.Normalize();

	// Intersection test
	junction test = findJunctions(r);

	// If ray intersects an object
	if (test.type != NONE) {

		// Get reflectiom/refraction constants
		r->refl_k = test.element.refl_k;
		r->refr_k = test.element.refr_k;

		// Calculate local intensity
		localColorCalc(r->r, r->g, r->b, test, r);

		// Decrement current depth of trace
		r->depth = r->depth - 1;

		// If depth > 0
		if (r->depth > 0) {
			calcAndShootReflectedRay(test, r);
			calcAndShootRefractedRay(test, r);
		}
		else {
			r = NULL;
		}
	}
	// If ray doesn't intersect anything, delete it
	else {
		r = NULL;
	}
}

// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void display(void)
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
void resize(int x,int y)
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
void mouseButton(int button,int state,int x,int y)
{
   ;
}

//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void mouseMotion(int x, int y)
{
	;
}

/**
 * A method for moving the image plane
 */
void moveImagePlane(int change) {
	// Place bounds on the image plane distance
	if (image_plane_distance > 1 && image_plane_distance < 20) {
		image_plane_distance += change;
	}
	// Cover close case
	else if (image_plane_distance == 1 && change > 0) {
		image_plane_distance += change;
	} 
	// Handle far case
	else if (image_plane_distance == 20 && change < 0) {
		image_plane_distance += change;
	}
	// Otherwise, do nothing
	printf("Image Plane is now located at %f units in front of the camera\n", image_plane_distance);
}

/**
 * A helper function for changing image plane size
 */
void changeImagePlaneSize(int change) {
	// Place bounds on image plane size
	if (image_plane_size > 1 && image_plane_size < 15) {
		image_plane_size += change;
	}
	// Cover small case
	else if (image_plane_size == 1 && change > 0) {
		image_plane_size += change;
	}
	// Cover large case
	else if (image_plane_size == 15 && change < 0) {
		image_plane_size += change;
	}
	// Otherwise, do nothing
	printf("Image Plane is now of size %f\n", image_plane_size);
}

// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
	// Quit
    case 'q':
		exit(1);
		break;
	// Decrease fake pixel size
	case '-':
		fb->Resize(fb->GetHeight()/2, fb->GetWidth()/2);
		break;
	// Increase fake pixel size
	case '=':
		fb->Resize(fb->GetHeight()*2, fb->GetWidth()*2);
		break;
	// Redraw image
	case 'r':
		rayTrace();
		break;
	// Move image plane closer to camera
	case '[':
		moveImagePlane(-1);
		break;
	// Move image plane away from camera
	case ']':
		moveImagePlane(1);
		break;
	// Decrease x,y dimensions of image plane
	case ',':
		changeImagePlaneSize(-1);
		break;
	// Increase c,y dimensions of image plane
	case '.':
		changeImagePlaneSize(1);
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}

/**
 * Drives the program
 */
int main(int argc, char* argv[])
{    

	fb = new FrameBuffer(INITIAL_RES, INITIAL_RES);
	image_plane_distance = 8;
	image_plane_size = 5;

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

	//rayTrace();

    // Switch to main loop
    glutMainLoop();
    return 0;        
}
