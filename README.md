# EECS-366-Assignment-3

Overview

This programming assignment will allow you to gain experience using transformations to control object positioning and camera placement. The scene will consist of a single 3D object loaded from a data file and two sets of coordinate axes that can be toggled on and off. One set of axes will be the world coordinate system, and the other will represent the local object coordinate system. You can assume that the local system starts out lined up with the world system. The user should be able to use the keyboard to translate, rotate, and scale the object. In particular, the user should have options to rotate the object around either the world coordinate axes or its local coordinate axes. The camera will be controlled with the mouse, but its behavior will be different than that of the camera you implemented in assignment #2. For this assignment, the camera motion will largely mimic that of a person moving through the scene. The details of the camera interface are summarized in the interface section below.

You cannot use the gluLookAt function or any OpenGL transformation commands, like glRotate or glTranslate, to transform the object or move the camera. You need to implement translation, rotation, and scale transformations manually and apply them directly to the object mesh.

You can use the same template program from assignment 2 that illustrates the use of simple keyboard, mouse, and drawing callback functions in the GLUT interface toolkit. Use this template as the basis for your implementation. We can also refer to the demo program from assignment 2 that implements some of the features required below in a similar fashion (note that we are using a different camera interface in this assignment).

Interface Details

The camera/navigation interface should work as follows:

LEFT mouse button - swings the viewing axis based on the mouse motion. This would be equivalent to turning your head side to side if the mouse moves horizontally and nodding your head up and down if the mouse moves vertically.
MIDDLE mouse button - moves the camera up and down in the plane perpendicular to its viewing axis. This is equivalent to moving sideways when the mouse moves horizontally and going up and down in a glass elevator when the mouse moves vertically. If the mouse moves diagonally, then these motions should be combined.
RIGHT mouse button - moves the camera forward and back along the viewing vector.

Use the following keys to control object transformations in your program:

NOTE : the following are with respect to the WORLD coordinate system

"4" : negative translation along x axis
"6" : positive translation along x axis
"8" : positive translation along y axis
"2" : negative translation along y axis
"9" : positive translation along z axis
"1" : negative translation along z axis
"[" : negative rotation around x axis
"]" : positive rotation around x axis
";" : negative rotation around y axis
"'" : positive rotation around y axis
"." : negative rotation around z axis
"/" : positive rotation around z axis
"=" : increase uniform scaling
"-" : decrease uniform scaling

NOTE : the following are with respect to the LOCAL object coordinate system

"i" : negative rotation around local x axis
"o" : positive rotation around local x axis
"k" : negative rotation around local y axis
"l" : positive rotation around local y axis
"m" : negative rotation around local z axis
"," : positive rotation around local z axis

Other keyboard commands :

"a" : toggle display of coordinate axes, should display world and local object coordinate axes
"c" : snap camera to pointing at the world origin and pointing at the object at alternating order
"p" : toggle between perspective and orthogonal projection (already in the template)
"q" : exit program

For full credit, all of the above functionality must be fully implemented!
