#pragma once

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
	double amb_r, amb_g, amb_b;
	double dif_r, dif_g, dif_b;
	double spec_r, spec_g, spec_b;
	double amb_k, dif_k, spec_k;

};

// STRUCTS //

/**
 * The face struct
 */
typedef struct _faceStruct {
	int v1, v2, v3;
	int n1, n2, n3;
} faceStruct;

/**
 * L < light type > < x y z > < R G B >
 */
typedef struct Light {
	int light_type;
	float x, y, z;
	float r, g, b;
} Light;