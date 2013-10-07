#ifndef SCENE
#define SCENE
#include "DNA.h"
#include "Object.h"
#include "Mesh.h"
#include "Armature.h"
#include <sstream>
#include <string.h>

namespace Blender{
	class Scene {
		std::string name;
		std::vector<Object *> objects;
		std::vector<Material > materials;
	public:
		Scene(){}
		Scene(const Scene &scene): name(name), materials(materials){
			for (unsigned int i = 0; i < scene.objects.size(); i++) {
				objects.push_back(scene.objects[i]->clone());
			}
		}
		~Scene() {
			for (unsigned int i = 0; i < objects.size(); i++) {
				delete objects[i];
			}
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
			if(fileblockheader == 0) return;
			name = fileblockheader->getString("id.name", sdna);
			if(BlenderGlobals::debug_mesh) {
				BlenderGlobals::Log("Parse Scene");			// scene name logging
				BlenderGlobals::Log("Scene name = " + name);			// scene name logging
			}
			// work on object ipo
			// fbobject = sdna->getFileBlock("*data", fbobject);
			// int address = fileblockheader->getInt("*ipo", sdna);
			// if(address == 0)
			// 	BlenderGlobals::Log("No path:s on this object");
			// else{
			// 	BlenderGlobals::Log(address);
			// 	FileBlockHeader *ipofbh = sdna->getFileBlock("*ipo", fileblockheader);
			// 	if(address == 0)
			// 		return;
			// 	BlenderGlobals::Log(ipofbh->getString("id.name", sdna));
			// }
			// printFields(fileblockheader, sdna);
			std::map<int, Object *> addressToObjects;
			FileBlockHeader *baseBlockStart = sdna->getFileBlock("base.*first", fileblockheader);
			if(baseBlockStart != 0) {
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
						offset_ref = sdna->getMemberOffset("ref", *fbh[i]);
						
					Material mat;
					mat.name = fbh[i]->getString("id.name", sdna);
					mat.r = fbh[i]->getFloat(offset_r, 0, len);
					mat.g = fbh[i]->getFloat(offset_g, 0, len);
					mat.b = fbh[i]->getFloat(offset_b, 0, len);
					mat.ref = fbh[i]->getFloat(offset_ref, 0, len);
					BlenderGlobals::Log((std::string)"Scene load material name: " + mat.name);
					// std::stringstream ss;
					// ss << "R " << mat.r << ", G " << mat.g << ", B " << mat.b << ", A " << mat.ref << ";\n";
					// BlenderGlobals::Log(ss.str());
					materials.push_back(mat);
				}

				while(baseBlockStart != 0) {
					FileBlockHeader *fbobject = sdna->getFileBlock("*object", baseBlockStart);
					FileBlockHeader *data = sdna->getFileBlock("*data", fbobject);
					if(data != 0){
						if(strncmp(data->getStructureName(sdna), "Mesh", 4) == 0) {
							if(BlenderGlobals::debug_mesh) 
								BlenderGlobals::Log("  -> Loading mesh");
							Mesh *object = new Mesh(BlenderGlobals::UV_SimpleMode);
							object->parse(sdna, fbobject, &materials);


							int addrParent = fbobject->getInt("*parent", sdna);
							if(addrParent != 0){
								if(addressToObjects.find(addrParent) != addressToObjects.end()){
									object->parent = addressToObjects[addrParent];
									std::cout << object << " <- parented object\n";
								}
								else
									addressToObjects[addrParent] = object;
							}
							addrParent = fbobject->oldMemoryAddress;
							if(addrParent != 0) {
								if(addressToObjects.find(addrParent) != addressToObjects.end()){
									addressToObjects[addrParent]->parent = object;
									std::cout << object << " <- parented object\n";
								}
							}

							objects.push_back(object);

							// FileBlockHeader *bDeformGroup = sdna->getFileBlock("defbase.*first", fbobject);

							// if(bDeformGroup != 0){
							// 	while(bDeformGroup) {
							// 		// printFields(bDeformGroup, sdna);
							// 		// BlenderGlobals::Log((std::string)("DeformGroup: ") + bDeformGroup->getString("name", sdna));
							// 		bDeformGroup = sdna->getFileBlock("*next", bDeformGroup);
							// 	}
							// }
						}
						else if(strncmp(data->getStructureName(sdna), "bArmature", 4) == 0) {
							if(BlenderGlobals::debug_mesh) 
								BlenderGlobals::Log("  -> Loading Armature");

							Armature *object = new Armature();
							object->parse(sdna, fbobject);


							int addrParent = fbobject->getInt("*parent", sdna);
							if(addrParent != 0){
								if(addressToObjects.find(addrParent) != addressToObjects.end())
									object->parent = addressToObjects[addrParent];
								else
									addressToObjects[addrParent] = object;
							}
							addrParent = fbobject->oldMemoryAddress;
							if(addrParent != 0) {
								if(addressToObjects.find(addrParent) != addressToObjects.end()){
									addressToObjects[addrParent]->parent = object;
									std::cout << object << " <- parented object\n";
								}
							}

							objects.push_back(object);
						}
						else if(BlenderGlobals::debug_basic){
							std::stringstream ss;
							ss << "  -> Unknown object type: " << data->getString("id.name", sdna) << "(" << data->getStructureName(sdna) << ")" << ", object not loaded";
							BlenderGlobals::Log(ss.str());
						}
					}
					 
					
					baseBlockStart = sdna->getFileBlock("*next", baseBlockStart);
				}

			}
			else if(BlenderGlobals::debug_basic)
				BlenderGlobals::Log("  -> No Base struct found");
		}
	};
}
#endif
