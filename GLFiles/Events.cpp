#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>

#include "Environment.h"
#include "GLBlenderWorld.h"
#include <map>
#include <sstream>
#define WINDOW_TITLE_PREFIX "Blender lib test by, Johannes"

void TimerFunction(int Value);
std::ostream &operator << (std::ostream &out, Vector3 v) {
	out << v.x << ", " << v.y << ", " << v.z;
	return out;
}
#include "GLMesh.h"
#include "GLArmature.h"


typedef std::map<std::string, Blender::Object *> ModelMap;
class InitWorld : public Blender::BlenderCallback {
private:
	ModelMap nonParentedObjects;
	ModelMap glblend;
	std::list<std::list<std::string> *> objectList;
public:
	InitWorld(){}
	virtual ~InitWorld(){
		for (ModelMap::iterator it = glblend.begin(); it != glblend.end(); ++it)
			delete it->second;
	}
	void objectLoaded(Blender::Object *obj){
		if(obj == 0)
			return;
		if(!objectList.empty() && objectList.back()->size() != 0 && std::find(objectList.back()->begin(), objectList.back()->end(), obj->name) == objectList.back()->end())
			return;
		Blender::Object *m = 0;
		if(obj->getType() == Blender::Mesh::ID){
			Blender::Mesh *mesh = static_cast<Blender::Mesh *>(obj);
			m = new GLMesh(*mesh);
			// delete m;
			// return;
		}
		else if(obj->getType() == Blender::Armature::ID) {
			// std::cout << "Armatures not supported\n";
			Blender::Armature *armature = static_cast<Blender::Armature *>(obj);
			m = new GLArmature(*armature);
		}
		if(m == 0) return;
		try {
			if(m->getParent()){
				std::string name = m->getParent()->name;
				ModelMap::iterator ex = glblend.find(name);
				if(ex != glblend.end())
					m->setParent(ex->second);
				else 
					nonParentedObjects[m->name] = m;
				
			// std::cout << m->getParent()->location << "\n";
			}
			for (ModelMap::iterator it = nonParentedObjects.begin(); it != nonParentedObjects.end(); ++it)
				if(it->second->getParent()->name == m->name) {
					it->second->setParent(m);
					nonParentedObjects.erase(it);
					it = nonParentedObjects.begin();
				}
		}
		catch(...) {
			std::cerr <<"Not able to se the parent of object with name " << m->name << " :(\n";
			// delete m;
			// return;
		}
		int len = strlen(obj->name);
		char *c_name = new char[len+1];
		for (int i = 0; i < len; ++i) 
			c_name[i] = obj->name[i];
		c_name[len] = '\0';
		std::string name(c_name);
		delete[] c_name;
		ModelMap::iterator founds = glblend.find(name);
		if(founds == glblend.end()) {
			glblend[name] = m;
			std::cout << name << " <- added\n";
		}
		else {
			std::stringstream ss;
			char str[4] = "000";
			int times = 1;
			do {
				ss << times << " ";
				ss >> (str+(times>99?0:(times>9?1:2)));
				times++;
			} while(glblend.find(name+'.' + str) != glblend.end());
			glblend[name + '.' + str] = m;
			std::cout << name + '.' + str << " <- modadded\n";
		}
		// GLBlender glb = GLBlender();
		// glb.loadMesh(mesh);
		// glblend.push_back(glb);
	}
	void correctParents() {
		while(!nonParentedObjects.empty()) {
			nonParentedObjects.begin()->second->setParent(0);
			nonParentedObjects.erase(nonParentedObjects.begin());
		}
	}
	void sceneLoaded(Blender::Scene *scene){
	}
	Blender::DCollection CollectionOfData(){
		return Blender::MESH;
	}
	void addToObjectList(std::list<std::string> &objs) {
		objectList = std::list<std::list<std::string> *>();
		objectList.push_back(&objs);
	}
	size_t getObjectCount(){
		return glblend.size();
	}
	Blender::Object *getObject(size_t i){
		if(glblend.size() > 0 && i < glblend.size()) {
			int c = 0;
			for (ModelMap::iterator it = begin(); it != end(); ++it, c++) 
				if(i==c)
					return it->second;
		}
		return 0;
	}
	ModelMap::iterator begin() {
		return glblend.begin();
	}
	ModelMap::const_iterator begin() const{
		return glblend.begin();
	}
	ModelMap::iterator end() {
		return glblend.end();
	}
	ModelMap::const_iterator end() const{
		return glblend.end();
	}
} initworld;
float rx = -20, ry = 0, rz = 0, s = 1;

