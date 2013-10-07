#ifndef BLENDERDEFS
#define BLENDERDEFS
#include "BlenderGlobals.h"
#include <fstream>
#include <vector>
#include <string>
#include "DNA.h"
#include "Scene.h"

namespace Blender {
	/**
	 * BlenderDesc -- This function contains all the data of a BlenderFile header
	 */
	class BlenderDesc {
	public:
		int pointersize;	// 4 or 8 (byte)
		bool littleEndian;  // false means big endian
		char version[4];	// cstring version info
	};


	// A number of collection subjects
	enum DCollection { MESH };

	/**
	 * BlenderCallback -- This function works as the comunication 
	 * 					  with the caller of 
	 * 					  Blender::parseBlenderFile(...)
	 */
	class BlenderCallback{
	public:
		/**
		 * This function is called when a mesh has been loaded from a .blend file
		 * @param mesh  - The mesh that has been loaded
		 */
		virtual void objectLoaded(Blender::Object *mesh) = 0;
		/**
		 * This function is called when the Blender class needs to know what data to collect.
		 * @return Returns the data that will be collected
		 */
		virtual DCollection CollectionOfData() = 0;
	};

	/**
	 *  Blender -- This Class parse a .blend file and calls a callback function when it is done.
	 */
	class BlenderFile{
		Scene scene;
	public:
		/**
		 * Initialize a blender-file object with a callback object
		 */
		BlenderFile(BlenderCallback *bc) {
			blenderCallback = bc;
		}

		/**
		 * Pare the blender file filename and extract info
		 * depending on CollectionOfData in the BlenderCallback 
		 * object
		 */
		bool parseBlenderFile(std::string filename, BlenderGlobals::UVMapping uvMapping = BlenderGlobals::UV_SimpleMode);
		BlenderDesc blenderDesc;  	// Blender header description
	private:
		// Parse Functions:
		bool readBlenderHeader(std::ifstream &blenderFile);
		void parseFileBlocks(StructureDNA *sdna, std::vector<FileBlockHeader> &blocks, BlenderGlobals::UVMapping uvMapping);

		BlenderCallback *blenderCallback;		/// Blender callback for variuos of events in the parse process
	};
}
#endif
