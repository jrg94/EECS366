#include "as3.h"

extern matrix4x4 transform;

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

	// Switch to main loop
	glutMainLoop();
	return 0;
}