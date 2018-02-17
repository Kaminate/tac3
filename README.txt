/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: Notes to the grader
Language: C++, msvc 11 (visual studio 2012)
Platform: Windows, opengl 420
Project: nathan.p_CS350_3
Author: Nathan Park, nathan.p, 50005211
Creation date: January 26, 2015
End Header --------------------------------------------------------*/


a. How to use parts of your user interface that is NOT specified in the 
  assignment description.

  All relevent gui is under the BSPTree and Octree group

  Camera controls:
  - arrow keys to move
  - q/e to move up and down

b. Any assumption that you make on how to use the application that,
 if violated, might cause the application to fail.

  TO BUILD:
  - ogl 4.2 must be suppported
  - I don't save the .suo, so you must change the working directory from
    from $(ProjectDir)
    to $(ProjectDir)../

  ETC:
  - Only use the gui in the BoundingVolume group
  
  - If you generate the bounding hierarchies while the 
    "Top down use point cloud" box is checked, be prepared to wait



c. Which part of the assignment has been completed? 

  Everything

d. Which part of the assignment has NOT been completed 

  Nothing

e. Where the relevant source codes (both C++ and shaders) for the 
  assignment are located. Specify the file path (folder name),
  file name, function name (or line number).

  Relevent C++ files: 
    tac3graphics/src/tacWorldRenderTechnique.cpp
    tac3graphics/src/tacWorldRenderTechnique.h
  Visual Studio Solution:
    tac3editor/vs2012/tac3editor.sln

f. Which machine (or lab) in DigiPen that you test your application on.
  DIT2451US in tesla
  
g. The number of hours you spent on the assignment, when you started working on it and when you completed it.
  30+ hours? 
  This assignment is disgustingly huge.
  Bounding volumes could be a separate assignment, hierarchy could be another,
  even different different spheres could be an assignment.

h. Any other useful information pertaining to the application

  - Originally, when generating an OBB for a cube, the axis would point
    in the direction of the diagonals instead of the direction of the faces.
    This was surprising, but made sense.

    However, it wasn't a tight fit, so the OBB generation code compares
    the volume to that of an AABB and chooses the shape with the lesser
    volume.

  - There's a known bug where levels 1 and above of the sphere tree aren't
    generated from a top-down approach
