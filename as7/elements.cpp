#include "elements.h"
#include <stdlib.h>
#include <stdio.h>

// The mesh reader itself
// It can read *very* simple obj files
void Mesh::Load(char *filename, int sign)
{
	float x, y, z, len;
	int i;
	char letter;
	point v1, v2, crossP;
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
	vertList = (point *)malloc(sizeof(point)*verts);
	normList = (point *)malloc(sizeof(point)*verts);

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

Sphere::Sphere() {}

Sphere::Sphere(point _center, double _radius, double _amb_r, double _amb_g, double _amb_b, double _dif_r, double _dif_g, double _dif_b,
	double _spec_r, double _spec_g, double _spec_b, double _amb_k, double _dif_k, double _spec_k,
	double _spec_ex, double _ind_ref, double _refl_k, double _refr_k) : Element(_amb_r, _amb_g, _amb_b, _dif_r, _dif_g, _dif_b,
	_spec_r, _spec_g, _spec_b, _amb_k, _dif_k, _spec_k,
	_spec_ex, _ind_ref, _refl_k, _refr_k) {

	center = _center;
	radius = _radius;
}

