#ifndef ANIM
#define ANIM

#include "DNA.h"
#include "BlenderGlobals.h"
#include "utils/Vector3.h"
#include <vector>

namespace Blender {

	struct BezTriple {
		Vector3 cursor, 		// x = time, y = value, z = ?
				handle1, 		// handle one of cursor
				handle2; 		// handle two of cursor
		int ipo;				// interpolation-mode[constant, linear, beizer]

	};
	struct Curve {
		std::vector<BezTriple > keyframes;	// keyframes for animation
		std::string rna_path;				//type of keys
		int data_index;						// index of a series of curves describing rna_path

		// Driver *driver; 					// TODO: add driver to class
		
		unsigned int getTotVert(){
			return keyframes.size();
		}
	};

	class AnimData {
		std::vector<Curve > curves;
	public:
		AnimData(){}

		int parse(FileBlockHeader *fbobject, StructureDNA *sdna) {
			if(fbobject == 0 || sdna == 0) return 0;

			FileBlockHeader *adt = sdna->getFileBlock("*adt", fbobject);
			if(!adt) return 0;
			adt = sdna->getFileBlock("*action", adt);
			if(!adt) return 0;
			adt = sdna->getFileBlock("groups.*first", adt);
			if(!adt) return 0;
			adt = sdna->getFileBlock("channels.*first", adt);
			if(!adt) return 0;

			while(adt != 0){
				// Load data into Curve:
				Curve cu;
				int address = adt->getInt("*rna_path", sdna);
				cu.rna_path = sdna->getFileBlockByAddress(address)->getString(0, 0, 0);
				cu.data_index = adt->getInt("array_index", sdna);
				
				FileBlockHeader *bezt = sdna->getFileBlock("*bezt", adt);
				unsigned int count = adt->getInt("totvert", sdna),
					len   = sdna->getLength(bezt->spStruct->typeIndex);
				if(bezt && count == bezt->count){
					int offset_c = sdna->getMemberOffset("vec", *bezt);
					for (unsigned int i = 0; i < count; i++) {
						BezTriple be;
						be.handle1.x = bezt->getFloat(offset_c, i, len);
						be.handle1.y = bezt->getFloat(offset_c+4, i, len);
						be.handle1.z = bezt->getFloat(offset_c+8, i, len);

						be.cursor.x = bezt->getFloat(offset_c+12, i, len);
						be.cursor.y = bezt->getFloat(offset_c+16, i, len);
						be.cursor.z = bezt->getFloat(offset_c+20, i, len);

						be.handle2.x = bezt->getFloat(offset_c+24, i, len);
						be.handle2.y = bezt->getFloat(offset_c+28, i, len);
						be.handle2.z = bezt->getFloat(offset_c+32, i, len);
						be.ipo = bezt->getInt("ipo", sdna);
						cu.keyframes.push_back(be);
					}
				}

				curves.push_back(cu);
				adt = sdna->getFileBlock("*next", adt);
			}
			return 1;
		}
		unsigned int getCurveCount(){
			return curves.size();
		}
		Curve getCurve(int index){
			return curves[index];
		}
		int getFrameCount(){
			unsigned int cc = curves.size();
			if(cc == 0) return 0;
			unsigned int kc = curves[0].keyframes.size();
			if(kc == 0) return 0;
			int frame = curves[0].keyframes[0].cursor.x;
			for(unsigned int i = 0; i < cc; i++){
				Curve cu = curves[i];
				kc = cu.keyframes.size();
				for (unsigned int j = 0; j < kc; j++){
					frame = (frame > cu.keyframes[j].cursor.x?frame: cu.keyframes[j].cursor.x);
				}
			}
			return frame;
		}
	};
}

#endif