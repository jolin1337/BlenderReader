#ifndef CONFIG_H
#define CONFIG_H

/* Using the standard output for fprintf */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/glut.h>

struct Logging {
	static bool shader;
};

char *close_file(FILE *input);

char* file_read(const char* filename);
 
/**
 * Display compilation errors from the OpenGL shader compiler
 */
void print_log(GLuint object);

/**
 * Compile the shader from file 'filename', with error handling
 */
GLuint create_shader(const char* filename, GLenum type);


GLfloat m_rand();

#endif
