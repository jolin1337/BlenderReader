#include "Mesh.h"
#include <vector>
#include <string.h>
#include <sstream>

using namespace std;

std::string Blender::Mesh::ID = "Mesh";

Blender::Mesh &Blender::Mesh::parseVerts(StructureDNA *sdna, FileBlockHeader *fbh) {
	StructureInfo *mvertStructure = fbh->spStruct;//sdna->findStructureType("MVert");
	unsigned int len = sdna->getLength(mvertStructure->typeIndex);

	unsigned int count = fbh->count;
	int offset_co = sdna->getMemberOffset("co[3]", *fbh);
	int offset_no = sdna->getMemberOffset("no[3]", *fbh);
	int offset_mat = sdna->getMemberOffset("mat_nr", *fbh);
	for ( unsigned int k = 0; k < count; k++) {
		Vertex vertex;
		vertex.position.x = fbh->getFloat(offset_co, k, len);
		vertex.position.y = fbh->getFloat(offset_co+4, k, len);
		vertex.position.z = fbh->getFloat(offset_co+8, k, len);
		vertex.normal.x = fbh->getShort(offset_no, k, len);
		vertex.normal.y = fbh->getShort(offset_no+2, k, len);
		vertex.normal.z = fbh->getShort(offset_no+4, k, len);
		vertex.normal.normalize();

		vertex.mat_nr = fbh->getString(offset_mat, k, len)[0];
		
		vertex.isUVSet = false;
		vertex.nextSupplVert = -1;

		// set dverts to 0 so no seg fault happens
		vertex.dvert.totweights = 0;
		vertex.dvert.weights = 0;
		verts.push_back(vertex);
	}
	return *this;
}
Blender::Mesh &Blender::Mesh::parseVertexGroups(StructureDNA *sdna, FileBlockHeader *fbh) {
	if(fbh == 0) return *this;
	FileBlockHeader *fbdefbase = sdna->getFileBlock("defbase.*first", fbh);
	fbh = sdna->getFileBlock("*data", fbh);
	if(fbh == 0) return *this;
	fbh = sdna->getFileBlock("*dvert", fbh);
	if(fbh == 0) return *this;
	unsigned int len = sdna->getLength(fbh->spStruct->typeIndex);
	unsigned int count = fbh->count;

	while(fbdefbase) {
		VertexDeform vertdef;
		vertdef.name = fbdefbase->getString("name", sdna);
		dverts.push_back(vertdef);
		

		fbdefbase = sdna->getFileBlock("*next", fbdefbase);
	}
	int offset_totweight = sdna->getMemberOffset("totweight", *fbh);
	int offset_dw = sdna->getMemberOffset("*dw", *fbh);
	for (unsigned int i = 0; i < count; i++) {

		int address = fbh->getInt(offset_dw, i, len);
		if(address) {
			FileBlockHeader *dw = sdna->getFileBlockByAddress(address);
			if(!dw) continue;
			
			unsigned int dw_count = dw->count;
			VertexDeform &dvert = verts[i].dvert;

			if(dverts.size() < dw_count){
				std::stringstream ss;
				ss << dverts.size() << ";" << count << "\n" << "More groups of vertices than there is vertexgroups";
				BlenderGlobals::Log(ss.str());
				continue;
			}
			unsigned int dw_len = sdna->getLength(dw->spStruct->typeIndex);
			int offset_def_nr = sdna->getMemberOffset("def_nr", *dw);
			int offset_weight = sdna->getMemberOffset("weight", *dw);

			dvert.totweights = fbh->getInt(offset_totweight, i, len);
			dvert.weights = new WeightDeform[dvert.totweights];
			// dw_count is the count of vertex groups of this vertex
			for (int j = 0; j < dvert.totweights; j++) {
				
				dvert.weights[j].def_nr = dw->getInt(offset_def_nr,   j, dw_len);
				dvert.weights[j].weight = dw->getFloat(offset_weight, j, dw_len);
			}
			// verts[i].dvert = dvert;
		}
	}
	return *this;
}
/*

Log: struct MPoly {
       int loopstart = 0;
       int totloop = 4;
       short mat_nr = 0;
       char flag = 2;
       char pad = 0;
}*n;
Log: struct MLoop {
       int v = 0;
       int e = 1;
}*48;

*/
Blender::Mesh &Blender::Mesh::parseNPoly(StructureDNA *sdna, FileBlockHeader *mpoly, FileBlockHeader *mloop) {
	unsigned int len_mpoly = sdna->getLength(mpoly->spStruct->typeIndex);
	unsigned int len_mloop = sdna->getLength(mloop->spStruct->typeIndex);

	unsigned int count = mpoly->count;
	int offset_totloop = sdna->getMemberOffset("totloop", *mpoly),
		offset_mat_nr = sdna->getMemberOffset("mat_nr", *mpoly),
		offset_loopstart = sdna->getMemberOffset("loopstart", *mpoly);
	int offset_v = sdna->getMemberOffset("v", *mloop);
	for (unsigned int i = 0; i < count; i++) {
		int totloop = mpoly->getInt(offset_totloop, i, len_mpoly);
		if(totloop < 5 && totloop > 2) {
			int loopstart = mpoly->getInt(offset_loopstart, i, len_mpoly);
			char mat_nr = mpoly->getShort(offset_mat_nr, i, len_mloop);

			int v[] = {
				mloop->getInt(offset_v, loopstart, len_mloop),
				mloop->getInt(offset_v, loopstart+1, len_mloop),
				mloop->getInt(offset_v, loopstart+2, len_mloop),
				0
			};
			int verts_size = verts.size();
			if(v[0] >= verts_size || v[1] >= verts_size || v[2] >= verts_size)
				continue;

			Face f1;
			f1.supplV1 = false;
			f1.supplV2 = false;
			f1.supplV3 = false;
			f1.isQuad  = false;
			f1.mat_nr = mat_nr;
			f1.v1 = v[0];
			f1.v2 = v[1];
			f1.v3 = v[2];
			// if face-quad:
			if(totloop == 4) {
				v[3] = mloop->getInt(offset_v, loopstart+3, len_mloop);
				if(v[3] >= verts_size)
					continue;
				Face f2;
				f1.isQuad  = true;

				f2.supplV1 = false;
				f2.supplV2 = false;
				f2.supplV3 = false;
				f2.isQuad  = true;
				f2.mat_nr = mat_nr;
				f2.v1 = v[0];
				f2.v2 = v[2];
				f2.v3 = v[3];
				faces.push_back(f2);
			}
			faces.push_back(f1);
		}
		else if(BlenderGlobals::debug_mesh) {
			std::stringstream ss;
			ss << "This face is " << totloop << " long and is not supported right now.";
			BlenderGlobals::Log(ss.str());
		}
	}
	// printFields(fbh, sdna);*/	
	return *this;
}
Blender::Mesh &Blender::Mesh::parseFaces(StructureDNA *sdna, FileBlockHeader *fbh) {
	StructureInfo *mfaceStructure = fbh->spStruct;
	unsigned int len = sdna->getLength(mfaceStructure->typeIndex);

	unsigned int count = fbh->count;
	int offset_v1 = sdna->getMemberOffset("v1", *fbh),
		offset_mat = sdna->getMemberOffset("mat_nr", *fbh);
	// unsigned int outFace = 0, externalFace = 0;			// count output faces
	// faces = vector<Face >(count*2);		// triangulized quads takes approx double room
	for ( unsigned int k = 0; k < count; k++) {
		int v[] = {
			fbh->getInt(offset_v1, k, len),
			fbh->getInt(offset_v1 + 4, k, len),
			fbh->getInt(offset_v1 + 8, k, len),
			fbh->getInt(offset_v1 + 12, k, len)
		};
		char mat_nr = fbh->getShort(offset_mat, k, len);

		for (int l = faces.size(); l > 0; l--) {
			Face f = faces[l-1];
			int *vr = (int *)&f;

			if(f.mat_nr != mat_nr){
				for(int m = 0; m < 3; m++){
					for(int n = 0; n < 4; n++){
						if(vr[m] == v[n]){
							// a color edge has been found
							Vertex copy = verts[v[n]];
							verts.push_back(copy);
							v[n] = verts.size()-1;
						}
					}
				}
			}
		}
		Face f1;
		f1.supplV1 = false;
		f1.supplV2 = false;
		f1.supplV3 = false;
		f1.isQuad  = false;
		f1.mat_nr = mat_nr;
		f1.v1 = v[0];
		f1.v2 = v[1];
		f1.v3 = v[2];
		// if face-quad:
		if (v[3] != 0) {
			Face f2;
			f1.isQuad  = true;

			f2.supplV1 = false;
			f2.supplV2 = false;
			f2.supplV3 = false;
			f2.isQuad  = true;
			f2.mat_nr = mat_nr;
			f2.v1 = v[0];
			f2.v2 = v[2];
			f2.v3 = v[3];
			faces.push_back(f2);
		}
		faces.push_back(f1);
	}
	totface = faces.size();
	return *this;
}
Blender::Mesh &Blender::Mesh::parseMTFaces(StructureDNA *sdna, FileBlockHeader *fbh) {
	if (BlenderGlobals::debug_uv && mappingMode == BlenderGlobals::UV_SimpleMode) 
		BlenderGlobals::Log("UV Mapping Simple mode: No Vertices will be duplicated. Some Textures may be distorted.\n");
	else if (BlenderGlobals::debug_uv && mappingMode == BlenderGlobals::UV_DuplicateVertex) 
		BlenderGlobals::Log("UV Mapping Duplicate Vertex mode: Vertices will be duplicated as needed.\n");

	StructureInfo *mtfaceStructure = fbh->spStruct;//findStructureType("MTFace");
	unsigned int len = sdna->getLength(mtfaceStructure->typeIndex);

	unsigned int count = fbh->count;

	uvs = vector<FaceUV >(count*2);		// triangulized quads takes approx double room

	int offset_uv = sdna->getMemberOffset("uv", *fbh);
	BlenderGlobals::Log(offset_uv);
	//int offset_tpage = getMemberOffset("*tpage", sdna, *fbh);
	unsigned int outFace = 0;  //count output faces
	for ( unsigned int k = 0; k < count; k++) {
		//memcpy(&uvs[outFace].uv, fbh->buf + k * len + offset_uv, sizeof(float[4][2]));
		//uvs[outFace].tpage = (void*)(fbh->buf + k * len + offset_tpage);
		
		uvs[outFace].uv[0][0] = fbh->getFloat(offset_uv, k, len);
		uvs[outFace].uv[0][1] = 1.0f - fbh->getFloat(offset_uv + 4, k, len);
		uvs[outFace].uv[1][0] = fbh->getFloat(offset_uv + 8, k, len);
		uvs[outFace].uv[1][1] = 1.0f - fbh->getFloat(offset_uv + 12, k, len);
		uvs[outFace].uv[2][0] = fbh->getFloat(offset_uv + 16, k, len);
		uvs[outFace].uv[2][1] = 1.0f - fbh->getFloat(offset_uv + 20, k, len);
		setUV(outFace);
		if (faces[outFace].isQuad) {
			outFace++;
			uvs[outFace].uv[0][0] = fbh->getFloat(offset_uv, k, len);
			uvs[outFace].uv[0][1] = 1.0f - fbh->getFloat(offset_uv + 4, k, len);
			uvs[outFace].uv[1][0] = fbh->getFloat(offset_uv + 16, k, len);
			uvs[outFace].uv[1][1] = 1.0f - fbh->getFloat(offset_uv + 20, k, len);
			uvs[outFace].uv[2][0] = fbh->getFloat(offset_uv + 24, k, len);
			uvs[outFace].uv[2][1] = 1.0f - fbh->getFloat(offset_uv + 28, k, len);
			setUV(outFace);

			//memcpy(&uvs[outFace].uv, fbh->buf + k * len + offset_uv, sizeof(float[4][2]));
			//uvs[outFace].tpage = (void*)(fbh->buf + k * len + offset_tpage);
		}
		outFace++;
	}
	return *this;
}

