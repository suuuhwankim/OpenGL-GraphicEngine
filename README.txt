/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: To explain whole status.
Language: C++
Platform: Window 10, GTX 2060 Super, 4.6 OpenGL Driver Version
Project: su.kim_C300_2
Author: Suhwan Kim, su.kim, Su Kim
Creation date: 11/8/2021
End Header --------------------------------------------------------*/

a. How to use parts of your user interfae that is NOT specified in the assignment description

- Press ESC to execute the program.
- it can control almost all things with ImGui.
- Press E and mouse move is the control. E likes toggle on and off.
- key Z and C is camera up and down.

b. Any assumption that you make on how to use the application that, if violated, might cause the application 
   to fail.

  - Sometimes the ImGui hides in plane, so please move the ImGui panel to another side to easy to see. 



c. Which part of the assignment has been completed?


  - sky box with environment map
  - refraction
  - reflection
  - scene setup
  - GUI

d. Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those
   parts are not completed?

   - may be camera working is little bit hard.



e. Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path
   (folder name), file name, function name(or line number)

   - Graphics :    All source files for run this program.
   - include  :     contains all the header files for openGL. (glfw)
   - shaders  :    All shader files(normal, blin shading, phong lighting, phong shading)
   - obj       :    obj files to be loaded for various models.
   - textures :    two ppm file for load the texutre.

f. Which machine(or lab) in DigiPen that you test your application on.
 
- ii ) (in remote)  Windows 10, 2060 Super, 4.6 OpenGL driver version.


g. The number of hours you spent on the assignment, when you started working on it and when you completed it.
 - 55 ~ 60 hours.

h. Any other useful information peratining to the application.
 - None