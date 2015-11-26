========================================================================
    CONSOLE APPLICATION : as8 Project Overview
========================================================================

AppWizard has created this as8 application for you.

This file contains a summary of what you will find in each of the files that
make up your as8 application.


as8.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

as8.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

as8.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named as8.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

Instructions

For simplicity, you only need to display a single object in the scene without any modeling transformations, i.e. located at the center of the world coordinates, with no rotation, scaling etc. The objects that will be shown will be selected based on the algorithm, as given below. The camera interface needs to operate as in the second assignment. For simplicity, camera only needs to operate in perspective projection mode, with a 60 degrees field of view in the y direction, and an aspect ratio properly adjusted with the viewport (You can use gluLookat and gluPerspective functions). You will have a single pure white point light source located at the center of projection of the camera and moves with the camera. In addition to the point light sources, have a low intensity (0.1) white ambient light in the scene. You need to use a Phong local illumination model as the base illumination model of the object, and a Phong interpolative shading algorithm.

The first algorithm that you will implement is basic texture mapping. The objects provided do not already have texture coordinates, so you will need to generate them using planar and spherical intermediate surfaces (as we have discussed in class). You will use the texture map to modulate the diffuse reflection coefficient of the surface. The surface should have white specular highlights. Pick specular reflection coefficients to generate reasonable specular effects.

The second algorithm you will implement is environment mapping for approximating reflection effects. In this assignment, reflection vectors should be calculated per fragment, rather than calculating per vertex and interpolated for fragments. Simply assume a constant reflectivity of 1 over the whole surface.

The last algorithm that you will implement is bump mapping. You need to generate the bumps using the primary texture with first difference approximation of the derivative. For convenience, you can alternatively use a second grayscale texture to calculate the gradients from. As in the basic texture mapping case, primary texture modulates the diffuse reflection coefficient of the surface, and the surface should have white specular highlights. Note that, for bump mapping, you will need to generate and send tanget vectors to your shader in addition to the normals. For bump mapping on the plane, the tangent vectors will be constant for all the vertices, if the texture coordinates are assigned uniformly. You can use this to simplify your implementation. In the case of bump mapping on the sphere, the tangent vectors vary, and therefore needs to be specified per vertex.

Interface Details

Use the following keyboard commands in your program :
"a" : at each key press, cycle through algorithms in the order given below:
texture mapping - plane, planar mapping
texture mapping - sphere, planar mapping
texture mapping - teapot, planar mapping
texture mapping - sphere, spherical mapping
texture mapping - teapot, spherical mapping
environment mapping - sphere, sphere map
environment mapping - teapot, sphere map
environment mapping - sphere, cube map [EXTRA CREDIT]
environment mapping - teapot, cube map [EXTRA CREDIT]
bump mapping - plane
bump mapping - sphere [EXTRA CREDIT]

/////////////////////////////////////////////////////////////////////////////
