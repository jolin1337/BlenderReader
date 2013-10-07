#ifndef D3DBLENDER_H
#define D3DBLENDER_H

// Enable d3d debug info in debug builds
#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

// #include <d3d9.h>
// #include <d3dx9.h>
#include <string>
#include <sstream>
#include <vector>
#include "BlenderGlobals.h"

// savely release COM object

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

// void LoadBlenderFile(
// 	const std::string& filename, 
// 	ID3DXMesh** meshOut, 
// 	std::vector<Mtrl>& mtrls, 
// 	std::vector<IDirect3DTexture9*>& texs,
// 	UVMapping uvMapping = UV_SimpleMode);

#endif // D3DBLENDER_H