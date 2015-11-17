#pragma once

#include <math.h>

// Define the various intersection types
#define NONE 0
#define SPHERE 1
#define MESH 2

#define Point_SOURCE 1
#define DIRECTIONAL_SOURCE 0

/**
 * The face struct
 */
typedef struct _faceStruct {
	int v1, v2, v3;
	int n1, n2, n3;
} faceStruct;

/**
* A Point clas
*/
class Point
{
public:
	double x, y, z, w;

	Point() { x = 0; y = 0; z = 0; w = 1; }
	Point(double xa, double ya, double za)
	{
		x = xa; y = ya; z = za; w = 1.0;
	}
	Point(double xa, double ya, double za, double wa)
	{
		x = xa; y = ya; z = za; w = wa;
	}
	void Normalize();
	float Dot(Point a);
};

/**
 * A class that represents a triangle
 */
class Triangle {
public:
	Point a, b, c;

	bool intersects(Point origin, Point direction, float t, float u, float v);
};

/**
 * A light class
 * L < light type > < x y z > < R G B >
 */
class Light {
public:
	int light_type;
	float x, y, z;
	float r, g, b;
};

/**
* A Ray class
*/
class Ray
{
public:
	// Fields
	Point origin;
	Point direction; 
	float r, g, b;	// Color of Ray
	int depth;		// Parameter to track trace depth
	double ind_ref;			// Holds the index of refraction for the ray
	double refl_k, refr_k;	// Reflected and Refracted constants
	double krg;
	Ray *reflected;	// Keeps track of the reflected Ray
	Ray *refracted;	// Keeps track of the refracted Ray
	bool inside;

	// Functions
	Ray();
	void computeVariables();
	const double Magnitude();
	void debug(char* message);
	void dumpData();
};

/**
 * The element class
 */
class Element {
public:
	// Fields
	double amb_r, amb_g, amb_b;
	double dif_r, dif_g, dif_b;
	double spec_r, spec_g, spec_b;
	double amb_k, dif_k, spec_k;
	double spec_ex, ind_ref;
	double refl_k, refr_k;

	// Functions
	Element();
	Element(double _amb_r, double _amb_g, double _amb_b, double _dif_r, double _dif_g, double _dif_b,
		double _spec_r, double _spec_g, double _spec_b, double _amb_k, double _dif_k, double _spec_k,
		double _spec_ex, double _ind_ref, double _refl_k, double _refr_k);
	~Element();
};

/**
* The intersection struct
*/
typedef struct _junction {
	int type;
	Element element;
	Point origin;
	Point normal;
	double magnitude;
} junction;

/**
 * Handles mesh objects
 */
class Mesh : public Element {
public:
	// Fields
	int verts;
	int faces; 
	int norms;
	Point* vertList;
	Point* normList;
	faceStruct *faceList;

	// Functions
	Mesh();
	Mesh(double _amb_r, double _amb_g, double _amb_b, double _dif_r, double _dif_g, double _dif_b,
		double _spec_r, double _spec_g, double _spec_b, double _amb_k, double _dif_k, double _spec_k,
		double _spec_ex, double _ind_ref, double _refl_k, double _refr_k);
	~Mesh();
	void Load(char *filename, int sign);
	junction junctions(Ray r);
};

/**
 * A class that represents a sphere object
 */
class Sphere : public Element {
public:
	// Fields
	Point center;
	double radius;

	// Functions
	Sphere();
	Sphere(Point _center, double _radius, double _amb_r, double _amb_g, double _amb_b, double _dif_r, double _dif_g, double _dif_b,
		double _spec_r, double _spec_g, double _spec_b, double _amb_k, double _dif_k, double _spec_k,
		double _spec_ex, double _ind_ref, double _refl_k, double _refr_k);
	junction junctions(Ray r);
};