float speed = 1.4f;
char keys[256];
// GLobject object;
int init_resources(){
	glutTimerFunc(0, TimerFunction, 0);
	int s = 1;
	if(Environment::files.empty()) {
		if(Environment::filename.empty())
			Environment::files.push_back("../assets/threecubes.blend");
		else
			Environment::files.push_back(Environment::filename);
	}
	for (std::list<Environment::FileObject>::iterator x = Environment::files.begin(); x != Environment::files.end(); x++) {
		Environment::filename = x->file;
		std::cout << "Log::Opening file " << Environment::filename << "...\n";
		try {
			initworld.addToObjectList(x->objects);
			Blender::BlenderFile blend(&initworld);
			s &= blend.parseBlenderFile(Environment::filename);
			// if(!s)
			// 	return s;
			initworld.correctParents();
		} catch(int i){
			std::string type = Blender::getTypeError(i);
			std::cout << "Error: " << type << "\n";
		} catch(std::exception &e) {
			
			std::cout << "std Error: " << e.what() << "\n";
		}
		
	}
	return s;

	// object.loadObj("../assets/wall.obj");

	// object.setupBuffers();

	// // uniform that will be used in the shader 
	// Uniform u("m_transform");   
	// Attribute a1("position"),   // position that will represent all the vercies
	// 		  // a2("texcoord"), // textures uppcomming TODOS
	// 		  a3("color"),      // color of the object that was loaded
	// 		  a4("normal");     // normal attribute

	// // Add all the uniforms and attributes for this key-object 
	// object.addUniform(u);
	// object.addAttribute(a1);
	// object.addAttribute(a3);
	// object.addAttribute(a4);
	
	// // load vertex-shader and fragment-shader to a program and bind to our properties
	// int status = object.loadShaders("GLFiles/shaders/wall.v-shader", "GLFiles/shaders/wall.f-shader");
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
	/*Matrix4 view = Matrix4().lookAt( Vector3(0,    5.0f,  0), 			// eye position
									 Vector3(0,    0.0f,   0), 			// focus point
									 Vector3(0.0f, 0.0f, 1.0f) );									// up direction
	Matrix4 projection = Matrix4().Perspective(45.0f, 										// fov
											   (float)Environment::screen_width / (float)Environment::screen_height, // aspect ratio
												1.0f, 																 // z-near
												500.0f );															 //z-far
	int j = 0, i = 0;
	Matrix4 model = Matrix4().Scale(Vector3(0.5,0.5,0.5));
	Matrix4 m_transform = projection * view * model;

	glUseProgram(object.getProgram());						// use program of the wall
	glUniformMatrix4fv(object.getUniform("m_transform"), 1, GL_FALSE, m_transform.getMatrix4());
	glUseProgram(0);
	return;*/
	static int oldTimeSinceStart = 0;
	static float frame = 0;
	static int max_frame = 0;
	if(frame == 0) {
		for (ModelMap::iterator it = initworld.begin(); it != initworld.end(); ++it) {
			int fr = it->second->getFrameCount();
			if(fr > max_frame)
				max_frame = fr;
		}
		frame = 1;
	}
	if(keys['w'] || keys['W'])
		rx -= speed;
	if(keys['a'] || keys['A'])
		rz += speed;
	if(keys['d'] || keys['D'])
		rz -= speed;
	if(keys['s'] || keys['S'])
		rx += speed;
	if(keys['+'])
		s /= 0.99;
	if(keys['-'])
		s /= 1.01;

	Matrix4 view = Matrix4().lookAt( Vector3(0,    20.0f,  0), 			// eye position
									 Vector3(0,    0.0f,   0), 			// focus point
									 Vector3(0.0f, 0.0f, 1.0f) );									// up direction
	view.RotateA(1.0f, Vector3(rx, ry, rz));
	view.Scale(Vector3(1,1,1) * s);
	Matrix4 projection = Matrix4().Perspective(45.0f, 										// fov
											   (float)Environment::screen_width / (float)Environment::screen_height, // aspect ratio
												1.0f, 																 // z-near
												500.0f );															 //z-far

	for (ModelMap::iterator it = initworld.begin(); it != initworld.end(); ++it)  {
		it->second->setFrame(frame);
		if(it->second->getType() != Blender::Mesh::ID && it->second->getType() != Blender::Armature::ID && Environment::printArmature) continue;
		Blender::Object *parent = it->second->getParent();
		Matrix4 model = Matrix4();//.Translate(Vector3::mult(&gbl->location, 1)).Scale(gbl->scale).Rotate(1.0f, gbl->rotation);
		while(parent) {
			// std::cout << parent->name << "\n";
			model.Translate(parent->location).Rotate(1.0f, parent->rotation).Scale(parent->scale);
			// if(parent->getType() == Blender::Armature::ID) {
			// 	Blender::Armature *arm = (Blender::Armature*)parent;
			// 	for (Blender::Armature::PoseIterator it = arm->begin(); it != arm->end(); ++it) {
			// 		std::cout << it->name << "\n";
			// 	}
			// }
			parent = parent->getParent();
		}
		
		if(it->second->getType() == Blender::Mesh::ID) {
			GLMesh *gbl = (GLMesh*)it->second;
			Matrix4 m_transform = projection * view * model;
			glUseProgram(gbl->getProgram());						// use program of the object
			glUniformMatrix4fv(gbl->getUniform("m_transform"), 1, GL_FALSE, m_transform.getMatrix4());
		}
		else if(it->second->getType() == Blender::Armature::ID) {
			GLArmature *gbl = (GLArmature*)it->second;
			Matrix4 m_transform = projection * view * model;
			glUseProgram(gbl->getProgram());						// use program of the object
			glUniformMatrix4fv(gbl->getUniform("m_transform"), 1, GL_FALSE, m_transform.getMatrix4());
		}
	}

	if(frame > max_frame)
		frame = 1;

	glUseProgram(0);
	Matrix4 m_transform = projection * view;
	glLoadMatrixf(m_transform.getMatrix4());
	// rz+=0.3f;

	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	frame += 0.05f * deltaTime;
	// ry+=0.3f;
	// rx+=0.5f;
	if(Environment::FrameCount < 70)
		glutPostRedisplay();
}

