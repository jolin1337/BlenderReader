#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>

#include "GLFiles/Environment.h"


/**
 * init_resources, inits the needed resources for the game ie start the game and let the game handle the resources
 * @return 0 on fail and 1 on success
 */
int init_resources();

/**
 * onReshape is called when the window reshapes by user
 * @param width  represents the new width of the window
 * @param height represents the new height of the window
 */
void onReshape(int width, int height);
/**
 * updates the game on every frame
 */
void idle();
/**
 * draw screen image of the game when needed
 */
void onDisplay();

/**
 * onMouseMove is called when the mouse is moving
 * @param x represents the x coordinate of the mouse
 * @param y represents the y coordinate of the mouse
 */
void onMouseMove(int x, int y);

/**
 * onKeyDown is called when a characterkey is pressed
 * @param Key the pressed key
 * @param x   current mouse x-coordinate
 * @param y   current mouse y-coordinate
 */
void onKeyDown(unsigned char Key, int x, int y);
/**
 * onKeyUp is called when a characterkey is released
 * @param Key the key released
 * @param x   current mouse x-coordinate
 * @param y   current mouse y-coordinate
 */
void onKeyUp(unsigned char Key, int x, int y);

void onKeyDownSpecial(int Key, int x, int y);
void onKeyUpSpecial(int Key, int x, int y);
int start_openGL(int argc, char *argv[]);


int main(int argc, char *argv[]) {
	return start_openGL(argc, argv);
}

int start_openGL(int argc, char *argv[]) {
	glutInit(&argc, argv);// glut initialise something so that rendering will be enababled
	glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);// tell glut how to render output: rgba colorsm alpha channel, double buffers for smooth rendering, stores the depth in the render of polygons etc..
	glutInitWindowSize(Environment::screen_width, Environment::screen_height); // sets the default window size
	glutCreateWindow("Blender lib test by, Johannes");               // create a window with a title

	// Extension wrangler initialising 
	GLenum glew_status = glewInit();    // init the extension
	if (glew_status != GLEW_OK) {       // if not success
		std::cerr << "Error: "<< glewGetErrorString(glew_status) << std::endl;    // error msg
		return EXIT_FAILURE;
	}
	if (!GLEW_VERSION_2_0) {            // if not right version
		std::cerr << "Error: your graphic card does not support OpenGL 2.0\n";  // error msg
		return 1;
	}

	if(argc>1){
		Environment::filename = argv[1];
	}
	else
		Environment::filename = "";

	// When all init functions runs without errors,
	// the program can initialise the resources 
	if (init_resources() == 1) {    // if we were able to init the resources
        // Go ahead and setup the opengl-Environment::game properties
		if(Environment::isFullscreen)    // if fullscreen will show att start
			glutFullScreen();// set fullscreen on the window 
		//glEnable(GL_CULL_FACE);   // draw both sides of one face
		// glutSetCursor(GLUT_CURSOR_NONE);// make the cursor vanish
		glEnable(GL_BLEND);         // make opengl blend-mode enabled
		glEnable(GL_DEPTH_TEST);    // make opengl depth-test enabled
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glutReshapeFunc(onReshape); // if the window resizes onReshape will be called
		glutIdleFunc(idle);         // idle will be called every frame
		glutDisplayFunc(onDisplay); // onDisplay function will draw/update the screen vissually

		glutPassiveMotionFunc(onMouseMove);// onMouseMove is called when the mouse is moving
		glutMotionFunc(onMouseMove);       // onMouseMove is called when the mouse is moving and mouse button is down

		glutKeyboardFunc(onKeyDown);        // onKeyDown will be called when a key has ben pressed
		glutKeyboardUpFunc(onKeyUp);        // onKeyUp will be called when a key has ben released
		glutSpecialFunc(onKeyDownSpecial);  // special keys like F1,F2...F9 and arrows etc 
		glutSpecialUpFunc(onKeyUpSpecial);
		
		glutMainLoop();                     // starts the main program/the window that was created
	}
	return 0;
}
