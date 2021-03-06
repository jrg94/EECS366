#include "elements.h"
#include <stdlib.h>
#include <stdio.h>

#define EPSILON .000001

/**
 * Normalizes a point (basically a vector)
 */
void Point::Normalize() {
	double magnitude = sqrt(x * x + y * y + z * z);
	x = x / magnitude;
	y = y / magnitude;
	z = z / magnitude;
}

/**
 * Provides dot product functionality for points
 */
float Point::Dot(Point a) {
	return (x * a.x) + (y * a.y) + (z * a.z);
}

/**
 * Provides cross product functionality for points
 */
Point Point::Cross(Point a) {
	Point ret;
	ret.x = y*a.z - z*a.y;
	ret.y = z*a.x - x*a.z;
	ret.z = x*a.y - y*a.x;
	return ret;
}

/**
 * Provides subtraction functionality for points
 */
Point Point::Sub(Point a) {
	Point ret;
	ret.x = x - a.x;
	ret.y = y - a.y;
	ret.z = z - a.z;
	return ret;
}

/** 
 * The mesh reader itself
 * It can read *very* simple obj files
 */
void Mesh::Load(char *filename, int sign, float x, float y, float z)
{
	float len;
	int i;
	char letter;
	Point v1, v2, crossP;
	int ix, iy, iz;
	int *normCount;
	FILE *fp;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open %s\n!", filename);
		exit(0);
	}

	verts = faces = 0;
	// Count the number of vertices and faces
	while (!feof(fp))
	{
		fscanf(fp, "%c %f %f %f\n", &letter, &x, &y, &z);
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
	vertList = (Point *)malloc(sizeof(Point)*verts);
	normList = (Point *)malloc(sizeof(Point)*verts);

	fp = fopen(filename, "r");

	// Read the veritces
	for (i = 0; i < verts; i++)
	{
		fscanf(fp, "%c %f %f %f\n", &letter, &x, &y, &z);
		vertList[i].x = x;
		vertList[i].y = y;
		vertList[i].z = z;
	}

	// Read the faces
	for (i = 0; i < faces; i++)
	{
		fscanf(fp, "%c %d %d %d\n", &letter, &ix, &iy, &iz);
		faceList[i].v1 = ix - 1;
		faceList[i].v2 = iy - 1;
		faceList[i].v3 = iz - 1;
	}
	fclose(fp);

	// The part below calculates the normals of each vertex
	normCount = (int *)malloc(sizeof(int)*verts);
	for (i = 0; i < verts; i++)
	{
		// Hardcoded translate
		Point temp;
		temp.x = 1.0 * vertList[i].x + 0.0 * vertList[i].y + 0.0 * vertList[i].z + x * 1.0;
		temp.y = 0.0 * vertList[i].x + 1.0 * vertList[i].y + 0.0 * vertList[i].z + y * 1.0;
		temp.z = 0.0 * vertList[i].x + 0.0 * vertList[i].y + 1.0 * vertList[i].z + z * 1.0;
		temp.w = 0.0 * vertList[i].x + 0.0 * vertList[i].y + 0.0 * vertList[i].z + 1.0 * 1.0;
		vertList[i] = temp;

		normList[i].x = normList[i].y = normList[i].z = 0.0;
		normCount[i] = 0;
	}

	for (i = 0; i < faces; i++)
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

		crossP.x = -crossP.x / len;
		crossP.y = -crossP.y / len;
		crossP.z = -crossP.z / len;

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
	for (i = 0; i < verts; i++)
	{
		normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
		normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
		normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
	}
}

/**
 * A ray constructor
 */
Ray::Ray() {
	reflected = NULL;
	refracted = NULL;
	r = g = b = 0;
	inside = false;
	krg = ind_ref = 1.0;
	refl_k = refr_k = 0.0;
}

/**
 * Handles behavior based on type of ray
 */
void Ray::computeVariables() {
	double refl_r, refl_g, refl_b;
	double refr_r, refr_g, refr_b;

	refl_r = refl_g = refl_b = refr_r = refr_g = refr_b = 0;

	// Computes constants for reflected ray
	if (reflected) {
		// Make a recursive call on the reflected ray
		reflected->computeVariables();
		refl_r = reflected->r;
		refl_g = reflected->g;
		refl_b = reflected->b;
	}

	// Computes constants for refracted ray
	if (refracted) {
		// Make a recursive call on the refracted ray
		refracted->computeVariables();
		refr_r = refracted->r;
		refr_g = refracted->g;
		refr_b = refracted->b;
	}
	
	// Compute color of this ray
	r = (r * krg) + (refl_r * refl_k) + (refr_r * refr_k);
	g = (g * krg) + (refl_g * refl_k) + (refr_g * refr_k);
	b = (b * krg) + (refl_b * refl_k) + (refr_b * refr_k);
}

