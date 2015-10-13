// Assignment5Template.cpp
// Computer Graphics - EECS 366/466 - Spring 2005

// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <GL\glut.h>
#include "Assignment5Classes.h"

using namespace std;

// User Interface Variables
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseMiddle = false;
bool MouseRight = false;
bool PerspectiveMode = true;
bool ShowAxes = true;
bool SelectionMode = false;
int SelectedObject = 0;
bool ShowBoundingBoxes = true;
int WindowWidth = 300, WindowHeight = 300;

// Scene Content Variables
Scene* pDisplayScene;
Camera* pDisplayCamera;

void DisplayFunc()
{
	int length;
	Vertex* input;
	Vertex* output;
	Vertex	temp,temp1,temp2,temp3;
	Vertex  orig,xunit,yunit,zunit;
	
	// vertices to be used in drawing the coordiante axes
	orig.x=orig.y=orig.z=xunit.y=xunit.z=yunit.x=yunit.z=zunit.x=zunit.y=0.0;
	orig.h=xunit.x=xunit.h=yunit.y=yunit.h=zunit.z=zunit.h=1.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(-1.1,1.1,-1.1,1.1);

	if(PerspectiveMode)
		pDisplayCamera->Perspective();
	else
		pDisplayCamera->Orthographic();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// draw worlc coordinate frames
	if(ShowAxes)
	{
		glLineWidth(3.0);
		glBegin(GL_LINES);

		temp = Transform(pDisplayCamera->ViewingMatrix,orig);
		temp1 = Transform(pDisplayCamera->ProjectionMatrix,temp);
		temp = Transform(pDisplayCamera->ViewingMatrix,xunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(1, 0, 0);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);

		temp = Transform(pDisplayCamera->ViewingMatrix,yunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(0, 1, 0);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);

		temp = Transform(pDisplayCamera->ViewingMatrix,zunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(0, 0, 1);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
		glEnd();
		glLineWidth(1.0);
	}

	// draw objects
	for(int i = 0; i < pDisplayScene->ObjectCount; i++)
	{
		// Color the selected object yellow and others blue
		if(i == SelectedObject)
			glColor3f(1, 1, 0);
		else
			glColor3f(0, 0, 1);
		// Clip and draw object faces
		for(int j = 0; j < pDisplayScene->pObjectList[i].FaceCount; j++)
		{
			input = new Vertex[3];
			input[0] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v1];
			input[1] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v2];
			input[2] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v3];

			for (int k=0; k<3; k++){
				temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,input[k]);
				temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
				input[k]= Transform(pDisplayCamera->ProjectionMatrix,temp2);
			}

			output = ClipPolygon(3, input, &length, pDisplayCamera);

			glBegin(GL_POLYGON);
			for(int k = 0; k < length; k++)
				glVertex2f(output[k].x/output[k].h, output[k].y/output[k].h);
			glEnd();

			delete [] input;
			input = NULL;
			delete [] output;
			output = NULL;
		}

		// Draw object coordinate frames
		if(ShowAxes)
		{
			glLineWidth(3.0);
			glBegin(GL_LINES);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,orig);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp2 = Transform(pDisplayCamera->ProjectionMatrix,temp1);
			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,xunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,yunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,zunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);
			glEnd();
			glLineWidth(1.0);
		}

		if(ShowBoundingBoxes)
		{
			// Color the selected object's bounding box red and others' blue
			if(i == SelectedObject)
				glColor3f(1, 0, 0);
			else
				glColor3f(0, 0, 1);
			// This code uses the object faces, not bounding boxes for ray casting,
			//  therefore bounding box drawing is skipped.
		} 
	}

	glutSwapBuffers();
}

void ReshapeFunc(int x,int y)
{
	// Get the correct view proportionality for the new window size
	pDisplayCamera->ViewWidth = pDisplayCamera->ViewWidth*((float)x/WindowWidth);
	pDisplayCamera->ViewHeight = pDisplayCamera->ViewHeight*((float)y/WindowHeight);
	glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}

void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !state;
	if(button == GLUT_MIDDLE_BUTTON)
		MouseMiddle = !state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !state;

	if(MouseLeft && SelectionMode)
	{
		// Select a new object with (x,y) mapped back to the (-1.1,1.1)x(-1.1,1.1) view volume
		SelectedObject = Select(SelectedObject, pDisplayScene, pDisplayCamera,
			2.2*(float)x/WindowWidth - 1.1, 1.1 - 2.2*(float)y/WindowHeight);
		glutPostRedisplay();
	}
}

