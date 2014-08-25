
#include "DNA.h"
#include <algorithm>
#include <list>
#include <sstream>
#include <string.h>
using namespace std;

std::string Blender::getTypeError(int i) {
	std::string type = "Uknown";
	switch(i) {
		case 0: type = "Int";
		break;
		case 1: type = "Short";
		break;
		case 2: type = "Float";
		break;
	}
	return type;
}
void Blender::FileBlockHeader::release() {
	if (this->buffer)			 // if buffer not empty
		delete [] this->buffer; // delete allocated memory
	this->buffer = 0;			 // set buffer to not used
}

void Blender::FileBlockHeader::readFileBlock(ifstream *in) {
	in->read(code, 4);						// read the Block type-code
	code[4] = 0; 							// terminate string with \0 char
	in->read((char*)&size, 4);				// read size of data
	in->read((char*)&oldMemoryAddress, 4);	// read old memory address(used to find structures)
	in->read((char*)&sdnaIndex, 4);			// read this fileblocks structure-dnaIndex
	in->read((char*)&count, 4);				// number of structures in this fileblock
	buffer = new unsigned char[size];			// allocate buffer size
	in->read((char*)buffer, size);				// read the buffer for this fileblock
}

int Blender::FileBlockHeader::getInt(const char *name, StructureDNA *sdna) {
	int offset = sdna->getMemberOffset(name, *this);	// get the offset to name-member in this fileblock
	if (offset == -1) throw 0;						// if name not found throw error 0
	int *iptr = (int*)&buffer[offset];					// get the int from buffer
	return *iptr;
}
int Blender::FileBlockHeader::getInt(unsigned int offset, unsigned int iteration, unsigned int structLength) {
	offset = offset + iteration * structLength;
	if(offset >= size)
		throw 0;
	int *iptr = (int*)(buffer+offset);
	return *iptr;
}

short Blender::FileBlockHeader::getShort(const char *name, StructureDNA *sdna) {
	int offset = sdna->getMemberOffset(name, *this);
	if(offset == -1) throw 1;
	short *sptr = (short *)&buffer[offset];
	return *sptr;
}
int Blender::FileBlockHeader::getShort(int indexOffset) {
	int one = buffer[indexOffset];
	int two = buffer[indexOffset+1];
	return one + 256 * two;
}
short Blender::FileBlockHeader::getShort(unsigned int offset, unsigned int iteration, unsigned int structLength) {
	offset = offset + iteration * structLength;
	if(offset >= size)
		throw 1;
	short *sptr = (short*)&buffer[offset];
	return *sptr;
}

float Blender::FileBlockHeader::getFloat(unsigned int offset, unsigned int iteration, unsigned int structLength) {
	offset = offset + iteration * structLength;
	if(offset >= size)
		throw 2;
	float *fptr = (float*)&buffer[offset];
	return *fptr;
}
float Blender::FileBlockHeader::getFloat(const char *name, StructureDNA *sdna) {
	int offset = sdna->getMemberOffset(name, *this);	// get the offset to name-member in this fileblock
	if (offset == -1) throw 2;						// if name not found throw error 0
	float *fptr = (float*)&buffer[offset];					// get the int from buffer
	return *fptr;
}

const char *Blender::FileBlockHeader::getString(unsigned int offset, unsigned int iteration, unsigned int structLength) {
	offset = offset + iteration * structLength;
	if(offset >= size)
		return "n/a";
	const char *cptr = (const char*)&buffer[offset + iteration * structLength];
	return cptr;
}
const char *Blender::FileBlockHeader::getString(const char *name, StructureDNA *sdna) {
	int offset = sdna->getMemberOffset(name, *this);
	if (offset == -1) return "n/a";
	const char *cptr = (const char*)&buffer[offset];
	return cptr;
}

const char *Blender::FileBlockHeader::getStructureName(StructureDNA *sdna) {
	return sdna->getType(sdna->getStructure(sdnaIndex)->typeIndex).c_str();
}

unsigned int Blender::FileBlockHeader::getSize () const {
	return size;
}