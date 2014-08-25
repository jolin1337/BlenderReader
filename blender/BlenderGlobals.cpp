#include "BlenderGlobals.h"
#include <sstream>
#include <stdio.h>

bool BlenderGlobals::debug_basic = false;
bool BlenderGlobals::debug_animation = true;
bool BlenderGlobals::debug_fileBlocks = false;
bool BlenderGlobals::debug_mesh = false;
bool BlenderGlobals::debug_materials = false;
bool BlenderGlobals::debug_uv = false;
bool BlenderGlobals::debug_errors = true;

void BlenderGlobals::Log(std::string text, std::ostream *os) {
	*os << "Log: " << text.c_str() << std::endl;		// print text to console
}
void BlenderGlobals::Log(int out, std::ostream *os) {
	*os << "Log: " << out << std::endl;
}
void BlenderGlobals::Log(unsigned int out, std::ostream *os) {
	*os << "Log: " << out << std::endl;
}
void BlenderGlobals::Log(float out, std::ostream *os) {
	*os << "Log: " << out << std::endl;
}
void BlenderGlobals::Error(std::string text, std::ostream *os) {
	*os << "Error: " << text.c_str() << std::endl;		// print text to console
}