void Blender::Mesh::parseMaterials(StructureDNA *sdna, FileBlockHeader *fbh, std::vector<Material > *mat) {
	if(mat == 0) return;
	if(fbh == 0 || totcol == 0)  {
		materials.push_back(&(mat->at(0)));
		return;
	}

	FileBlockHeader *mfbh = sdna->getFileBlock("**mat", fbh);

	if(mfbh) {
		for (int i = 0; i < totcol; i++) {
			int addr = mfbh->getInt(0, i, sizeof(int*));

			FileBlockHeader *color = sdna->getFileBlockByAddress(addr);
			if(color == 0) return;

			std::string colname = color->getString("id.name", sdna);

			for (unsigned int j = 0; j < mat->size(); j++) {
				if(colname == mat->at(j).name){
					materials.push_back(&(mat->at(j)));
					break;
				}
			}
		}
	}
	materials.push_back(&mat->at(0));
}

void Blender::Mesh::parseAnimation(StructureDNA *sdna, FileBlockHeader *fbh) {
	adr.parse(fbh, sdna);
}

Blender::Mesh &Blender::Mesh::parse(StructureDNA *sdna, FileBlockHeader *fileblockheader, std::vector<Material > *mat) {
	if(fileblockheader == 0 || sdna == 0) return *this;
	if (BlenderGlobals::debug_basic) BlenderGlobals::Log("Parse Mesh data");
	this->supplVerts.clear();
	this->verts.clear();
	this->faces.clear();
	this->uvs.clear();
		unsigned int len = sizeof(float);
		int offsetLoc = sdna->getMemberOffset("loc", *fileblockheader);
		location.x = fileblockheader->getFloat(offsetLoc, 0, len);
		location.y = fileblockheader->getFloat(offsetLoc, 1, len);
		location.z = fileblockheader->getFloat(offsetLoc, 2, len);
		int offsetRot = sdna->getMemberOffset("rot", *fileblockheader);
		rotation.x = fileblockheader->getFloat(offsetRot, 0, len);
		rotation.y = fileblockheader->getFloat(offsetRot, 1, len);
		rotation.z = fileblockheader->getFloat(offsetRot, 2, len);
		int offsetSize = sdna->getMemberOffset("size", *fileblockheader);
		scale.x = fileblockheader->getFloat(offsetSize, 0, len);
		scale.y = fileblockheader->getFloat(offsetSize, 1, len);
		scale.z = fileblockheader->getFloat(offsetSize, 2, len);

		parseAnimation(sdna, fileblockheader);
		FileBlockHeader *fbdata = sdna->getFileBlock("*data", fileblockheader);
		if(fbdata == 0) return *this;

		totvert = fbdata->getInt("totvert", sdna);
		totedge = fbdata->getInt("totedge", sdna);
		totface = fbdata->getInt("totface", sdna);
		totcol = fbdata->getShort("totcol", sdna);
		name = fbdata->getString("id.name", sdna);

		if(mat != 0)
			parseMaterials(sdna, fbdata, mat);
		// for (int i = 0; i < totcol; i++)
		// 	BlenderGlobals::Log(materials[i]->name);
		int address = fbdata->getInt("*mvert", sdna);
		if(address != 0)
			parseVerts(sdna, sdna->getFileBlockByAddress(address));
		address = fbdata->getInt("*mface", sdna);
		if(address != 0)
			parseFaces(sdna, sdna->getFileBlockByAddress(address));
		address = fbdata->getInt("*mpoly", sdna);
		int address2 = fbdata->getInt("*mloop", sdna);
		if(address != 0 && address2 != 0)
			parseNPoly(sdna, sdna->getFileBlockByAddress(address), sdna->getFileBlockByAddress(address2));
		address = fbdata->getInt("*mtface", sdna);
		if(address != 0)
			parseMTFaces(sdna, sdna->getFileBlockByAddress(address));

		// address = fbdata->getInt("*dvert", sdna);
		// if(address != 0)
		parseVertexGroups(sdna, fileblockheader);

		if (BlenderGlobals::debug_mesh) {
			std::ostringstream oss;
			oss << "Mesh name = " << name << "\n"
				<< "   :  Vertex size = " << totvert << "\n"
				<< "   :  Edge size   = " << totedge << "\n"
				<< "   :  Face size   = " << totface << "\n"
				<< "   :  Col size    = " << totface << "\n"
				<< "   :  vertices for UV mapping = " << supplVerts.size() << "\n";
			BlenderGlobals::Log(oss.str());
		}
	return *this;
}





