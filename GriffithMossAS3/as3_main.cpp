#include "as3.h"

extern matrix4x4 transform;
extern matrix4x4 viewTrans;
extern point origin;
extern point objectOrigin;

/**
* The main function
*/
int main(int argc, char* argv[]) {
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Assignment 3");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);

	// Initialize GL
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.5, 2.5, -2.5, 2.5, -10000, 10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	//Test object
	meshReader("Objects\\helicopter.obj", 1);
	matrixIdentity(transform);
	matrixIdentity(viewTrans);
	origin.x = 0;
	origin.y = 0;
	origin.z = 0;
	objectOrigin.x = 0;
	objectOrigin.y = 0;
	objectOrigin.z = 0;

	// Switch to main loop
	glutMainLoop();
	return 0;
}