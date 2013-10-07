#include "Armature.h"

std::string Blender::Armature::ID = "Armature";



int Blender::Armature::parseAnimation(StructureDNA *sdna, FileBlockHeader *fbh) {
	return adr.parse(fbh, sdna);
}

int Blender::Armature::parseBones(StructureDNA *sdna, FileBlockHeader *fbh){
	// FileBlockHeader *base = sdna->getFileBlock("bonebase.*first", fbh);
	//printFields(base, sdna);
	return 1;
} 

Blender::Armature &Blender::Armature::parse(StructureDNA *sdna, FileBlockHeader *fileblockheader){
	if(fileblockheader == 0 || sdna == 0) return *this;
	if (BlenderGlobals::debug_basic) BlenderGlobals::Log("Parse Armature data");

	unsigned int len = sizeof(float);
	// Load location
	int offsetLoc = sdna->getMemberOffset("loc", *fileblockheader);
	location.x = fileblockheader->getFloat(offsetLoc, 0, len);
	location.y = fileblockheader->getFloat(offsetLoc, 1, len);
	location.z = fileblockheader->getFloat(offsetLoc, 2, len);
	// Load ritation
	int offsetRot = sdna->getMemberOffset("rot", *fileblockheader);
	rotation.x = fileblockheader->getFloat(offsetRot, 0, len);
	rotation.y = fileblockheader->getFloat(offsetRot, 1, len);
	rotation.z = fileblockheader->getFloat(offsetRot, 2, len);
	// Load scale
	int offsetSize = sdna->getMemberOffset("size", *fileblockheader);
	scale.x = fileblockheader->getFloat(offsetSize, 0, len);
	scale.y = fileblockheader->getFloat(offsetSize, 1, len);
	scale.z = fileblockheader->getFloat(offsetSize, 2, len);
	// load animations of the object
	parseAnimation(sdna, fileblockheader);

	FileBlockHeader *data = sdna->getFileBlock("*data", fileblockheader);
	name = data->getString("id.name", sdna);

	parseBones(sdna, data);
	return *this;
}
Blender::Object *Blender::Armature::clone() const{
	return new Armature(*this);
}
std::string Blender::Armature::getType() const {
	return ID;
}