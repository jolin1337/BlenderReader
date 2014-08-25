#include "Armature.h"

std::string Blender::Armature::ID = "Armature";

std::string Blender::Pose::ID = "PoseBone";



Blender::Object *Blender::Pose::clone() const {
	return new Pose;
}
std::string Blender::Pose::getType() const  {
	return ID;
}

Blender::Bone *Blender::Armature::getBone(std::string str) {
	for (std::vector<Blender::Bone>::iterator it = bones.begin(); it != bones.end(); ++it) {
		Blender::Bone &parent = *it;
		std::cout << "kjhkjdhfkjd";
		return 0;
		if(parent.name == str)
			return &parent;
		std::vector<Blender::Bone *> children(parent.children.size(), 0);

		for (unsigned int i = 0; i < parent.children.size(); i++) 
			children[i] = &(parent.children[i]);

		while(!children.empty()) {
			Blender::Bone *b = children.back();
			children.pop_back();
			if(b == 0) continue;
			if(b->name == str)
				return b;

			for (unsigned int i = 0; i < b->children.size(); i++) 
				children.push_back(&(b->children[i]));
		}
	}
	return 0;
}
int Blender::Armature::parseAnimation(StructureDNA *sdna, FileBlockHeader *fbh) {
	return adr.parse(fbh, sdna);
}

Blender::Bone Blender::Armature::readBone(StructureDNA *sdna, FileBlockHeader *fbh) {
	Bone bone;
	bone.bone_mat = Matrix4();
	bone.parent = 0;
	int len = sizeof(float);
	bone.name = fbh->getString("name", sdna);

				// std::cout << "Name: " << bone.name;
	int offsetHead = sdna->getMemberOffset("head", *fbh);
	int offsetTail = sdna->getMemberOffset("tail", *fbh);
	int offsetMat = sdna->getMemberOffset("bone_mat", *fbh);
	for (int i = 0; i < 16; i++) {
		bone.bone_mat[i/4][i%4] = fbh->getFloat(offsetMat, i, len);
		// if(i%4 == 0) std::cout << "\n";
		// std::cout << bone.bone_mat[i/4][i%4] << " ";
	}
	// std::cout << "\n";
	bone.head.x = fbh->getFloat(offsetHead, 0, len);
	bone.head.y = fbh->getFloat(offsetHead, 1, len);
	bone.head.z = fbh->getFloat(offsetHead, 2, len);

	bone.tail.x = fbh->getFloat(offsetTail, 0, len);
	bone.tail.y = fbh->getFloat(offsetTail, 1, len);
	bone.tail.z = fbh->getFloat(offsetTail, 2, len);
				// std::cout << "Name: " << bone.name << "\n";
				// std::cout << "  Head: " << bone.head.x << ", " << bone.head.y << ", " << bone.head.z << "\n";
				// std::cout << "  Tail: " << bone.tail.x << ", " << bone.tail.y << ", " << bone.tail.z << "\n";

	bone.segments = fbh->getFloat("segments", sdna);
	bone.roll = fbh->getFloat("roll", sdna);
	return bone;
}
int Blender::Armature::parseBones(StructureDNA *sdna, FileBlockHeader *fbh) {
	FileBlockHeader *base = sdna->getFileBlock("bonebase.*first", fbh);
	if(base == 0) return 0;

	while(base) {
		// printFields(base, sdna);
		// base = sdna->getFileBlock("*next", base);
		bones.push_back(readBone(sdna, base));
		listBones.push_back(&(bones.back()));

		std::vector<FileBlockHeader *> childbases;		// parent that has childs
		std::vector<Bone *> childBonebases;				// same as childbases but with bones instead

		childBonebases.push_back(&(bones.back())); // push current read bone
		childbases.push_back(sdna->getFileBlock("childbase.*first", base));

		// if((std::string)childBonebases.back()->name == "Bone")
		// 	printFields(base, sdna);

		while(!childbases.empty()) {
			// std::vector<Bone> childs;
			while(childbases[0]) {
				Bone bone = readBone(sdna, childbases[0]);
				// childs.push_back(readBone(sdna, childbases[0]));

				// if((std::string)childs.back().name == "Bone") {
					// BlenderGlobals::Log("adlkjsad");
					// printFields(childbases[0], sdna);
				// }
				// std::cout << childBonebases[0]->name << "\n";
				bone.parent = childBonebases[0];
				// childs[childs.size()-1].parent = childBonebases[0];

				childBonebases[0]->children.push_back(bone);
				childBonebases.push_back(&(childBonebases[0]->children.back()));
				listBones.push_back(&(childBonebases[0]->children.back()));
				// childBonebases.push_back(&childs[childs.size()-1]);

				childbases.push_back(sdna->getFileBlock("childbase.*first", childbases[0]));
				childbases[0] = sdna->getFileBlock("*next", childbases[0]);
			}
			// childBonebases[0]->children = std::vector<Bone>(childs.begin(), childs.end());
			childBonebases.erase(childBonebases.begin());
			childbases.erase(childbases.begin());
		}
		base = sdna->getFileBlock("*next", base);
	}
	
	return 1;
} 

