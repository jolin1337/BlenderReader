#ifndef OBJECTBLEND
#define OBJECTBLEND
#include "AnimData.h"
#include "utils/Vector3.h"
#include <string>

namespace Blender{
	class Object {
	public:
		static std::string ID;			// identifire for this type of poly. object
		virtual Object *clone() const = 0;
		virtual std::string getType() const = 0;

		const char *name;				// name of the mesh
		AnimData adr;					// animation data for this object
		Vector3 location, 				// main-position of the object
				rotation, 				// main-rotation of the object
				scale;					// main-scale of the object
		Object *parent;					// the object where this object is parented to
	};
}
#endif