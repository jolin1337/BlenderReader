#include "Object.h"

std::string Blender::Object::ID = "Object";

Blender::Object::Object():name(0), location(0.0f,0.0f,0.0f), rotation(0.0f,0.0f,0.0f), scale(1.0f,1.0f,1.0f), parent(0) {}
Blender::Object::~Object() {}

void Blender::Object::setParent(Object *p) {
	if(parent != 0)
		BlenderGlobals::Log("Object already has a parent. Replacing parent now!");
	parent = p;
}
Blender::Object * Blender::Object::getParent(){
	return parent;
}

int Blender::Object::getFrameCount() {
	return adr.getFrameCount();
}

bool Blender::Object::setFrame(int frame) {
	if(frame < 1) return false;
	int cc = adr.getCurveCount();
	for (int i = 0; i < cc; i++){
		Curve cu = adr.getCurve(i);

		int totvert = cu.getTotVert();
		for (int j = 0; j < totvert; j++) {
			BezTriple bezt = cu.keyframes[j];
			if(bezt.cursor.x == frame){
				if(cu.rna_path == "location")
					location[cu.data_index] = bezt.cursor.y;
				if(cu.rna_path == "rotation_euler" || cu.rna_path == "rotation")
					rotation[cu.data_index] = bezt.cursor.y;
				if(cu.rna_path == "scale")
					scale[cu.data_index] = bezt.cursor.y;
				break;
			}
			else if(bezt.cursor.x > frame){
				if(j == 0) return false;
				BezTriple bezt2 = cu.keyframes[j-1];
				float nom = bezt.cursor.x-bezt2.cursor.x;
				if(nom == 0)
					return false;
				float k = (bezt.cursor.y-bezt2.cursor.y)/nom;
				float m = bezt.cursor.y - k * bezt.cursor.x;

				if(cu.rna_path == "location")
					location[cu.data_index] = k * frame + m;
				if(cu.rna_path == "rotation_euler" || cu.rna_path == "rotation")
					rotation[cu.data_index] = k * frame + m;
				if(cu.rna_path == "scale")
					scale[cu.data_index] = k * frame + m;
				break;
			}
			else {
			}
		}
	}
	return true;
}