int findMatchingVert(Blender::Mesh *mesh, Blender::Vertex *vert, Blender::FaceUV *mtface, int index) {
	// try to find a matching additional vertex with same uv coords:
	Blender::Vertex *vertSearch = vert;
	while (vertSearch->nextSupplVert != -1) {
		int i = vertSearch->nextSupplVert;
		vertSearch = &mesh->supplVerts[i];
		if (vertSearch->uv.x == mtface->uv[index][0] && vertSearch->uv.y == mtface->uv[index][1]) {
			// found matching vertex index in suppl list
			return i;
		}
	}
	return -1;
}
int createDuplicateVertex(Blender::Mesh *mesh, Blender::Vertex *vert) {
	Blender::Vertex newVert = *vert;
	// vert->nextSupplVert is copied, so we insert at head of supplement vertices
	mesh->supplVerts.push_back(newVert);
	vert->nextSupplVert = (int)mesh->supplVerts.size() - 1;
	return vert->nextSupplVert;
}
void checkCreateDuplicateVertex(int index, Blender::Mesh *mesh, Blender::Face *mface, Blender::FaceUV *mtface) {
	Blender::Vertex *vert;
	switch (index) {
		case 0: 
			vert = &mesh->verts[mface->v1];
			break;
		case 1: 
			vert = &mesh->verts[mface->v2];
			break;
		case 2: 
			vert = &mesh->verts[mface->v3];
			break;
		default:
			vert = 0;
	}
	 
	if (vert->isUVSet && (vert->uv.x != mtface->uv[index][0] || vert->uv.y != mtface->uv[index][1])) {
		// try to find a matching additional vertex with same uv coords:
		int supplMVertIndex = findMatchingVert(mesh, vert, mtface, index);
		if (supplMVertIndex == -1) {
			// not found: create duplicate of this vertex:
			supplMVertIndex = createDuplicateVertex(mesh, vert);
		}
		switch (index) {
			case 0: 
				mface->v1 = supplMVertIndex;
				mface->supplV1 = true;
				break;
			case 1: 
				mface->v2 = supplMVertIndex;
				mface->supplV2 = true;
				break;
			case 2: 
				mface->v3 = supplMVertIndex;
				mface->supplV3 = true;
				break;
		}
		mesh->supplVerts[supplMVertIndex].uv.x = mtface->uv[index][0];
		mesh->supplVerts[supplMVertIndex].uv.y = mtface->uv[index][1];
	}
}
Blender::Mesh &Blender::Mesh::setUV(unsigned int faceIndex) {
	FaceUV *mtface = &uvs[faceIndex];
	Face *face = &faces[faceIndex];
	// go through all vertices of the face and set uv coordinates:
	Vertex *vert = &verts[face->v1];
	bool v1AlreadySet = false;
	bool v2AlreadySet = false;
	bool v3AlreadySet = false;
	if (!vert->isUVSet) {
		vert->uv.x = mtface->uv[0][0];
		vert->uv.y = mtface->uv[0][1];
		vert->isUVSet = true;
	} else {
		if (vert->uv.x != mtface->uv[0][0] || vert->uv.y != mtface->uv[0][1])
			v1AlreadySet = true;
	}
	vert = &verts[face->v2];
	if (!vert->isUVSet) {
		vert->uv.x = mtface->uv[1][0];
		vert->uv.y = mtface->uv[1][1];
		vert->isUVSet = true;
	} else {
		if (vert->uv.x != mtface->uv[1][0] || vert->uv.y != mtface->uv[1][1])
			v2AlreadySet = true;
	}
	vert = &verts[face->v3];
	if (!vert->isUVSet) {
		vert->uv.x = mtface->uv[2][0];
		vert->uv.y = mtface->uv[2][1];
		vert->isUVSet = true;
	} else {
		if (vert->uv.x != mtface->uv[2][0] || vert->uv.y != mtface->uv[2][1])
			v3AlreadySet = true;
	}
	if (v1AlreadySet || v2AlreadySet || v3AlreadySet) {
		if (BlenderGlobals::debug_uv) {
			std::ostringstream oss;
			oss << " face " << faceIndex << " has uv already set on ";
			if (v1AlreadySet) oss << "v1 ";
			if (v2AlreadySet) oss << "v2 ";
			if (v3AlreadySet) oss << "v3 ";

			BlenderGlobals::Log(oss.str());
		}
		if (mappingMode == BlenderGlobals::UV_DuplicateVertex) {
			handleUVDuplicationMode(faceIndex);
		}
	}
	return *this;
}

