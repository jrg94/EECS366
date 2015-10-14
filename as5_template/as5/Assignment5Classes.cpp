// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>
#include "Assignment5Classes.h"

using namespace std;

Vertex::Vertex()
{
	x = y = z = 0;
	h = 1;
}

void Vertex::Normalize()
{
	x = x/h;
	y = y/h;
	z = z/h;
	h = 1;
}

Object::Object()
{
	pBoundingBox = new Vertex[8];
	// Load the identity for the initial modeling matrix
	ModelMatrix[0] = ModelMatrix[5] = ModelMatrix[10] = ModelMatrix[15] = 1;
	ModelMatrix[1] = ModelMatrix[2] = ModelMatrix[3] = ModelMatrix[4] =
		ModelMatrix[6] = ModelMatrix[7] = ModelMatrix[8] = ModelMatrix[9] =
		ModelMatrix[11] = ModelMatrix[12] = ModelMatrix[13] = ModelMatrix[14]= 0;
}

Object::~Object()
{
	delete [] pBoundingBox;
	delete [] pVertexList;
	delete [] pFaceList;
}

// Load an object (.obj) file
void Object::Load(char* file, float s, float rx, float ry, float rz,
				  float tx, float ty, float tz)
{
	FILE* pObjectFile = fopen(file, "r");
	if(!pObjectFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char DataType;
	float a, b, c;

	// Scan the file and count the faces and vertices
	VertexCount = FaceCount = 0;
	while(!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%c %f %f %f\n", &DataType, &a, &b, &c);
		if(DataType == 'v')
            VertexCount++;
		else if(DataType == 'f')
			FaceCount++;
	}
	pVertexList = new Vertex[VertexCount];
	pFaceList = new Face[FaceCount];

	fseek(pObjectFile, 0L, SEEK_SET);

	cout << "Number of vertices: " << VertexCount << endl;
	cout << "Number of faces: " << FaceCount << endl;

	// Load and create the faces and vertices
	int CurrentVertex = 0, CurrentFace = 0;
	float MinimumX, MaximumX, MinimumY, MaximumY, MinimumZ, MaximumZ;
	while(!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%c %f %f %f\n", &DataType, &a, &b, &c);
		if(DataType == 'v')
		{
			pVertexList[CurrentVertex].x = a;
			pVertexList[CurrentVertex].y = b;
			pVertexList[CurrentVertex].z = c;
			// Track maximum and minimum coordinates for use in bounding boxes
			if(CurrentVertex == 0)
			{
				MinimumX = MaximumX = a;
				MinimumY = MaximumY = b;
				MinimumZ = MaximumZ = c;
			}
			else
			{
				if(a < MinimumX)
					MinimumX = a;
				else if(a > MaximumX)
					MaximumX = a;
				if(b < MinimumY)
					MinimumY = b;
				else if(b > MaximumY)
					MaximumY = b;
				if(c < MinimumZ)
					MinimumZ = c;
				else if(c > MaximumZ)
					MaximumZ = c;
			}
			CurrentVertex++;
		}
		else if(DataType == 'f')
		{
			// Convert to a zero-based index for convenience
			pFaceList[CurrentFace].v1 = (int)a - 1;
			pFaceList[CurrentFace].v2 = (int)b - 1;
			pFaceList[CurrentFace].v3 = (int)c - 1;
			CurrentFace++;
		}
	}

	// Initialize the bounding box vertices
	pBoundingBox[0].x = MinimumX; pBoundingBox[0].y = MinimumY; pBoundingBox[0].z = MinimumZ;
	pBoundingBox[1].x = MaximumX; pBoundingBox[1].y = MinimumY; pBoundingBox[1].z = MinimumZ;
	pBoundingBox[2].x = MinimumX; pBoundingBox[2].y = MaximumY; pBoundingBox[2].z = MinimumZ;
	pBoundingBox[3].x = MaximumX; pBoundingBox[3].y = MaximumY; pBoundingBox[3].z = MinimumZ;
	pBoundingBox[4].x = MinimumX; pBoundingBox[4].y = MinimumY; pBoundingBox[4].z = MaximumZ;
	pBoundingBox[5].x = MaximumX; pBoundingBox[5].y = MinimumY; pBoundingBox[5].z = MaximumZ;
	pBoundingBox[6].x = MinimumX; pBoundingBox[6].y = MaximumY; pBoundingBox[6].z = MaximumZ;
	pBoundingBox[7].x = MaximumX; pBoundingBox[7].y = MaximumY; pBoundingBox[7].z = MaximumZ;

	// Apply the initial transformations in order
	LocalScale(s);
	WorldRotate((float)(M_PI*rx/180.0), (float)(M_PI*ry/180.0), (float)(M_PI*rz/180.0));
	WorldTranslate(tx, ty, tz);	
}

// Do world-based translation
void Object::WorldTranslate(float tx, float ty, float tz)
{
	ModelMatrix[12] += tx;
	ModelMatrix[13] += ty;
	ModelMatrix[14] += tz;
}

// Perform world-based rotations in x,y,z order (intended for one-at-a-time use)
void Object::WorldRotate(float rx, float ry, float rz)
{
	float temp[16];

	if(rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[1] = temp[1]*cosx - temp[2]*sinx;
		ModelMatrix[2] = temp[2]*cosx + temp[1]*sinx;
		ModelMatrix[5] = temp[5]*cosx - temp[6]*sinx;
		ModelMatrix[6] = temp[6]*cosx + temp[5]*sinx;
		ModelMatrix[9] = temp[9]*cosx - temp[10]*sinx;
		ModelMatrix[10] = temp[10]*cosx + temp[9]*sinx;
		ModelMatrix[13] = temp[13]*cosx - temp[14]*sinx;
		ModelMatrix[14] = temp[14]*cosx + temp[13]*sinx;
	}

	if(ry != 0)
	{
		float cosy = cos(ry), siny = sin(ry);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosy + temp[2]*siny;
		ModelMatrix[2] = temp[2]*cosy - temp[0]*siny;
		ModelMatrix[4] = temp[4]*cosy + temp[6]*siny;
		ModelMatrix[6] = temp[6]*cosy - temp[4]*siny;
		ModelMatrix[8] = temp[8]*cosy + temp[10]*siny;
		ModelMatrix[10] = temp[10]*cosy - temp[8]*siny;
		ModelMatrix[12] = temp[12]*cosy + temp[14]*siny;
		ModelMatrix[14] = temp[14]*cosy - temp[12]*siny;
	}

	if(rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosz - temp[1]*sinz;
		ModelMatrix[1] = temp[1]*cosz + temp[0]*sinz;
		ModelMatrix[4] = temp[4]*cosz - temp[5]*sinz;
		ModelMatrix[5] = temp[5]*cosz + temp[4]*sinz;
		ModelMatrix[8] = temp[8]*cosz - temp[9]*sinz;
		ModelMatrix[9] = temp[9]*cosz + temp[8]*sinz;
		ModelMatrix[12] = temp[12]*cosz - temp[13]*sinz;
		ModelMatrix[13] = temp[13]*cosz + temp[12]*sinz;
	}
}

// Perform locally-based rotations in x,y,z order (intended for one-at-a-time use)
void Object::LocalRotate(float rx, float ry, float rz)
{
	float temp[16];

	if(rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[4] = temp[4]*cosx + temp[8]*sinx;
		ModelMatrix[5] = temp[5]*cosx + temp[9]*sinx;
		ModelMatrix[6] = temp[6]*cosx + temp[10]*sinx;
		ModelMatrix[7] = temp[7]*cosx + temp[11]*sinx;
		ModelMatrix[8] = temp[8]*cosx - temp[4]*sinx;
		ModelMatrix[9] = temp[9]*cosx - temp[5]*sinx;
		ModelMatrix[10] = temp[10]*cosx - temp[6]*sinx;
		ModelMatrix[11] = temp[11]*cosx - temp[7]*sinx;
	}

	if(ry != 0)
	{
        float cosy = cos(ry), siny = sin(ry);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosy - temp[8]*siny;
		ModelMatrix[1] = temp[1]*cosy - temp[9]*siny;
		ModelMatrix[2] = temp[2]*cosy - temp[10]*siny;
		ModelMatrix[3] = temp[3]*cosy - temp[11]*siny;
		ModelMatrix[8] = temp[8]*cosy + temp[0]*siny;
		ModelMatrix[9] = temp[9]*cosy + temp[1]*siny;
		ModelMatrix[10] = temp[10]*cosy + temp[2]*siny;
		ModelMatrix[11] = temp[11]*cosy + temp[3]*siny;
	}

	if(rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosz + temp[4]*sinz;
		ModelMatrix[1] = temp[1]*cosz + temp[5]*sinz;
		ModelMatrix[2] = temp[2]*cosz + temp[6]*sinz;
		ModelMatrix[3] = temp[3]*cosz + temp[7]*sinz;
		ModelMatrix[4] = temp[4]*cosz - temp[0]*sinz;
		ModelMatrix[5] = temp[5]*cosz - temp[1]*sinz;
		ModelMatrix[6] = temp[6]*cosz - temp[2]*sinz;
		ModelMatrix[7] = temp[7]*cosz - temp[3]*sinz;
	}
}

// Do locally-based uniform scaling
void Object::LocalScale(float s)
{
	for(int i = 0; i <= 11; i++)
        ModelMatrix[i] = s*ModelMatrix[i];
}

// Load a Scene (.dat) file
void Scene::Load(char* file)
{
	FILE* pSceneFile = fopen(file, "r");
	if(!pSceneFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char MeshFile[255];
	float Scaling;
	float RotationX, RotationY, RotationZ;
	float TranslationX, TranslationY, TranslationZ;

	// Step through the file and count the objects
	ObjectCount = 0;
	while(!feof(pSceneFile))
	{
		fscanf(pSceneFile, "%s %f %f %f %f %f %f %f\n", MeshFile, &Scaling,
			&RotationX, &RotationY, &RotationZ, &TranslationX, &TranslationY, &TranslationZ);
		ObjectCount++;
	}
	pObjectList = new Object[ObjectCount];

	fseek(pSceneFile, 0L, SEEK_SET);

	// Step through the file and create/load the objects
    for(int i = 0; i < ObjectCount; i++)
	{
		fscanf(pSceneFile, "%s %f %f %f %f %f %f %f\n", MeshFile, &Scaling,
			&RotationX, &RotationY, &RotationZ, &TranslationX, &TranslationY, &TranslationZ);
		pObjectList[i].Load(MeshFile, Scaling, RotationX, RotationY, RotationZ,
			TranslationX, TranslationY, TranslationZ);
	}

	cout << "Number of Objects Loaded: " << ObjectCount << endl;
}

Camera::Camera()
{
	Position.x = 10.0;
	Position.y = 0.0;
	Position.z = 0.0;

	Pitch = ((float) M_PI)/2;
	Yaw = 0.0;
	v.i = 0; v.j = 0; v.k = 1;
	EnforceVectors();

	ViewWidth = 10.0;
	ViewHeight = 10.0;

	FarPlane = 10.0;
	NearPlane = 2.0;
	ViewPlane = 8.0;

	LookAt();
}

// Move the camera with respect to its viewing vectors
void Camera::Move(float tx, float ty, float tz)
{
	Position.x += tz*n.i + ty*v.i + tx*u.i;
	Position.y += tz*n.j + ty*v.j + tx*u.j;
	Position.z += tz*n.k + ty*v.k + tx*u.k;
	LookAt();
}

// Pan the camera about its local axes
void Camera::Pan(float y, float p)
{
	Yaw += y;
	Pitch += p;
	EnforceVectors();
	LookAt();
}

// Scale the viewing plane
void Camera::ScaleView(float s)
{
	ViewWidth = s*ViewWidth;
	ViewHeight = s*ViewHeight;
}

// Move the viewing plane toward/away from the camera
void Camera::MoveView(float d)
{
	if(ViewPlane + d > 1.0)
        ViewPlane = ViewPlane + d;
}

// Set (and normalize) the camera vectors based on the viewing angles
void Camera::EnforceVectors()
{
	float magnitude;
	Vector temp;

	n.i = sin(Pitch)*cos(Yaw);
	n.j = sin(Pitch)*sin(Yaw);
	n.k = cos(Pitch);

	if(((n.i == 0) && (n.j == 0) && (n.k == 1)) || ((n.i == 0) && (n.j == 0) && (n.k == -1)))
	{
        v.i = 1; v.j = 0; v.k = 0;
	}
	else
	{
        v.i = 0; v.j = 0; v.k = 1;
	}

	temp.i = v.j*n.k - v.k*n.j;
	temp.j = v.k*n.i - v.i*n.k;
	temp.k = v.i*n.j - v.j*n.i;
	magnitude = sqrt(temp.i*temp.i + temp.j*temp.j + temp.k*temp.k);
	u.i = temp.i/magnitude; u.j = temp.j/magnitude; u.k = temp.k/magnitude;

	v.i = n.j*u.k - n.k*u.j;
	v.j = n.k*u.i - n.i*u.k;
	v.k = n.i*u.j - n.j*u.i;
}

// Calculate the new perspective projection matrix; maps into (-1,1)x(-1,1)x(0,1)
void Camera::Perspective()
{
    ProjectionMatrix[0] = ProjectionMatrix[5] = ProjectionMatrix[10] = ProjectionMatrix[15] = 1;
	ProjectionMatrix[1] = ProjectionMatrix[2] = ProjectionMatrix[3] = ProjectionMatrix[4] =
		ProjectionMatrix[6] = ProjectionMatrix[7] = ProjectionMatrix[8] = ProjectionMatrix[9] =
		ProjectionMatrix[11] = ProjectionMatrix[12] = ProjectionMatrix[13] = ProjectionMatrix[14]= 0;

	// Set the non-identity elements
	ProjectionMatrix[0] = 2*ViewPlane/ViewWidth;
	ProjectionMatrix[5] = 2*ViewPlane/ViewHeight;
	ProjectionMatrix[10] = FarPlane/(NearPlane - FarPlane);
	ProjectionMatrix[11] = -1.0;
	ProjectionMatrix[14] = FarPlane*NearPlane/(NearPlane - FarPlane);
	ProjectionMatrix[15] = 0.0;
}

// Calculate the new orthographic projection matrix; maps into (-1,1)x(-1,1)x(0,1)
void Camera::Orthographic()
{
	ProjectionMatrix[0] = ProjectionMatrix[5] = ProjectionMatrix[10] = ProjectionMatrix[15] = 1;
	ProjectionMatrix[1] = ProjectionMatrix[2] = ProjectionMatrix[3] = ProjectionMatrix[4] =
		ProjectionMatrix[6] = ProjectionMatrix[7] = ProjectionMatrix[8] = ProjectionMatrix[9] =
		ProjectionMatrix[11] = ProjectionMatrix[12] = ProjectionMatrix[13] = ProjectionMatrix[14]= 0;

	// Set the non-identity elements
	ProjectionMatrix[0] = 2/ViewWidth;
	ProjectionMatrix[5] = 2/ViewHeight;
	ProjectionMatrix[10] = NearPlane/(NearPlane - FarPlane);
}

// Calculate the new viewing transform matrix
void Camera::LookAt()
{
	ViewingMatrix[0] = u.i; ViewingMatrix[4] = u.j; ViewingMatrix[8] = u.k;
		ViewingMatrix[12] = -(u.i*Position.x + u.j*Position.y + u.k*Position.z);
	ViewingMatrix[1] = v.i; ViewingMatrix[5] = v.j; ViewingMatrix[9] = v.k;
		ViewingMatrix[13] = -(v.i*Position.x + v.j*Position.y + v.k*Position.z);
	ViewingMatrix[2] = n.i; ViewingMatrix[6] = n.j; ViewingMatrix[10] = n.k;
		ViewingMatrix[14] = -(n.i*Position.x + n.j*Position.y + n.k*Position.z);
	ViewingMatrix[3] = ViewingMatrix[7] = ViewingMatrix[11] = 0; ViewingMatrix[15] = 1;
}

// Transform a point with an arbitrary matrix
Vertex Transform(float* matrix, Vertex& point)
{
	Vertex temp;
	temp.x = matrix[0]*point.x + matrix[4]*point.y + matrix[8]*point.z + matrix[12]*point.h;
	temp.y = matrix[1]*point.x + matrix[5]*point.y + matrix[9]*point.z + matrix[13]*point.h;
	temp.z = matrix[2]*point.x + matrix[6]*point.y + matrix[10]*point.z + matrix[14]*point.h;
	temp.h = matrix[3]*point.x + matrix[7]*point.y + matrix[11]*point.z + matrix[15]*point.h;
	return temp;
}

// Select a new object by intersecting the selection ray with all object faces
int Select(int previous, Scene* pScene, Camera* pCamera, float x, float y)
{
	int select = previous;
	float zp = 10000, z;
	float s, t;
	Vertex temp1, temp2, temp3;

	for(int i = 0; i < pScene->ObjectCount; i++)
	{
		for(int j = 0; j < pScene->pObjectList[i].FaceCount; j++)
		{
			// Project each vertex into the standard view volume
			temp1 = pScene->pObjectList[i].pVertexList[pScene->pObjectList[i].pFaceList[j].v1];
			temp1 = Transform(pScene->pObjectList[i].ModelMatrix, temp1);
			temp1 = Transform(pCamera->ViewingMatrix, temp1);
			temp1 = Transform(pCamera->ProjectionMatrix, temp1);
			temp1.Normalize();

			temp2 = pScene->pObjectList[i].pVertexList[pScene->pObjectList[i].pFaceList[j].v2];
			temp2 = Transform(pScene->pObjectList[i].ModelMatrix, temp2);
			temp2 = Transform(pCamera->ViewingMatrix, temp2);
			temp2 = Transform(pCamera->ProjectionMatrix, temp2);
			temp2.Normalize();

			temp3 = pScene->pObjectList[i].pVertexList[pScene->pObjectList[i].pFaceList[j].v3];
			temp3 = Transform(pScene->pObjectList[i].ModelMatrix, temp3);
			temp3 = Transform(pCamera->ViewingMatrix, temp3);
			temp3 = Transform(pCamera->ProjectionMatrix, temp3);
			temp3.Normalize();

			// Use parametric equations to find the intersection between the selection ray
			// and the current (transformed) face
			t = ((temp1.x - temp3.x)*(y - temp3.y) - (temp1.y - temp3.y)*(x - temp3.x))/
				((temp1.x - temp3.x)*(temp2.y - temp3.y) + (temp1.y - temp3.y)*(temp3.x - temp2.x));
			if((temp1.x - temp3.x) != 0)
                s = (x - temp3.x + (temp3.x - temp2.x)*t)/(temp1.x - temp3.x);
			else
				s = (y - temp3.y+(temp3.y - temp2.y)*t)/(temp1.y - temp3.y);
			// Use the intersection if its valid (within the triangle) and closer
			if((s >= 0) && (t >= 0) && ((s + t) <= 1))
			{
				z = temp1.z*s + temp2.z*t + (1 - s - t)*temp3.z;
				if(z < zp)
				{
					zp = z;
					select = i;
				}
			}
		}
	}

	return select;
}

// Determines if point c is inside the border ab
bool InsideOf(Vertex a, Vertex b, Vertex c) {
	return (a.x - c.x) * (b.y - c.y) > (a.y - c.y) * (b.x - c.x);
}

// Calculates the intersection point 
Vertex intersection(Vertex a, Vertex b, Vertex c, Vertex d) {

	float a1 = b.y - a.y;
	float b1 = a.x - b.x;
	float c1 = a1 * a.x + b1 * a.y;

	float a2 = d.y - c.y;
	float b2 = c.x - d.x;
	float c2 = a2 * c.x + b2 * c.y;

	float det = a1 * b2 - b1 * a2;
	float x = (b2 * c1 - c2 * b1) / det;
	float y = (a1 * c2 - c1 * a2) / det;

	Vertex ret;
	ret.x = x;
	ret.y = y;
	//printf("%f, %f\n", ret.x, ret.y);

	return ret;
}

// Clip a polygon against view volume borders
// count = number of vertices in the polygon (In our case, a triangle)
// input = a pointer to a list of vertices
// out_count = number of vertices of the resulting polygon
// ADD CODE HERE: dummy function only copies polygons
Vertex* ClipPolygon(int count, Vertex* input, int* out_count, Camera* display)
{
	// Holds the output vertices
	Vertex* output = (Vertex*) calloc(count, sizeof(Vertex));

	// Loads input polygon vertices into output polygon vertex list
	for (int i = 0; i < count; i++) {
		//printf("%f, %f\n", input[i].x, input[i].y);
		output[i] = input[i];
	}

	*out_count = count;

	// Each of the corners of the clipping area
	Vertex topLeft, topRight, bottomLeft, bottomRight;

	// Initialize the top left corner
	topLeft.x = -display->ViewWidth / 2;
	topLeft.y = display->ViewHeight / 2;

	// Initialize the top right corner
	topRight.x = display->ViewWidth / 2;
	topRight.y = display->ViewHeight / 2;;

	// Initialize the bottom left corner
	bottomLeft.x = -display->ViewWidth / 2;
	bottomLeft.y = -display->ViewHeight / 2;;

	// Initialize the bottom right corner
	bottomRight.x = display->ViewWidth / 2;
	bottomRight.y = -display->ViewHeight / 2;;

	Vertex windowCorners[4] = { topLeft, topRight, bottomRight, bottomLeft };

	// For each edge in the clipping polygon
	for (int i = 0; i < 4; i++) {

		Vertex* inputList = new Vertex[count];

		// Store output list into input list
		for (int j = 0; j < count; j++) {
			inputList[j] = output[j];
		}

		// Reallocates the size of the output array
		output = (Vertex*)realloc(output, (count + 10) * sizeof(Vertex));

		Vertex edgePoint1 = windowCorners[(i + 4 - 1) % 4];
		Vertex edgePoint2 = windowCorners[i];

		// Keeps track of the index in the output array
		int added = 0;

		// For each point in the input list
		for (int j = 0; j < count; j++) {

			// Test points for the current segment
			Vertex point1 = inputList[(j + count - 1) % count];
			Vertex point2 = inputList[j];

			//printf("p1:%f,%f p2:%f,%f e1:%f,%f e2:%f,%f\n", point1.x, point1.y, point2.x, point2.y, edgePoint1.x, edgePoint1.y, edgePoint2.x, edgePoint2.y);

			// If point 2 is inside this edge
			if (InsideOf(edgePoint1, edgePoint2, point2)) {

				// If point 1 is outside of this edge
				if (!InsideOf(edgePoint1, edgePoint2, point1)) {
					// Compute intersection and add inner intersection to output
					Vertex intersect = intersection(edgePoint1, edgePoint2, point1, point2);
					//printf("%f, %f\n", intersect.x, intersect.y);
					output[added] = intersect;
					//printf("%f, %f\n", output[added].x, output[added].y);
					added++;
					*out_count = *out_count + 1;
				}

				// Add point 2 to output
				output[added] = point2;
				added++;
			}

			// Else if last is inside this edge
			else if (InsideOf(edgePoint1, edgePoint2, point1)) {
				// Compute intersection and add inner intersection to output
				Vertex intersect = intersection(edgePoint1, edgePoint2, point1, point2);
				output[added] = intersect;
				added++;
			}	
		}
		count = *out_count;	

		// To avoid memory leaks
		delete[] inputList;
		inputList = NULL;
	}
	
	
	return output;
}