void MotionFunc(int x, int y)
{
	if(MouseLeft && !SelectionMode)
		pDisplayCamera->Pan(((float) x - MouseX)/128, ((float) y - MouseY)/128);
	if(MouseMiddle && !SelectionMode)
		pDisplayCamera->Move(((float) MouseX - x)/32, ((float) y - MouseY)/32, 0);
	if(MouseRight && !SelectionMode)
		pDisplayCamera->Move(0, 0, ((float) y - MouseY)/32);

	if(MouseMiddle && SelectionMode)
	{
		// Move the Near Plane
		// Make sure near plane stays between camera and far plane
		// ADD CODE HERE

		// If moving near plane away from the camera
		if (y - MouseY > 1) {
			// If the near plane is further than a unit from the far plane, move it
			if (pDisplayCamera->NearPlane < pDisplayCamera->FarPlane + 1) {
				pDisplayCamera->NearPlane += y - MouseY;
			}
			else {
				// Do nothing
			}
		}
		// Otherwise, you're moving the camera toward the camera
		else {
			// If the near plane is further than a unit from the view plane, move it
			if (pDisplayCamera->NearPlane > pDisplayCamera->ViewPlane + 1) {
				pDisplayCamera->NearPlane += y - MouseY;
			}
			else {
				// Do nothing
			}
		}

	}
	if(MouseRight && SelectionMode)
	{
		// Move the Far Plane
		// Make sure far plane does not go in front of near plane
		// ADD CODE HERE

		// If moving the far plane away - move it
		if (y - MouseY > 1) {
			pDisplayCamera->FarPlane += y - MouseY;
		}
		// Otherwise, you're moving the far plane towards the near plane
		// Check that it doesn't go past the near plane
		else if (pDisplayCamera->FarPlane > pDisplayCamera->NearPlane + 1) {
			pDisplayCamera->FarPlane += y - MouseY;
		}
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}

void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'O':
	case 'o':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(M_PI/32, 0, 0);
		break;
	case 'I':
	case 'i':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(-M_PI/32, 0, 0);
		break;
	case 'L':
	case 'l':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, M_PI/32, 0);
		break;
	case 'K':
	case 'k':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, -M_PI/32, 0);
		break;
	case ',':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, 0, M_PI/32);
		break;
	case 'M':
	case 'm':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, 0, -M_PI/32);
		break;
	case '6':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(M_PI/32, 0, 0);
		break;
	case '4':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(-M_PI/32, 0, 0);
		break;
	case '8':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, M_PI/32, 0);
		break;
	case '2':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, -M_PI/32, 0);
		break;
	case '9':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, 0, M_PI/32);
		break;
	case '1':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, 0, -M_PI/32);
		break;
	case ']':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(M_PI/32, 0, 0);
		break;
	case '[':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(-M_PI/32, 0, 0);
		break;
	case 39:	// Apostrophe
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, M_PI/32, 0);
		break;
	case 59:	// Semicolon
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, -M_PI/32, 0);
		break;
	case '/':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, 0, M_PI/32);
		break;
	case '.':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, 0, -M_PI/32);
		break;
	case '=':
		pDisplayScene->pObjectList[SelectedObject].LocalScale(1.05);
		break;
	case '-':
		pDisplayScene->pObjectList[SelectedObject].LocalScale(0.95);
		break;
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'B':
	case 'b':
		ShowBoundingBoxes = !ShowBoundingBoxes;
		break;
	case 'N':
	case 'n':
		SelectionMode = !SelectionMode;
		if(SelectionMode)
			cout << "Selection Mode" << endl;
		else
			cout << "Camera Mode" << endl;
		break;
	case 'P':
	case 'p':
		PerspectiveMode = !PerspectiveMode;
		if(PerspectiveMode)
			glutSetWindowTitle("Assignment 5 (Perspective)");
		else
			glutSetWindowTitle("Assignment 5 (Orthogonal)");
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'Y':
	case 'y':
		pDisplayCamera->MoveView(0.5);
		break;
	case 'U':
	case 'u':
		pDisplayCamera->MoveView(-0.5);
		break;
	case 'H':
	case 'h':
		pDisplayCamera->ScaleView(0.95);
		break;
	case 'J':
	case 'j':
		pDisplayCamera->ScaleView(1.05);
		break;
    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    pDisplayScene = new Scene;
	pDisplayScene->Load("scene1.dat");
	pDisplayCamera = new Camera;
	pDisplayCamera->ViewWidth = (float)WindowWidth/32;
	pDisplayCamera->ViewHeight = (float)WindowHeight/32;

	// Initialize GLUT
    glutInit(&argc, argv);
	glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Assignment 5 (Perspective)");
    glutDisplayFunc(DisplayFunc);
    glutReshapeFunc(ReshapeFunc);
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

    // Switch to main loop
    glutMainLoop();

	return 0;
}