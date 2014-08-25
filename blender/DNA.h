#ifndef DNAFBH
#define DNAFBH
#include "BlenderGlobals.h"
#include <fstream>
#include <vector>
#include <map>

namespace Blender {
	class StructureField {
	public:
		size_t typeIndex;
		size_t nameIndex;
		int offset;
	};

	class StructureInfo {
	public:
	  size_t typeIndex;
	  std::vector<StructureField> fields;
	};

	class FileBlockHeader;
	class StructureDNA {
		std::vector<std::string> names;		// names of fields
		std::vector<std::string> types;		// data-/struct-types
		std::vector<int> lengths;			// the length of data
		std::vector<StructureInfo> structures;// structures loaded

		// links between FileBlockHeader and StructureDNA
		std::map<std::string, std::vector<FileBlockHeader *> > blocksByName;
		std::map<int, FileBlockHeader *> blocksByAddress;
	public:
		/**
		 * figures out the length and alters it
		 * @param  name          - the name of the altered part
		 * @param  struct_length - length of the name
		 * @return a offset from names begin
		 */
		int alterLengthByName(std::string name, int struct_length);
		/**
		 * parse the Structures and saves in structures variable
		 * @param  fbheader - The DNA1 headerblock
		 * @return true on success otherwise false
		 */
		bool parse(FileBlockHeader *fbheader);
		/**
		 * Finds a structure by the name/typename
		 * @param  name
		 * @return the structure with name name
		 */
		StructureInfo *findStructureType(const char *name);
		/**
		 * Finds a structure by the typeIndex 
		 * @param  typeIndex
		 * @return the structure with type-index typeIndex
		 */
		StructureInfo *findStructureType(size_t typeIndex);
		StructureInfo *getStructure(size_t sdnaIndex);

		void addFileBlock(FileBlockHeader *fbh);
		FileBlockHeader *getFileBlockByAddress(int address);
		std::vector<FileBlockHeader *> getFileBlocksByName(std::string name);
		FileBlockHeader * getFileBlock(std::string namePath, FileBlockHeader *fbh);

		/**
		 * Sets the offsets value to what offset the member-name 
		 * is in the structure of fbh(which is calculated)
		 * @param offset - the offset calculated
		 * @param name   - the name that the function will 
		 * 					calculate the offset to
		 * @param fbh    - the FileBlock header that tells 
		 * 					the function what structure where to
		 * 					search
		 */
		void getOffsets(int *offset, std::string name, FileBlockHeader &fbh);
		int getMemberOffset(const char *name, FileBlockHeader &fbh);

		int getLength(size_t typeIndex);
		std::string getType(size_t typeIndex);
		std::string getName(size_t typeIndex);

		bool isPointer(size_t nameIndex);
		bool isArray(size_t nameIndex);
		bool isCType(size_t typeIndex);
		bool isSimpleType(StructureField *field);
		int arraySize(size_t nameIndex);
	};

	class Object;
	class FileBlockHeader {
	public:
		char code[5];				// 4
		unsigned int size;			// 4
		int oldMemoryAddress;		// pointersize (4)
		unsigned int sdnaIndex;	// 4
		unsigned int count;		// 4
		unsigned char *buffer;	     	// buffer content;
		StructureInfo *spStruct;	// link to a structure that the data contains

		FileBlockHeader():size(0), buffer(0),spStruct(0) {}
		/**
		 * reads a fileblock from .blend file-type input stream then
		 * saves it to this fileblock buffer
		 * @param in - the file to read from
		 */
		void readFileBlock(std::ifstream *in);

		const char *getString(unsigned int offset, unsigned int iteration, unsigned int structLength);
		/**
		 * rethreaves the string with name in this fileblock from structure dna
		 * @param  name - the member-name
		 * @param  sdna - structure dna data
		 * @return returns the name with \0 as the end-point
		 */
		const char* getString(const char *name, StructureDNA *sdna);
		/**
		 * searches for a int with name in sdna 
		 * @param  name - the name of member-variable
		 * @param  sdna - parsed structure dna 
		 * @return the int found from sdna with name -1 elsewhere
		 */
		int getInt(const char *name, StructureDNA *sdna);
		/**
		 * gets the int type variable with (offset+iteration*structLength) from start of buffer
		 * @param  offset 	    - a number of offset bytes
		 * @param  iteration    - steps to take from offset in buffer
		 * @param  structLength - size of one step
		 * @return the int on the given position from buffer start
		 */
		int getInt(unsigned int offset, unsigned int iteration, unsigned int structLength);

		short getShort(const char *name, StructureDNA *sdna);
		/**
		 * retriew short from buffer 
		 * @param  indexOffset a offset from buffer starts where the short is in
		 * @return a short from buffer
		 */
		int getShort(int indexOffset);
		/**
		 * gets the short type variable with (offset+iteration*structLength) from start of buffer
		 * @param  offset 	    - a number of offset bytes
		 * @param  iteration    - steps to take from offset in buffer
		 * @param  structLength - size of one step
		 * @return the short on the given position from buffer start
		 */
		short getShort(unsigned int offset, unsigned int iteration, unsigned int structLength);

		/**
		 * gets the float type variable with (offset+iteration*structLength) from start of buffer
		 * @param  offset 	    - a number of offset bytes
		 * @param  iteration    - steps to take from offset in buffer
		 * @param  structLength - size of one step
		 * @return the float on the given position from buffer start
		 */
		float getFloat(unsigned int offset, unsigned int iteration, unsigned int structLength);
		float getFloat(const char *name, StructureDNA *sdna);

		/**
		 * Gets the structure type of this FileBlockHeader
		 * @param  sdna - the sdna that contains all the names
		 * @return the name of this structure type
		 */
		const char *getStructureName(StructureDNA *sdna);

		/**
		 * relsease buffer array of this object so that it bekomes unusable
		 */
		void release();
		/**
		 * Returns the size of this fileblockheader allocated when called readFileBlock
		 * otherwise 0
		 */
		unsigned int getSize () const;
	};
	void printFields(FileBlockHeader *fbh, StructureDNA *sdna, size_t index=0, std::string name="");
	std::string getTypeError(int i);
}
#endif