/**
 * Computes the magnitude of a ray
 */
const double Ray::Magnitude() {
	float x = direction.x - origin.x;
	float y = direction.y - origin.y;
	float z = direction.z - origin.z;
	return sqrt((x*x) + (y*y) + (z*z));
}

/**
 * Helper method for printing out messages about a ray
 */
void Ray::debug(char* message) {
	printf("Ray %f, %f, %f: %s\n", direction.x, direction.y, direction.z, message);
}

/**
 * Helper function for dumping data about a ray
 */
void Ray::dumpData() {
	printf("Ray: r %f, g %f, b %f, krg %f, ind_ref %f\n", r, g, b, krg);
}

/**
 * Default constructor for element
 */
Element::Element() {}

/**
 * Standard constructor for element
 */
Element::Element(double _amb_r, double _amb_g, double _amb_b, double _dif_r, double _dif_g, double _dif_b,
	double _spec_r, double _spec_g, double _spec_b, double _amb_k, double _dif_k, double _spec_k,
	double _spec_ex, double _ind_ref, double _refl_k, double _refr_k) {

	amb_r = _amb_r;
	amb_g = _amb_g;
	amb_b = _amb_b;
	dif_r = _dif_r;
	dif_g = _dif_g;
	dif_b = _dif_b;
	spec_r = _spec_r;
	spec_g = _spec_g;
	spec_b = _spec_b;
	amb_k = _amb_k;
	dif_k = _dif_k;
	spec_k = _spec_k;
	spec_ex = _spec_ex;
	ind_ref = _ind_ref;
	refl_k = _refl_k;
	refr_k = _refr_k;
}

/**
 * Empty destructor
 */
Element::~Element() {}

/**
 * Empty destructor for Mesh
 */
Mesh::~Mesh() {
	delete[] vertList;
	delete[] faceList;
	delete[] boundingBox;
}

/**
 * Empty constructor for Mesh
 */
Mesh::Mesh() {}

Mesh::Mesh(double _amb_r, double _amb_g, double _amb_b, double _dif_r, double _dif_g, double _dif_b,
	double _spec_r, double _spec_g, double _spec_b, double _amb_k, double _dif_k, double _spec_k,
	double _spec_ex, double _ind_ref, double _refl_k, double _refr_k) : Element(_amb_r, _amb_g, _amb_b, _dif_r, _dif_g, _dif_b,
		_spec_r, _spec_g, _spec_b, _amb_k, _dif_k, _spec_k,
		_spec_ex, _ind_ref, _refl_k, _refr_k) {
}

/**
 * Computes intersection for meshes
 */
junction Mesh::junctions(Ray r) {
	junction ret;
	ret.type = NONE;

	int i;
	for (i = 0; i < faces; i++) {
		float t, u, v;

		Triangle temp = Triangle();
		temp.a = vertList[faceList[i].v1];
		temp.b = vertList[faceList[i].v2];
		temp.c = vertList[faceList[i].v3];

		if (temp.intersects(r.origin, r.direction, &t, &u, &v)) {
			junction intersection;
			intersection.type = MESH;
			intersection.magnitude = t;

			if ((ret.type == NONE || ret.magnitude > intersection.magnitude) && intersection.magnitude > .001) {
				intersection.element = (Element)*this;

				// Build intersection's origin
				intersection.origin.x = ((1 - u - v)*temp.a.x) + (u * temp.b.x) + (v * temp.c.x);
				intersection.origin.y = ((1 - u - v)*temp.a.y) + (u * temp.b.y) + (v * temp.c.y);
				intersection.origin.z = ((1 - u - v)*temp.a.z) + (u * temp.b.z) + (v * temp.c.z);

				intersection.origin.Normalize();

				// Build intersection's normal
				intersection.normal.x = ((1 - u - v)*normList[faceList[i].v1].x) + (u * normList[faceList[i].v2].x) + (v * normList[faceList[i].v3].x);
				intersection.normal.y = ((1 - u - v)*normList[faceList[i].v1].y) + (u * normList[faceList[i].v2].y) + (v * normList[faceList[i].v3].y);
				intersection.normal.z = ((1 - u - v)*normList[faceList[i].v1].z) + (u * normList[faceList[i].v2].z) + (v * normList[faceList[i].v3].z);

				// Normalize normal
				intersection.normal.Normalize();

				// Write this intersection back to the return intersection
				ret = intersection;
			}
		}
	}

	return ret;
}

