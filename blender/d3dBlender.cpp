
// #include "DXUT.h"

// next include is from Frank D. Luna's great book, so I guess I am not allowed to repeat its definitions here.
// go get his great boot (A shader's approach to DirectX 9)
// #include "d3dUtil.h"	// Mtrl and device
#include "d3dBlender.h"
// #include "Vertex.h"		// VertexPNT
//#include "WorldObject.h"
// #include "string2wstring.h"
#include <algorithm>
#include <list>
#include <iostream>
#include <fstream>
using namespace std;
#include "Blender.h"

// comment/uncomment next lines to disable/enable extended logging
//#define DEBUG_VERTICES


class D3dUtilBlender : public BlenderCallback
{
public:
	virtual void meshLoaded(Mesh *mesh);
	// ID3DXMesh *d3dMesh;
};

// void D3dUtilBlender::meshLoaded(Mesh *mesh) {
// 	//Blender::Log("verticesLoaded callback called\n");
	
// 	// Create the mesh
// 	HRESULT hr;
// 	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
// 	UINT numElems = 0;
// 	V(VertexPNT::Decl->GetDeclaration(elems, &numElems));

// 	ID3DXMesh* pMesh = NULL;
// 	V( D3DXCreateMesh(
// 		mesh->totface,
// 		mesh->totvert + mesh->supplMVerts.size(), // total vertices = verts in blender file + created on the fly for UV mapping
// 		D3DXMESH_MANAGED | D3DXMESH_32BIT,
// 		elems,
// 		gd3dDevice,
// 		&pMesh ) );
// 	d3dMesh = pMesh;

// 	// Copy the vertex data
// 	VertexPNT* pVertex;
// 	V( pMesh->LockVertexBuffer( 0, (void**) &pVertex ) );
// 	for (int i = 0; i < mesh->totvert; i++) {
// 		// vertices
// 		pVertex[i].pos.x = mesh->mverts[i].co[0];
// 		pVertex[i].pos.y = mesh->mverts[i].co[1];
// 		pVertex[i].pos.z = mesh->mverts[i].co[2];
// 		// normalize blender normal vector: (x,y,z) / ||(x,y,z)||
// 		D3DXVECTOR3 normalized = D3DXVECTOR3(mesh->mverts[i].no[0], mesh->mverts[i].no[1], mesh->mverts[i].no[2]);
// 		D3DXVec3Normalize(&pVertex[i].normal, &normalized);
// 		// texture coordiantes
// 		pVertex[i].tex0.x = mesh->mverts[i].uv[0];
// 		pVertex[i].tex0.y = mesh->mverts[i].uv[1];
		
// 		#ifdef DEBUG_VERTICES
// 		std::ostringstream oss;
// 		oss << i << " vertex: " << pVertex[i].pos.x << " " << pVertex[i].pos.y << " " << pVertex[i].pos.z << " uv " << pVertex[i].tex0.x << " " << pVertex[i].tex0.y << "\n";
// 		Blender::Log(oss.str());
// 		#endif
// 	}
// 	int max = mesh->totvert;
// 	for (unsigned int i = 0; i < mesh->supplMVerts.size(); i++) {
// 		// vertices
// 		pVertex[max + i].pos.x = mesh->supplMVerts[i].co[0];
// 		pVertex[max + i].pos.y = mesh->supplMVerts[i].co[1];
// 		pVertex[max + i].pos.z = mesh->supplMVerts[i].co[2];
// 		// normalize blender normal vector: (x,y,z) / ||(x,y,z)||
// 		D3DXVECTOR3 normalized = D3DXVECTOR3(mesh->supplMVerts[i].no[0], mesh->supplMVerts[i].no[1], mesh->supplMVerts[i].no[2]);
// 		D3DXVec3Normalize(&pVertex[max + i].normal, &normalized);
// 		// texture coordiantes
// 		pVertex[max + i].tex0.x = mesh->supplMVerts[i].uv[0];
// 		pVertex[max + i].tex0.y = mesh->supplMVerts[i].uv[1];

// 		#ifdef DEBUG_VERTICES
// 		std::ostringstream oss;
// 		oss << max + i << " vertex: " << pVertex[max + i].pos.x << " " << pVertex[max + i].pos.y << " " << pVertex[max + i].pos.z << " uv " << pVertex[max + i].tex0.x << " " << pVertex[max + i].tex0.y << "\n";
// 		Blender::Log(oss.str());
// 		#endif
// 	}
// 	//memcpy( pVertex, m_Vertices.GetData(), m_Vertices.GetSize() * sizeof(VERTEX) );
// 	pMesh->UnlockVertexBuffer();

