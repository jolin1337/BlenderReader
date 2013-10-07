#ifndef BLENDERGLOB
#define BLENDERGLOB
#include <string>
#include <iostream>

/**
 * BlenderGlobals -- contains all global variables that are nessecary for Blender classes
 */
class BlenderGlobals {
public:
	// how per-face texture coords are handled in Mesh object
	enum UVMapping {
		UV_SimpleMode,		// no vertices are created, 
							// texture coords are used from the first face 
							// using this vertex may lead to texture artifacts, 
							// especially in objects with few vertices/faces
		UV_DuplicateVertex	// if a vertex needs to have more than 
							// one texture coordinate pair it will 
							// be duplicated
	};

	/**
	 * Logs an info message to a ostream object
	 * @param text - is the text-message
	 * @param *os  - is the output target
	 */
	static void Log(std::string text, std::ostream *os=&std::cout);
	static void Log(int out, std::ostream *os=&std::cout);
	static void Log(float out, std::ostream *os=&std::cout);
	/**
	 * Deals an error message to a ostream object
	 * @param text - is the text-message
	 * @param *os  - is the output target
	 */
	static void Error(std::string text, std::ostream *os=&std::cerr);

	// debug log levels:
	static bool debug_basic;		// all default events in the different modes(DCollection)
	static bool debug_animation;	// logging all animation info
	static bool debug_fileBlocks;	// logging all loaded fileblock headers
	static bool debug_mesh;			// mesh logging
	static bool debug_uv;			// uv logging
	static bool debug_errors;		// logg the errors in the parse functions
};

#endif