#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>

#include "Environment.h"
#include "GLBlenderWorld.h"
#define WINDOW_TITLE_PREFIX "Blender lib test by, Johannes"

void TimerFunction(int Value);

InitWorld initworld;
int init_resources(){
	Blender::BlenderFile blend(&initworld);
	glutTimerFunc(0, TimerFunction, 0);
	if(Environment::filename.empty())
		return blend.parseBlenderFile("../assets/fyrcubes.blend");
	else
		return blend.parseBlenderFile(Environment::filename);

	// glblend.loadObj("../assets/wall.obj");

	// glblend.setupBuffers();

	// // uniform that will be used in the shader 
	// Uniform u("m_transform");   
	// Attribute a1("position"),   // position that will represent all the vercies
	// 		  // a2("texcoord"), // textures uppcomming TODOS
	// 		  a3("color"),      // color of the object that was loaded
	// 		  a4("normal");     // normal attribute

	// // Add all the uniforms and attributes for this key-object 
	// glblend.addUniform(u);
	// glblend.addAttribute(a1);
	// glblend.addAttribute(a3);
	// glblend.addAttribute(a4);
	
	// // load vertex-shader and fragment-shader to a program and bind to our properties
	// int status = glblend.loadShaders("GLFiles/shaders/wall.v-shader", "GLFiles/shaders/wall.f-shader");
	// if(status != 1) // if loadshaders failed to load
	// 	return status; // abort
	// return 1;
}

void onReshape(int width, int height) {
	Environment::screen_width = width;
	Environment::screen_height = height;
	glViewport(0, 0, Environment::screen_width, Environment::screen_height);
	// glutWarpPointer(Environment::screen_width/2, Environment::screen_height/2);
}

void idle(){
	static float frame = 0;
	static int max_frame = 0;
	static float rx = -20, ry = 0, rz = 0;
	if(frame == 0) {
		for (int i = 0; i < initworld.getObjectCount(); i++) {
			int fr = initworld.getMesh(i)->getFrameCount();
			if(fr > max_frame)
				max_frame = fr;
		}
		frame = 1;
	}
	Matrix4 view = Matrix4().lookAt( Vector3(0,    5.0f,  0), 			// eye position
									 Vector3(0,    0.0f,   0), 			// focus point
									 Vector3(0.0f, 0.0f, 1.0f) );									// up direction
	view.RotateA(1.0f, Vector3(rx, ry, rz));
	Matrix4 projection = Matrix4().Perspective(45.0f, 										// fov
											   (float)Environment::screen_width / (float)Environment::screen_height, // aspect ratio
												1.0f, 																 // z-near
												500.0f );															 //z-far
	int j = 0, i = 0;
	Matrix4 model = Matrix4().Scale(Vector3(0.5,0.5,0.5));
	Matrix4 m_transform = projection * view * model;

	for (int i = 0; i < initworld.getObjectCount(); i++) {
		GLBlender *gbl = initworld.getMesh(i);
		gbl->setFrame(frame);
		glUseProgram(gbl->getProgram());						// use program of the wall
		glUniformMatrix4fv(gbl->getUniform("m_transform"), 1, GL_FALSE, m_transform.getMatrix4());
		if(frame > max_frame)
			frame = 1;
	}
	glUseProgram(0);
	rz+=0.3f;
	frame += 0.6f;
	// ry+=0.3f;
	// rx+=0.5f;
	glutPostRedisplay();
}

void onDisplay() {
	++Environment::FrameCount;
	glClearColor(1.0, 1.0, 1.0, 1.0);
	// Clear the background as black 
	// glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < initworld.getObjectCount(); i++) {
		GLBlender *gbl = initworld.getMesh(i);
		glUseProgram(gbl->getProgram());
		gbl->render();
	}
	glUseProgram(0);

	// glBegin(GL_LINE_STRIP);
	// 	glColor3f(1.0f, 1.0f, 1.0f);
	// 	glVertex2f( 0.0f,  0.0f);
	// 	glVertex2f( 0.5f,  0.0f);
	// 	glVertex2f( 0.5f,  0.5f);
	// 	glVertex2f( 0.0f,  0.5f);
	// 	glVertex2f( 0.0f,  0.0f);
	// glEnd();
	// Light:
	// GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	// glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Display the result 
	glutSwapBuffers();
}

void onMouseMove(int x, int y){
	float speed = 0.02;
}

void onKeyDown(unsigned char Key, int x, int y){
	switch(Key){
		case 27:
			exit(0);
			break;
		case 'f':
			if(Environment::isFullscreen && Environment::screen_height > 0 && Environment::screen_width > 0){
				Environment::isFullscreen = false;
				glutPositionWindow(0,0);
			}
			else{
				glutFullScreen();
				Environment::isFullscreen = true;
			}
			break;
		default:
			break;
	}
	
}
void onKeyUp(unsigned char Key, int x, int y){
}
void onKeyDownSpecial(int Key, int x, int y){
	// keys[Key] = true;
}
void onKeyUpSpecial(int Key, int x, int y){
	// keys[Key] = false;
}

void TimerFunction(int Value) {
    if (0 != Value) {
        char* TempString = (char*)
            malloc(512 + strlen(WINDOW_TITLE_PREFIX));
 
        sprintf(
            TempString,
            "%s: %d fps @ %d x %d",
            WINDOW_TITLE_PREFIX,
            Environment::FrameCount * 4,
            Environment::screen_width, 
            Environment::screen_height
        );
 
        glutSetWindowTitle(TempString);
        free(TempString);
    }
     
    Environment::FrameCount = 0;
    glutTimerFunc(250, TimerFunction, 1);
}