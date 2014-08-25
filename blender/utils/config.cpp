// config.cpp
#include "config.h"


bool Logging::shader = false;

char *close_file(FILE *input){
	if(input != 0)
		fclose(input);
	return NULL;
}

char* file_read(const char* filename) {
	FILE* input = fopen(filename, "rb");
	if(input == NULL) return close_file(input);

	if(fseek(input, 0, SEEK_END) == -1) return close_file(input);
	long size = ftell(input);
	if(size == -1 || fseek(input, 0, SEEK_SET) == -1) return close_file(input);

	/*if using c-compiler: dont cast malloc's return value*/
	char *content = (char*) malloc( (size_t) size +1  ); 
	if(content == NULL) return close_file(input);

	fread(content, 1, (size_t)size, input);
	if(ferror(input)) {
		free(content);
		return close_file(input);
	}

	fclose(input);
	content[size] = '\0';
	return content;
}
 
/**
 * Display compilation errors from the OpenGL shader compiler
 */
void print_log(GLuint object) {
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else {
		fprintf(stderr, "printlog: Not a shader or a program\n");
		return;
	}

	char* log = (char*)malloc(log_length);

	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);

	fprintf(stderr, "%s", log);
	free(log);
}

/**
 * Compile the shader from file 'filename', with error handling
 */
GLuint create_shader(const char* filename, GLenum type) {
	const GLchar* source = file_read(filename);
	if (source == NULL) {
		fprintf(stderr, "Error opening %s: ", filename); perror("");
		return 0;
	}
	GLuint res = glCreateShader(type);
  const GLchar* sources[] = {
	    // Define GLSL version
	#ifdef GL_ES_VERSION_2_0
	    "#version 100\n"
	#else
	    "#version 120\n"
	#endif
	    ,
	    // GLES2 precision specifiers
	#ifdef GL_ES_VERSION_2_0
	    // Define default float precision for fragment shaders:
	    (type == GL_FRAGMENT_SHADER) ?
	    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
	    "precision highp float;           \n"
	    "#else                            \n"
	    "precision mediump float;         \n"
	    "#endif                           \n"
	    : ""
	    // Note: OpenGL ES automatically defines this:
	    // #define GL_ES
	#else
	    // Ignore GLES 2 precision specifiers:
	    "#define lowp   \n"
	    "#define mediump\n"
	    "#define highp  \n"
	#endif
	    ,
	    source };
	glShaderSource(res, 3, sources, NULL);
	free((void*)source);

	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		fprintf(stderr, "%s:", filename);
		print_log(res);
		glDeleteShader(res);
		return 0;
	}

	return res;
}


GLfloat m_rand(){
	return ((GLfloat)rand())/((GLfloat)RAND_MAX);
}
