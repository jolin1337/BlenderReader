#ifndef ENV_H
#define ENV_H

#include <GL/glut.h>
#include <string>

#include <list>

struct Environment {
	struct FileObject {
		FileObject();
		FileObject(std::string file);
		FileObject(const char * file);
		std::string file;
		std::list<std::string> objects;
	};

	static unsigned int FrameCount;
	static GLint screen_width, screen_height;
	static bool isFullscreen;

	static std::string filename;
	static std::list<FileObject> files;

	static bool printArmature;

};

#endif