Blender::Mesh &Blender::Mesh::handleUVDuplicationMode(unsigned int faceIndex) {
	FaceUV *mtface = &uvs[faceIndex];
	Face *mface = &faces[faceIndex];
	// go through all vertices of the face and set uv coordinates:
	// for vertices that didn't have uv already set,
	// uv had to be set into regular vertices before calling this method
	checkCreateDuplicateVertex(0, this, mface, mtface);
	checkCreateDuplicateVertex(1, this, mface, mtface);
	checkCreateDuplicateVertex(2, this, mface, mtface);
	Vertex *vert = &verts[mface->v1];
	if (vert->isUVSet && vert->uv.x != mtface->uv[0][0] && vert->uv.y != mtface->uv[0][1]) {
		// try to find a matching additional vertex with same uv coords:
		int supplMVertIndex = findMatchingVert(this, vert, mtface, 0);
		if (supplMVertIndex != -1) {
			mface->v1 = supplMVertIndex;
			mface->supplV1 = true;
		} else {
			supplMVertIndex = createDuplicateVertex(this, vert);
			mface->v1 = supplMVertIndex;
			mface->supplV1 = true;
			supplVerts[supplMVertIndex].uv.x = mtface->uv[0][0];
			supplVerts[supplMVertIndex].uv.y = mtface->uv[0][1];
		}
	}
	return *this;
}