Sphere::Sphere() {}

/**
 * Parameterized constructor
 */
Sphere::Sphere(Point _center, double _radius, double _amb_r, double _amb_g, double _amb_b, double _dif_r, double _dif_g, double _dif_b,
	double _spec_r, double _spec_g, double _spec_b, double _amb_k, double _dif_k, double _spec_k,
	double _spec_ex, double _ind_ref, double _refl_k, double _refr_k) : Element(_amb_r, _amb_g, _amb_b, _dif_r, _dif_g, _dif_b,
	_spec_r, _spec_g, _spec_b, _amb_k, _dif_k, _spec_k,
	_spec_ex, _ind_ref, _refl_k, _refr_k) {

	center = _center;
	radius = _radius;
}

/**
 * Returns an intersection on a sphere
 */
junction Sphere::junctions(Ray r) {
	// Create return junction
	junction ret;
	ret.type = NONE;

	// Hold axis magnitudes
	double magx = r.origin.x - center.x;
	double magy = r.origin.y - center.y;
	double magz = r.origin.z - center.z;

	// Quadratic Formula // -> Sphere can only be intersected twice
	// a = x*x + y*y + z*z
	double a = (r.direction.x * r.direction.x) + (r.direction.y * r.direction.y) + (r.direction.z * r.direction.z);

	// b = 2 * (x*magx + y*magy + z*magz)
	double b = 2.0 * ((r.direction.x * magx) + (r.direction.y * magy) + (r.direction.z * magz));

	// c = magx*magx + magy*magy + magz*magz - radius*radius
	double c = (magx * magx) + (magy * magy) + (magz * magz) - (radius * radius);

	// Calculate discriminant
	double discriminant = b * b - 4 * a * c;

	// Test the discriminant
	if (discriminant < 0.0) {
		return ret;
	}

	// Compute quadratic formula (-)
	double q = (-b - sqrt(discriminant)) / (2 * a);

	// Test quadratic formula -> This never evaluates (may be because of the location of the sphere)
	if (q > 0.0) {
		double inverse = 1.0 / radius;
		ret.origin.x = r.origin.x + q * r.direction.x;
		ret.origin.y = r.origin.y + q * r.direction.y;
		ret.origin.z = r.origin.z + q * r.direction.z;
		ret.normal.x = (ret.origin.x - center.x) / inverse;
		ret.normal.y = (ret.origin.y - center.y) / inverse;
		ret.normal.z = (ret.origin.z - center.z) / inverse;
		ret.magnitude = q;
		ret.element = (Sphere) *this;
		ret.type = SPHERE;
		return ret;
	}

	// Recompute quadratic formula (+)
	q = (-b + sqrt(discriminant)) / (2 * a);

	// Test quadratic formula
	if (q > 0.0) {
		double inverse = 1.0 / radius;
		ret.origin.x = r.origin.x + q * r.direction.x;
		ret.origin.y = r.origin.y + q * r.direction.y;
		ret.origin.z = r.origin.z + q * r.direction.z;
		ret.normal.x = (ret.origin.x - center.x) / inverse;
		ret.normal.y = (ret.origin.y - center.y) / inverse;
		ret.normal.z = (ret.origin.z - center.z) / inverse;
		ret.magnitude = q;
		ret.element = (Sphere)*this;
		ret.type = SPHERE;
		return ret;
	}

	return ret;
}

Triangle::Triangle() {
	a = Point();
	b = Point();
	c = Point();
}

/**
 * A triangle intersection test function
 */
bool Triangle::intersects(Point origin, Point direction, float *t, float *u, float *v) {

	// Build two edges
	Point edge1 = b.Sub(a);
	Point edge2 = c.Sub(a);

	// Determine the p vector
	Point pvec = direction.Cross(edge2);

	// Calculate the determinant
	float determinant = edge1.Dot(pvec);

	if (determinant > -EPSILON && determinant < EPSILON) {
		return false;
	}

	float inv_det = 1.0 / determinant;

	Point tvec = origin.Sub(a);

	*u = tvec.Dot(pvec) * inv_det;

	if (*u < 0.0 || *u > 1.0) {
		return false;
	}

	Point qvec = tvec.Cross(edge1);

	*v = direction.Dot(qvec) * inv_det;

	if (*v < 0.0 || *u + *v > 1.0) {
		return false;
	}

	*t = edge2.Dot(qvec) * inv_det;

	return true;
}

