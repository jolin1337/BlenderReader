#ifndef ARMATUREH
#define ARMATUREH 

#include "Object.h"
#include "BlenderGlobals.h"
#include "DNA.h"
#include <string>

namespace Blender {
	struct Bone {
		Bone *next, *prev;
		Bone *parent;
		std::vector<Bone> children;
	};


	class Armature : public Object {
	public:
		static std::string ID;

		int parseAnimation(StructureDNA *sdna, FileBlockHeader *fbh);

		int parseBones(StructureDNA *sdna, FileBlockHeader *fbh);

		Armature &parse(StructureDNA *sdna, FileBlockHeader *fileblockheader);
		virtual Object *clone() const;
		virtual std::string getType() const;
	};
}


#endif
