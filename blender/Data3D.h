
#include "utils/Vector3.h"

namespace Blender{
	struct Material {
		float r,g,b, ref;
		const char *name;
	};

	struct WeightDeform
	{
		float weight;			// weight of this object
		int def_nr;				// this is the deformgroup name index
		
	};
	struct VertexDeform {
		WeightDeform *weights;	// deformation groups of this vertex
		int totweights;			// total count of weights
		const char *name;
	};

	struct Vertex {
		Vector3 position;		// position of the read vertex
		Vector3 normal;
		char mat_nr;			// the material of the vertex(is not used)
		Material mat;
		// char bweight;		// what does this do?
		Vector uv;				// not in blender file format, filled in during MTFace parsing
		bool isUVSet;			// set to true for first texture coord pair set (to prevent overwriting in later faces)
		int nextSupplVert;		// used in duplicteVertex mode to to point to next additional vertex
		VertexDeform dvert;	// deformation vertex of this vertex
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