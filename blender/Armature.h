#ifndef ARMATUREH
#define ARMATUREH 

#include "Object.h"
#include "BlenderGlobals.h"
#include "DNA.h"
#include <string>
#include <list>
#include "utils/Matrix4.h"

namespace Blender {
	class Bone {
	public:
		// Bone *next, *prev;
		const char *name;				// name of the bone
		Bone *parent;
		std::vector<Bone> children;
		float roll;
		float segments;
		Vector3 head, tail;
		Matrix4 bone_mat;
		Bone(const Bone &bone);
		Bone();
		~Bone();
	};
	class Pose : public Object {
	public:
		static std::string ID;			// identifire for this type of poly. object

		virtual Object *clone() const;
		virtual std::string getType() const;
		Matrix4 mat;
	};

	class Armature : public Object {

		enum Rotmode {  
			ROT_MODE_QUAT   = 0,
			    /* euler rotations - keep in sync with enum in BLI_math.h */
			ROT_MODE_EUL = 1,       /* Blender 'default' (classic) - must be as 1 to sync with BLI_math_rotation.h defines */
			ROT_MODE_XYZ = 1,
			ROT_MODE_XZY,
			ROT_MODE_YXZ,
			ROT_MODE_YZX,
			ROT_MODE_ZXY,
			ROT_MODE_ZYX,
			/* NOTE: space is reserved here for 18 other possible 
			 * euler rotation orders not implemented 
			 */
			    /* axis angle rotations */
			ROT_MODE_AXISANGLE = -1,
			ROT_MODE_MIN = ROT_MODE_AXISANGLE,  /* sentinel for Py API */
			ROT_MODE_MAX = ROT_MODE_ZYX
		};
	public:
		typedef std::vector<Pose>::iterator PoseIterator;
		typedef std::vector<Pose>::const_iterator const_PoseIterator;
		static std::string ID;

		Armature();
		Armature(const Armature &arm);
		Armature &operator =(const Armature &rhs);

		int parseAnimation(StructureDNA *sdna, FileBlockHeader *fbh);

		int parseBones(StructureDNA *sdna, FileBlockHeader *fbh);
		int parsePose(StructureDNA *sdna, FileBlockHeader *fbh);
		Bone *getBone(std::string str);

		Armature &parse(StructureDNA *sdna, FileBlockHeader *fileblockheader);
		virtual Object *clone() const;
		virtual std::string getType() const;

		PoseIterator begin() {
			return poseBones.begin();
		}
		PoseIterator end() {
			return poseBones.end();
		}
		const_PoseIterator begin() const {
			return poseBones.begin();
		}
		const_PoseIterator end() const {
			return poseBones.end();
		}

	protected:
		Bone readBone(StructureDNA *sdna, FileBlockHeader *fbh);
		Pose readPose(StructureDNA *sdna, FileBlockHeader *fbh);
		std::vector<Bone> bones;
		std::list<Bone *> listBones;
		std::vector<Pose> poseBones;
	};
}


#endif
