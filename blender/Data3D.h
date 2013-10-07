
#include "utils/Vector3.h"

namespace Blender{
	struct Material {
		float r,g,b, ref;
		const char *name;
	};

	struct WeightDeform
	{
		float weight;
		int def_nr;
		
	};
	struct VertexDeform {
		WeightDeform *weights;
		int totweights;
	};

	struct Vertex {
		Vector3 position;
		Vector3 normal;
		char flag;
		char mat_nr;
		char bweight;
		Vector uv;			// not in blender file format, filled in during MTFace parsing
		bool isUVSet;		// set to true for first texture coord pair set (to prevent overwriting in later faces)
		int nextSupplVert;	// used in duplicteVertex mode to to point to next additional vertex
		VertexDeform *dvert;
	};

	struct Face{
		int v1;
		int v2;
		int v3;
		//int v4; removed: we only have triangles
		char mat_nr;
		char edcode;
		char flag;
		bool isQuad;
		bool supplV1;  // true if v1 is index from duplicated vertices
		bool supplV2;  // true if v2 is index from duplicated vertices
		bool supplV3;  // true if v3 is index from duplicated vertices
	};

	struct FaceUV{
		float uv[3][2];  // this is [4][2] in Blender to support Quads - we need only 3 for each triangle
		//void *tpage;

		//int v3;
		//int v4;
		//char mat_nr;
		//char edcode;
		//char flag;
	};
}