Blender::Pose Blender::Armature::readPose(StructureDNA *sdna, FileBlockHeader *fbh) {
	return Pose();
}
int Blender::Armature::parsePose(StructureDNA *sdna, FileBlockHeader *pose) {
	pose = sdna->getFileBlock("chanbase.*first", pose);
	int len = sizeof(float);
	while(pose) { // all bones loops here despite of childs
		Pose p;
		// std::cout << pose->getString("name", sdna) << "\n";
		p.name = pose->getString("name", sdna);
		int offset = sdna->getMemberOffset("loc", *pose);
		p.location.x = pose->getFloat(offset, 0, len);
		p.location.y = pose->getFloat(offset, 1, len);
		p.location.z = pose->getFloat(offset, 2, len);

		Rotmode rmode = (Rotmode)pose->getInt("rotmode", sdna);
		if(rmode == ROT_MODE_QUAT) {
			// read quaternion rotation
			BlenderGlobals::Error("Not implemented quaternion rotation in Armature.");
			p.rotation = Vector3();
		}
		else if(rmode == ROT_MODE_AXISANGLE || rmode == ROT_MODE_MIN || rmode == ROT_MODE_MAX) {
			// Axis angles
			// p.rotation = Vector3(x*cos(a)*cos(a), y*sin(a)*sin(a), z*sin(a)*cos(a)); // ???
			BlenderGlobals::Error("Not implemented axis angle rotation in Armature.");
			p.rotation = Vector3();
		}
		else {
			offset = sdna->getMemberOffset("eul", *pose);
			int x = pose->getFloat(offset, 0, len),
				y = pose->getFloat(offset, 1, len),
				z = pose->getFloat(offset, 2, len);
			switch(rmode) {
				//case ROT_MODE_EUL:
				case ROT_MODE_XYZ:
					p.rotation = Vector3(x,y,z);
				break;
				case ROT_MODE_XZY:
					p.rotation = Vector3(x,z,y);
				break;
				case ROT_MODE_YXZ:
					p.rotation = Vector3(y,x,z);
				break;
				case ROT_MODE_YZX:
					p.rotation = Vector3(y,z,x);
				break;
				case ROT_MODE_ZXY:
					p.rotation = Vector3(z,x,y);
				break;
				default:
					throw "Uknown rotation type";
			}
		}

		offset = sdna->getMemberOffset("size", *pose);
		p.scale.x = pose->getFloat(offset, 0, len);
		p.scale.y = pose->getFloat(offset, 1, len);
		p.scale.z = pose->getFloat(offset, 2, len);
		poseBones.push_back(p);
		pose = sdna->getFileBlock("*next", pose);
	}
	return 1;
}

