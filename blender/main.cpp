// http://www.fehrprice.de/index.php?option=com_content&view=category&layout=blog&id=78&Itemid=101

#include "Blender.h"

#include <iostream>

class InitWorld : public Blender::BlenderCallback {
public:
	void objectLoaded(Blender::Object *mesh){

		std::cout << "Done loading Mesh: " << mesh->name << std::endl;
		// std::cout << "TOT vertex size: " << mesh->totvert << std::endl;
		// for (int i = 0; i < mesh->totvert; i++)
		// 	for (int j = 0; j < 3; j++)
		// 		std::cout << mesh->mverts[i].co[j] << (j!=2?",":"\n");
	}
	Blender::DCollection CollectionOfData(){
		return Blender::MESH;
	}
};

int main(int argc, char *argv[]){
	InitWorld initworld;
	Blender::BlenderFile blend(&initworld);
	if(argc == 1) {
		std::cout << "Open blendfile: ../../assets/fyrcubes.blend...\n";
		blend.parseBlenderFile("../../assets/fyrcubes.blend");	
	}
	else {
		std::cout << "Open blendfile: " << argv[1] << "...\n";
		blend.parseBlenderFile(argv[1]);	
	}
}
