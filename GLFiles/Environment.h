#ifndef ENV_H
#define ENV_H

#include <GL/glut.h>
#include <string>

struct Environment {
	static unsigned int FrameCount;
	static GLint screen_width, screen_height;
	static bool isFullscreen;

	static std::string filename;
};

#endif