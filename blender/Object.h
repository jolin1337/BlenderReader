#ifndef OBJECTBLEND
#define OBJECTBLEND
#include "AnimData.h"
#include "utils/Vector3.h"
#include <string>

namespace Blender{
	class Object {
		static std::string ID;			// identifire for this type of poly. object
	public:
		Object();
		virtual ~Object();
		virtual Object *clone() const = 0;
		virtual std::string getType() const = 0;
		void setParent(Object *);
		Object * getParent();

		int getFrameCount();
		bool setFrame(int frame);

		const char *name;				// name of the mesh
		AnimData adr;					// animation data for this object
		Vector3 location, 				// main-position of the object
				rotation, 				// main-rotation of the object
				scale;					// main-scale of the object
		Object *parent;					// the object where this object is parented to
	};
}
#endif