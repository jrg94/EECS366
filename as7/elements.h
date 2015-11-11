#pragma once

#include <math.h>

// STRUCTS //

/**
* The face struct
*/
typedef struct _faceStruct {
	int v1, v2, v3;
	int n1, n2, n3;
} faceStruct;

// CLASSES //

/**
* A point clas
*/
class point
{
public:
	double x, y, z, w;

	point() { x = 0; y = 0; z = 0; w = 1; }
	point(double xa, double ya, double za)
	{
		x = xa; y = ya; z = za; w = 1.0;
	}
	point(double xa, double ya, double za, double wa)
	{
		x = xa; y = ya; z = za; w = wa;
	}
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
* A ray class
*/
class ray
{
public:
	double x;
	double y;
	double z;

	// Empty constructor
	ray()
	{
		x = (0.0); y = (0.0); z = (0.0);
	}

	// Parameterized constructor
	ray(double _x, double _y, double _z)
	{
		x = (_x); y = (_y); z = (_z);
	}

	// A magnitude function
	const double Magnitude() const
	{
		return sqrt((x*x) + (y*y) + (z*z));
	}

	// A unit ray function
	const ray UnitRay() const
	{
		const double mag = Magnitude();
		return ray(x / mag, y / mag, z / mag);
	}
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
 * Handles mesh objects
 */
class Mesh : public Element {
public:
	// Fields
	int verts;
	int faces; 
	int norms;
	point* vertList;
	point* normList;
	faceStruct *faceList;

	// Functions
	Mesh();
	Mesh(double _amb_r, double _amb_g, double _amb_b, double _dif_r, double _dif_g, double _dif_b,
		double _spec_r, double _spec_g, double _spec_b, double _amb_k, double _dif_k, double _spec_k,
		double _spec_ex, double _ind_ref, double _refl_k, double _refr_k);
	~Mesh();
	void Load(char *filename, int sign);
};

class Sphere : public Element {
public:
	// Fields
	point center;
	double radius;

	// Functions
	Sphere();
	Sphere(point _center, double _radius, double _amb_r, double _amb_g, double _amb_b, double _dif_r, double _dif_g, double _dif_b,
		double _spec_r, double _spec_g, double _spec_b, double _amb_k, double _dif_k, double _spec_k,
		double _spec_ex, double _ind_ref, double _refl_k, double _refr_k);
};