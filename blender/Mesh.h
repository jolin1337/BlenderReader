#ifndef MESHH
#define MESHH

#include "BlenderGlobals.h"
#include <vector>
#include "DNA.h"
#include "Object.h"
#include "Data3D.h"

namespace Blender {

	class FileBlockHeader;

	class Mesh : public Object {
	public:
		static std::string ID;
		Mesh(BlenderGlobals::UVMapping = BlenderGlobals::UV_SimpleMode);
		~Mesh();
		
		int totvert;		// total amount of ...
		int totedge;	
		int totface;

		int totcol;
		char objectcolor;	// index of the object color(from the base material)
		std::vector<Material *> materials;// the material where vertex index are counted from

		BlenderGlobals::UVMapping mappingMode;	// default is simpleMode
		std::vector<Vertex > verts;				// vertex array
		std::vector<VertexDeform > dverts;
		std::vector<Face > faces;				// faces array
		std::vector<FaceUV > uvs;				// array of uv coordinates


		std::vector<Vertex > supplVerts; 		// used in duplicteVertex mode to store additional vertices

	private:
		Mesh &parseVerts(StructureDNA *sdna, FileBlockHeader *fbh);
		Mesh &parseVertexGroups(StructureDNA *sdna, FileBlockHeader *fbh);
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