Blender::Armature &Blender::Armature::parse(StructureDNA *sdna, FileBlockHeader *fileblockheader) {
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
	// printFields(pose, sdna);
	if(data) {
		name = data->getString("id.name", sdna);
		parseBones(sdna, data);
	}
	FileBlockHeader *pose = sdna->getFileBlock("*pose", fileblockheader);
	if(pose)
		parsePose(sdna, pose);

	return *this;
}
Blender::Object *Blender::Armature::clone() const{
	return new Armature(*this);
}
std::string Blender::Armature::getType() const {
	return ID;
}
#include <algorithm>
Blender::Armature::Armature(){}
Blender::Armature::Armature(const Armature &arm):bones(arm.bones), listBones(), poseBones(arm.poseBones) {
	// not working.
	// for (std::list<Bone *>::const_iterator it = arm.listBones.begin(); it != arm.listBones.end(); ++it)
	// {
	// 	listBones.push_back(*it);
	// }

	// return;
	std::vector<Bone *> vissitedBones;
	std::list<Bone *> queuelist;
	for (std::vector<Bone>::iterator it = bones.begin(); it != bones.end(); ++it)
		queuelist.push_back(&(*it));	

	while(!queuelist.empty()) {
		Bone * v = queuelist.front();
		queuelist.pop_front();
		if(std::find(vissitedBones.begin(), vissitedBones.end(), v) == vissitedBones.end()) {
			vissitedBones.push_back(v);
			for (std::vector<Bone>::iterator i = v->children.begin(); i != v->children.end(); ++i)
				queuelist.push_back(&(*i));
		}
	}
	for (std::vector<Bone *>::iterator it = vissitedBones.begin(); it != vissitedBones.end(); ++it)
		listBones.push_back(*it);
	return;
	std::list<Bone *>::iterator listit = listBones.begin();
	for (std::vector<Blender::Bone>::iterator it = bones.begin(); it != bones.end(); ++it) {
		Blender::Bone &parent = *it;
		// if(listBones.end() == listit) {
		listBones.push_back(&parent);
		listit = listBones.end();
		listit--;
		// }
		// else{
		// 	*listit = &parent; listit++;
		// }
		std::vector<Blender::Bone *> children(parent.children.size(), 0);
		std::vector<Blender::Bone *> parents(1, &parent);

		for (unsigned int i = 0; i < parent.children.size(); i++) 
			children[i] = &(parent.children[i]);

		while(!children.empty()) {
			Blender::Bone *b = children.back();
			children.pop_back();
			if(b == 0) continue;

			if(listBones.end() == listit) {
				listBones.push_back(&parent);
				listit = listBones.end();
				listit--;
			}
			else{
				*listit = &parent; listit++;
			}
			// b->parent = getBone(b->parent->name);
			for (std::list<Bone *>::iterator it2 = listBones.begin(); it2 != listBones.end(); ++it2)
				if((*it2)->name == b->parent->name) {
					b->parent = *it2;
					break;
				}

			for (unsigned int i = 0; i < b->children.size(); i++) 
				children.push_back(&(b->children[i]));
		}
	}
	return;

	while(listit != listBones.end()) {
		listBones.erase(listit);
		listit++;
	}
}
Blender::Armature &Blender::Armature::operator =(const Armature &rhs) {
	Armature arm(rhs);
	std::swap(arm.bones, bones);
	std::swap(arm.poseBones, poseBones);
	std::swap(arm.listBones, listBones);
	return *this;
}

Blender::Bone::Bone(const Bone &bone):name(bone.name), parent(bone.parent), children(),
									  roll(bone.roll), segments(bone.segments), 
									  head(bone.head), tail(bone.tail), bone_mat(bone.bone_mat) {
	for (std::vector<Bone>::const_iterator it = bone.children.begin(); it != bone.children.end(); ++it) {
		children.push_back(*it);
		children.back().parent = this;
	}
}
Blender::Bone::Bone(): name(0), parent(0) {}
Blender::Bone::~Bone() {
	for (std::vector<Bone>::iterator i = children.begin(); i != children.end(); ++i)
		i->parent = 0;
}