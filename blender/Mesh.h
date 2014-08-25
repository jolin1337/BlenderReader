#ifndef MESHH
#define MESHH

#include "BlenderGlobals.h"
#include <vector>
#include "DNA.h"
#include "Object.h"
#include "Data3D.h"

namespace Blender {
	typedef std::vector<Vertex > Vertices;
	typedef std::vector<VertexDeform > DeformVertices;
	typedef std::vector<Face > Faces;
	class FileBlockHeader;

	class Mesh : public Object {
	public:
		static std::string ID;
		Mesh(BlenderGlobals::UVMapping = BlenderGlobals::UV_SimpleMode);
		Mesh(const Mesh &mesh);
		virtual ~Mesh();

		Mesh &operator =(const Mesh &mesh);

		void swap(Mesh m);

		Vertex *getVertices();
		Face *getFaces();

		unsigned int size();
		unsigned int f_size();
		
		int totvert;		// total amount of vertices
		int totedge;		// total amount of edges
		int totface;		// total amount of faces

		int totcol;			// total amount of materials
		
		std::vector<Material *> materials;// the material where vertex index are counted from

		BlenderGlobals::UVMapping mappingMode;	// default is simpleMode
		Vertices verts;				// vertex array
		DeformVertices dverts;
		Faces faces;				// faces array
		std::vector<FaceUV > uvs;				// array of uv coordinates


		std::vector<Vertex > supplVerts; 		// used in duplicteVertex mode to store additional vertices

	private:
		Mesh &parseVerts(StructureDNA *sdna, FileBlockHeader *fbh);
		Mesh &parseVertexGroups(StructureDNA *sdna, FileBlockHeader *fbh);
		Mesh &parseNPoly(StructureDNA *sdna, FileBlockHeader *mpoly, FileBlockHeader *mloop);
		Mesh &parseFaces(StructureDNA *sdna, FileBlockHeader *fbh);
		Mesh &parseMTFaces(StructureDNA *sdna, FileBlockHeader *fbh);
		
		void parseMaterials(StructureDNA *sdna, FileBlockHeader *fbh, std::vector<Material > *mat);
		
		Mesh &setUV(unsigned int faceIndex);
		Mesh &handleUVDuplicationMode(unsigned int faceIndex);

		void parseAnimation(StructureDNA *sdna, FileBlockHeader *fbh);

	public:
		Mesh &parse(StructureDNA *sdna, FileBlockHeader *fileblockheader, std::vector<Material > *mat);
		virtual Object *clone() const{
			return new Mesh(*this);
		}
		virtual std::string getType() const {
			return ID;
		}
	};
}
#endif