// 	// Copy the index data
// 	DWORD* pIndex;
// 	//V( pMesh->LockIndexBuffer( 0, (void**) &pIndex ) );
// 	V( pMesh->LockIndexBuffer( 0, (void**) &pIndex ) );
// 	for ( int i = 0; i < mesh->totface; i++) {
// 		// convert one input rect to 2 output triangles
// 		if (mesh->mfaces[i].supplV1) {
// 			pIndex[i*3+0] = mesh->mfaces[i].v1 + max;
// 		} else {
// 			pIndex[i*3+0] = mesh->mfaces[i].v1;
// 		}
// 		if (mesh->mfaces[i].supplV2) {
// 			pIndex[i*3+1] = mesh->mfaces[i].v2 + max;
// 		} else {
// 			pIndex[i*3+1] = mesh->mfaces[i].v2;
// 		}
// 		if (mesh->mfaces[i].supplV3) {
// 			pIndex[i*3+2] = mesh->mfaces[i].v3 + max;
// 		} else {
// 			pIndex[i*3+2] = mesh->mfaces[i].v3;
// 		}
// 		#ifdef DEBUG_VERTICES
// 		std::ostringstream oss;
// 		oss << i << " index: " << pIndex[i*3+0] << " " << pIndex[i*3+1] << " " << pIndex[i*3+2] << " " << "\n";
// 		Blender::Log(oss.str());
// 		#endif
// 	}
// 	pMesh->UnlockIndexBuffer();
// }

// void LoadBlenderFile(
// 	const std::string& filename, 
// 	ID3DXMesh** meshOut,
// 	std::vector<Mtrl>& mtrls, 
// 	std::vector<IDirect3DTexture9*>& texs,
// 	UVMapping uvMapping)
// {
// 	// Step 1: Define callback for blender file parsing

// 	D3dUtilBlender blenderCallback;
// 	Blender blender(&blenderCallback);

// 	// Step 2: Parse blender file

// 	// during the following call the above callback method will be called
// 	if (!blender.parseBlenderFile(filename, uvMapping)) {
// 		return; // parse errors
// 	}

// 	HRESULT hr;
// 	ID3DXMesh* meshSys      = blenderCallback.d3dMesh;

// 	// Step 3: Blender Meshes should have normals, but let's make sure and fill a boolean hasNormals

// 	D3DVERTEXELEMENT9 elems[MAX_FVF_DECL_SIZE];
// 	V(meshSys->GetDeclaration(elems));
	
// 	bool hasNormals = false;
// 	D3DVERTEXELEMENT9 term = D3DDECL_END();
// 	for(int i = 0; i < MAX_FVF_DECL_SIZE; ++i)
// 	{
// 		// Did we reach D3DDECL_END() {0xFF,0,D3DDECLTYPE_UNUSED, 0,0,0}?
// 		if(elems[i].Stream == 0xff )
// 			break;

// 		if( elems[i].Type == D3DDECLTYPE_FLOAT3 &&
// 			elems[i].Usage == D3DDECLUSAGE_NORMAL &&
// 			elems[i].UsageIndex == 0 )
// 		{
// 			hasNormals = true;
// 			break;
// 		}
// 	}

// 	// Step 4: Change vertex format to VertexPNT.

// 	D3DVERTEXELEMENT9 elements[64];
// 	UINT numElements = 0;
// 	VertexPNT::Decl->GetDeclaration(elements, &numElements);

// 	ID3DXMesh* temp = 0;
// 	V(meshSys->CloneMesh(D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT, 
// 		elements, gd3dDevice, &temp));
// 	ReleaseCOM(meshSys);
// 	meshSys = temp;

// 	// Step 5: Generate normals if necessary

// 	if( hasNormals == false)
// 		V(D3DXComputeNormals(meshSys, 0));

// 	// Step 6: Optimize the mesh.

// 	DWORD* adj = new DWORD[meshSys->GetNumFaces()*3];
// 	V(meshSys->GenerateAdjacency(EPSILON, adj));
// 	V(meshSys->Optimize(D3DXMESH_MANAGED | 
// 		D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, 
// 		adj, 0, 0, 0, meshOut));
// 	ReleaseCOM(meshSys); // Done w/ system mesh.
// 	delete[] adj;

// 	// Step 7: fake materials and textures:

// 	// currently there is no attempt to get them from blender file,
// 	// just a fixed Material and Texture is created here

// 	Mtrl m;
// 	m.ambient   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
// 	m.diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
// 	m.spec      = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
// 	m.specPower = 1.0f;
// 	mtrls.push_back( m );
// 	IDirect3DTexture9* tex = 0;
// 	V(D3DXCreateTextureFromFile(gd3dDevice, L"res\\dirt6.dds", &tex));
// 	texs.push_back(tex);

// }