void onDisplay() {
	if(Environment::FrameCount > 70) return;
	++Environment::FrameCount;
	// return;
	glClearColor(1.0, 1.0, 1.0, 1.0);
	// Clear the background as black 
	// glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// glBegin(GL_POLYGON);
	// 	glVertex3d(-0.5, 0.5, 0.0);
	// 	glVertex3d(-0.5, -0.5, 0.0);
	// 	glVertex3d(0.5, -0.5, 0.0);
	// 	glVertex3d(0.5, 0.5, 0.0);
	// glEnd(); 
	for (ModelMap::iterator it = initworld.begin(); it != initworld.end(); ++it) {
		if(it->second->getType() == Blender::Mesh::ID) {
			GLMesh *gbl = (GLMesh*)it->second;
			glUseProgram(gbl->getProgram());
			gbl->render();
		}
		else if(it->second->getType() == Blender::Armature::ID && Environment::printArmature) {
			GLArmature *gbl = (GLArmature*)it->second;
			glUseProgram(gbl->getProgram());
			gbl->render();
		}
	}

	glUseProgram(0);

	// glUseProgram(object.getProgram());
	// object.render();
	// glUseProgram(0);
	// Light:
	// GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	// glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Display the result 
	glutSwapBuffers();
}

void onMouseMove(int x, int y){
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
		case 'e':
		case 'E':
			speed += 0.1f;
			break;
		case 'q':
		case 'Q':
			speed -= 0.1f;
			break;
		// case 'w':
		// case 'W':
		// 	rx += speed;
		// 	break;
		// case 'a':
		// case 'A':
		// 	rz += speed;
		// 	break;
		// case 'd':
		// case 'D':
		// 	rz -= speed;
		// 	break;
		// case 's':
		// case 'S':
		// 	rz -= speed;
		// 	break;
		default:
			break;
	}
	
	keys[Key] = true;
}
void onKeyUp(unsigned char Key, int x, int y){
	keys[Key] = false;
}
void onKeyDownSpecial(int Key, int x, int y){
}
void onKeyUpSpecial(int Key, int x, int y){
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