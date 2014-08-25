#ifndef SCENE
#define SCENE
#include "DNA.h"
#include "Object.h"
#include "Mesh.h"
#include "Armature.h"
#include <sstream>
#include <string.h>
#include <algorithm>

namespace Blender{
	class Scene {
		std::string name;
		std::vector<Object *> objects;
		std::vector<Material > materials;
	public:
		Scene(){}
		Scene(const Scene &scene): name(scene.name), materials(scene.materials){
			for (unsigned int i = 0; i < scene.objects.size(); i++) {
				objects.push_back(scene.objects[i]->clone());
			}
		}
		Scene &operator=(const Scene &rhs) {
			Scene tmp(rhs);
			std::swap(tmp.name, name);
			std::swap(tmp.objects, objects);
			std::swap(tmp.materials, materials);
			// for (unsigned int i = 0; i < objects.size(); i++)
			// 	delete objects[i];
			// for (unsigned int i = 0; i < rhs.objects.size(); i++)
			// 	objects.push_back(rhs.objects[i]->clone());
			return *this;
		}
		virtual ~Scene() {
			for (unsigned int i = 0; i < objects.size(); i++) 
				if(objects[i]) 
					delete objects[i];
		}
		const char * getName(){
			return std::string(name).c_str();
		}

		unsigned int getObjectCount(){
			return objects.size();
		}
		Object *getObject(unsigned int index){
			if(index < getObjectCount())
				return objects[index];
			return 0;
		}
		void parse(StructureDNA *sdna, FileBlockHeader *fileblockheader, std::vector<FileBlockHeader> &blocks){
			for (unsigned int i = 0; i < objects.size(); i++) {
				delete objects[i];
			}
			name = "";
			materials.clear();

			if(fileblockheader == 0) return;
			name = fileblockheader->getString("id.name", sdna);
			if(BlenderGlobals::debug_mesh) {
				BlenderGlobals::Log("Parse Scene");						// scene name logging
				BlenderGlobals::Log("Scene name = " + name);			// scene name logging
			}

			std::vector<std::pair<int, Object *> > addressToObjects;
			std::vector<bool > hasParents;
			std::map<int, Object *> objectsloaded;
			std::map<Object *, int> parentedObjects;
			FileBlockHeader *baseBlockStart = sdna->getFileBlock("base.*first", fileblockheader);
			if(baseBlockStart != 0) {
				parseMaterials(sdna);
				while(baseBlockStart != 0) {
					FileBlockHeader *fbobject = sdna->getFileBlock("*object", baseBlockStart);
					FileBlockHeader *data = sdna->getFileBlock("*data", fbobject);
					if(data != 0){
						Object *object = 0;
						BlenderGlobals::Log(data->getString("id.name", sdna));
						if(strncmp(data->getStructureName(sdna), "Mesh", 4) == 0) {
							if(BlenderGlobals::debug_mesh) 
								BlenderGlobals::Log("  -> Loading mesh");
							object = new Mesh(BlenderGlobals::UV_SimpleMode);
							((Mesh*)object)->parse(sdna, fbobject, &materials);
						}
						else if(strncmp(data->getStructureName(sdna), "bArmature", 4) == 0) {
							if(BlenderGlobals::debug_mesh) 
								BlenderGlobals::Log("  -> Loading Armature");
							object = new Armature();
							((Armature*)object)->parse(sdna, fbobject);
						}
						else if(BlenderGlobals::debug_basic){
							std::stringstream ss;
							ss << "  -> Unknown object type: " << data->getString("id.name", sdna) << "(" << data->getStructureName(sdna) << ")" << ", object not loaded";
							BlenderGlobals::Log(ss.str());
						}
						if(object) {
							object->setParent(0);
							int addrParent = fbobject->getInt("*parent", sdna);
							if(addrParent != 0) 
								parentedObjects[object] = addrParent;// fbobject->oldMemoryAddress;
							objectsloaded[fbobject->oldMemoryAddress] = object;
							objects.push_back(object);
						}
					}
					 
					
					baseBlockStart = sdna->getFileBlock("*next", baseBlockStart);
				}

			}
			else if(BlenderGlobals::debug_basic)
				BlenderGlobals::Log("  -> No Base struct found");
			
			for (std::map<Object *, int>::iterator it = parentedObjects.begin(); it != parentedObjects.end(); ++it)
				it->first->setParent(objectsloaded[it->second]);
			
			// for (std::vector<Object *>::iterator it = objects.begin(); it != objects.end(); ++it)
			// {
			// 	Object * par = (*it)->getParent();
			// 	if(par) {
			// 		BlenderGlobals::Log((*it)->name);
			// 		BlenderGlobals::Log(par->name);
			// 		BlenderGlobals::Log("\n");
			// 	}
			// }
		}
	private:
		void parseMaterials(StructureDNA *sdna) {
			std::vector<FileBlockHeader *> fbh = sdna->getFileBlocksByName("Material");
			Material mat;
			mat.name = "DefaultMat";
			mat.r = 0.5f;
			mat.g = 0.5f;
			mat.b = 0.5f;
			mat.ref = 1.0f;
			materials.push_back(mat);
			for (unsigned int i = 0; i < fbh.size(); i++) {
				unsigned int len = sdna->getLength(fbh[i]->spStruct->typeIndex);
				int offset_r = sdna->getMemberOffset("r", *fbh[i]),
					offset_g = sdna->getMemberOffset("g", *fbh[i]),
					offset_b = sdna->getMemberOffset("b", *fbh[i]),
					// offset_ref = sdna->getMemberOffset("ref", *fbh[i]);
					offset_ref = sdna->getMemberOffset("alpha", *fbh[i]);
					
				Material mat;
				mat.r = fbh[i]->getFloat(offset_r, 0, len);
				mat.g = fbh[i]->getFloat(offset_g, 0, len);
				mat.b = fbh[i]->getFloat(offset_b, 0, len);
				mat.ref = fbh[i]->getFloat(offset_ref, 0, len);
				mat.name = fbh[i]->getString("id.name", sdna);
				// std::stringstream color("Red: ");
				// color << mat.r << ", Green: " << mat.g << ", Blue: " << mat.b << ", ref: " << mat.ref << "\n";
				// BlenderGlobals::Log(color.str());
				if(BlenderGlobals::debug_materials)
					BlenderGlobals::Log((std::string)"Scene load material name: " + mat.name);

				materials.push_back(mat);
			}
		}
	};
}
#endif
