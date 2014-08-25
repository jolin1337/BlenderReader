#include "Environment.h"

int Environment::screen_width = 600;
int Environment::screen_height = 600;
bool Environment::isFullscreen = true;
unsigned int Environment::FrameCount = 0;

std::string Environment::filename = "";
std::list<Environment::FileObject> Environment::files(0);


bool Environment::printArmature = true;

Environment::FileObject::FileObject(std::string file): file(file) {}
Environment::FileObject::FileObject(const char *file): file(file) {}
Environment::FileObject::FileObject(): file(file) {}