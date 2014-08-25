// #include "DXUT.h"
// #include "Vertex.h"
// #include "ShadedPath.h"
// #include "Terrain.h"
// #include "Camera.h"
// #include "d3dUtil.h"
#include "Blender.h"
#include <algorithm>
#include <list>
#include <sstream>
#include <string.h>
using namespace std;

void Blender::BlenderFile::parseFileBlocks(StructureDNA *sdna, vector<FileBlockHeader> &blocks, BlenderGlobals::UVMapping uvMapping) {
	for (unsigned int blockNum = 0; blockNum < blocks.size(); blockNum++) {
		FileBlockHeader *fbh = &blocks[blockNum];
		string name = fbh->getStructureName(sdna);
		
		fbh->spStruct = (sdna->getStructure(fbh->sdnaIndex));
		sdna->addFileBlock(fbh);
	}
	for (unsigned int blockNum = 0; blockNum < blocks.size(); blockNum++) {
		FileBlockHeader *fbh = &blocks[blockNum];
		const char *structureName = fbh->getStructureName(sdna);
		if (BlenderGlobals::debug_fileBlocks) {
			std::ostringstream oss;
			oss << " <-- Loaded " << fbh->code << " " << structureName << ", size == " << fbh->size << " #C == " << fbh->count << " -->";
			BlenderGlobals::Log(oss.str());
		}
		/*if (strcmp("Mesh", structureName) == 0) {
			Mesh mesh(uvMapping);
			mesh.parse(sdna, fbh, 0);
			blenderCallback->objectLoaded(&mesh);
		} */
		if (strcmp("Scene", structureName) == 0) {
			scene.parse(sdna, fbh, blocks);
			int countObjects = scene.getObjectCount();
			for (int i = 0; i < countObjects; i++) {
				blenderCallback->objectLoaded(scene.getObject(i));
			}
			blenderCallback->sceneLoaded(&scene);
		}
	}
}

bool Blender::BlenderFile::readBlenderHeader(ifstream &blenderFile) {
	// header processing:
	char header[12];
	blenderFile.read(header, 12);
	if (strncmp("BLENDER", &header[0], 7) == 0) {
		if (BlenderGlobals::debug_basic) BlenderGlobals::Log("Blender id found");
	} else {
		// check for gzip file:
		if (header[0] == '\x1f' && header[1] == '\x8b') {
			BlenderGlobals::Error("Compression .blend file not supported and cannot be read\n");
		} else {
			BlenderGlobals::Error("This is no .blend file!\n");
		}
		return false;
	}
	if (header[7] == '_')
		blenderDesc.pointersize = 4;
	else if (header[7] == '-') {
		BlenderGlobals::Log("Pointer size 8 bytes not supported\n");
		return false;
	} else {
		BlenderGlobals::Log("Unknown pointer size, verify the .blend file.\n");
		return false;
	}
	if (header[8] == 'v')
		blenderDesc.littleEndian = true;
	else if (header[8] == 'V') {
		blenderDesc.littleEndian = false;
		BlenderGlobals::Log("Big endian not supported\n");
		return false;
	} else {
		BlenderGlobals::Log("Unknown endianness\n");
		return false;
	}
	blenderDesc.version[0] = header[9];
	blenderDesc.version[1] = header[10];
	blenderDesc.version[2] = header[11];
	blenderDesc.version[3] = 0;
	if(BlenderGlobals::debug_basic){
		stringstream ss;
		ss << " version: " << blenderDesc.version << "\n";
		BlenderGlobals::Log(ss.str().c_str());
	}
	return true;
}
bool Blender::BlenderFile::parseBlenderFile(string filename, BlenderGlobals::UVMapping uvMapping) {
	vector<FileBlockHeader> fileBlocks;
	ifstream blenderFile(filename.c_str(), ios::in | ios::binary);
	if (!blenderFile) {
		BlenderGlobals::Log("File not found: " + filename + "\n");
		return false;
	}
	if (BlenderGlobals::debug_basic) BlenderGlobals::Log("File opened: " + filename + "\n");

	// basic assumptions about data types:
	if (sizeof(char) != 1 || sizeof(unsigned int) != 4 || sizeof(float) != 4) {
		BlenderGlobals::Error("Invalid compiler data-type lengths detected. compile with different options!\n");
		return false;
	}

	if(!readBlenderHeader(blenderFile))
		return false;

	StructureDNA sdna;
	FileBlockHeader fbheader;
	fbheader.buffer = 0;
	bool mesh_found = false;			// true for first Mesh found
	bool in_data_gathering = false;		// true while gathering DATA block for Mesh
	bool done = false;					// done if first mesh and data was gathered
	do {
		// we are only interested in (first) Mesh block and subsequent DATA blocks
		fbheader.readFileBlock(&blenderFile);
		// if(strcmp(fbheader.code, "DATA") != 0)
		// 	printf("%s\n", fbheader.code);
		//Log(std::string("Header code: ") + fbheader.code + "\n");
		if (strcmp(fbheader.code, "DNA1") == 0) {
			bool ok = sdna.parse(&fbheader);
			if (!ok) return false;
			fbheader.release();
		} else {
			if ( !done && strcmp(fbheader.code, "ME") == 0) {
				mesh_found = true;
			}
			// store mesh file block and its data file blocks
			if (mesh_found) {
				fileBlocks.push_back(fbheader);
				mesh_found = false;
				in_data_gathering = true;
			} else if (in_data_gathering && strcmp(fbheader.code, "DATA") == 0) {
				fileBlocks.push_back(fbheader);
			} else {
				if (in_data_gathering) {
					// we already gathered mesh and data blocks
					done = true;
					in_data_gathering = false;
				}
				fileBlocks.push_back(fbheader);
				// fbheader.release();
			}
		}
	} while (strcmp(fbheader.code, "ENDB") != 0);

	blenderFile.close();
	if (BlenderGlobals::debug_basic) BlenderGlobals::Log("File closed\n");

	parseFileBlocks(&sdna, fileBlocks, uvMapping);

	for (unsigned int k = 0; k < fileBlocks.size(); k++) {
		fileBlocks[k].release();
	}
	return true;
}