void Blender::Mesh::swap(Mesh m) {
	std::swap(totvert, m.totvert);
	std::swap(totedge, m.totedge);
	std::swap(totface, m.totface);
	std::swap(totcol, m.totcol);
	std::swap(materials, m.materials);
	std::swap(mappingMode, m.mappingMode);
	std::swap(verts, m.verts);
	std::swap(dverts, m.dverts);
	std::swap(faces, m.faces);
	std::swap(uvs, m.uvs);
	std::swap(supplVerts, m.supplVerts);
}




// mesh implementations:
Blender::Mesh::Mesh(BlenderGlobals::UVMapping mode):totvert(0), totedge(0), totface(0), totcol(0), mappingMode(mode) {
}
Blender::Mesh::Mesh(const Mesh &mesh):Blender::Object(mesh), 
										totvert(mesh.totvert), totedge(mesh.totedge), totface(mesh.totface), totcol(mesh.totcol),
										materials(mesh.materials), mappingMode(mesh.mappingMode), 
										verts(mesh.verts), dverts(mesh.dverts), faces(mesh.faces), uvs(mesh.uvs), 
										supplVerts(mesh.supplVerts) {
	for (Vertices::iterator it = verts.begin(); it != verts.end(); ++it) {
		if(it->dvert.weights != 0) {
			WeightDeform *wd = it->dvert.weights;
			it->dvert.weights = new WeightDeform[it->dvert.totweights];
			for (int i = 0; i < it->dvert.totweights; i++)
				it->dvert.weights[i] = wd[i];
		}
	}
}

Blender::Mesh::~Mesh() {
	for (unsigned int i = 0; i < verts.size(); i++)
		if(verts[i].dvert.weights != 0)
			delete[] verts[i].dvert.weights;
}

Blender::Mesh &Blender::Mesh::operator =(const Mesh &rhs) {
	Mesh m(rhs);
	swap(m);
	return *this;
}


Blender::Vertex *Blender::Mesh::getVertices() {
	return &verts[0];
}
Blender::Face *Blender::Mesh::getFaces() {
	return &faces[0];
}


unsigned int Blender::Mesh::size(){
	return verts.size();
}
unsigned int Blender::Mesh::f_size(){
	return faces.size();
}