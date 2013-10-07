#include "DNA.h"
#include <string.h>
#include <sstream>

using namespace std;

int Blender::StructureDNA::alterLengthByName(string name, int struct_length) {
	//if (name.compare(0, 1, "*")) return 4;
	// check for pointer or function pointer
	if (name.at(0) == '*' || name.at(0) == '(') 
		return 4;
	// look for arrays:
	//size_t posX = name.find("][");
	//if (posX != string::npos) {
	//	Log("dgzu");
	//}
	size_t pos = name.find("[");
	int arrayMult = 1;
	while (pos != string::npos) {
		// we may find multiple array dimensions: multiply all of them
		size_t end = name.find("]", pos);
		string arrayLength = name.substr(pos+1, end-pos-1);
		stringstream ss(arrayLength.c_str());
		int num;
	    if((ss >> num).fail()) { 
			BlenderGlobals::Error("Error in number conversion");
			return 0;
	    }
		arrayMult *= num;
		pos = name.find("[", end);
	}
	return arrayMult * struct_length;
}

bool Blender::StructureDNA::parse(FileBlockHeader *fbheader) {
	names.clear();
	types.clear();
	lengths.clear();
	structures.clear();
	unsigned char *buffer = fbheader->buffer;
	size_t i = 0;
	if (0 != strncmp("SDNA", (char*)&buffer[i], 4)) {
		BlenderGlobals::Error("parse error in structure DNA");
		return false;
	}
	i += 4;
	if (0 != strncmp("NAME", (char*)&buffer[i], 4)) {
		BlenderGlobals::Error("parse error in structure DNA");
		return false;
	}
	i += 4;
	// get names
	unsigned int numNames = *((unsigned int*)&buffer[i]);
	i += 4;
	for (unsigned int k = 0; k < numNames; k++) {
		names.push_back(string((char*)&buffer[i]));
		i += (names[k].size() + (size_t)1);
	}

	// get types:
	while ((i%4) != 0) i++;	// align to 4 byte boundary
	if (0 != strncmp("TYPE", (char*)&buffer[i], 4)) {
		BlenderGlobals::Error("parse error in structure DNA");
		return false;
	}
	i += 4;
	unsigned int numTypes = *((unsigned int*)&buffer[i]);
	i += 4;
	for (unsigned int k = 0; k < numTypes; k++) {
		types.push_back(string((char*)&buffer[i]));
		i += (types[k].size() + (size_t)1);
	}

	// get lengths:
	while ((i%4) != 0) i++;	// align to 4 byte boundary
	if (0 != strncmp("TLEN", (char*)&buffer[i], 4)) {
		BlenderGlobals::Error("parse error in structure DNA");
		return false;
	}
	i += 4;
	for (unsigned int k = 0; k < numTypes; k++) {
		int len = fbheader->getShort(i);
		i += 2;
		lengths.push_back(len);
	}

	// get structures:
	while ((i%4) != 0) i++;	// align to 4 byte boundary
	if (0 != strncmp("STRC", (char*)&buffer[i], 4)) {
		BlenderGlobals::Error("parse error in structure DNA");
		return false;
	}
	i += 4;
	unsigned int numStructures = *((unsigned int*)&buffer[i]);
	i += 4;
	for (unsigned int k = 0; k < numStructures; k++) {
		StructureInfo sinfo;
		StructureField sfield;
		size_t typeIndex = fbheader->getShort(i);
		i += 2;
		sinfo.typeIndex = typeIndex;
		size_t numFields = fbheader->getShort(i);
		i += 2;
		int offset = 0; // track field offset
		for (unsigned int l = 0; l < numFields; l++) {
			// parse fields of this structure:
			size_t fieldTypeIndex = fbheader->getShort(i);
			i += 2;
			size_t fieldNameIndex = fbheader->getShort(i);
			i += 2;
			sfield.typeIndex = fieldTypeIndex;
			sfield.nameIndex = fieldNameIndex;
			sfield.offset = offset;
			offset += alterLengthByName(names[fieldNameIndex], lengths[fieldTypeIndex]);
			sinfo.fields.push_back(sfield);
		}
		structures.push_back(sinfo);
	}

	if (BlenderGlobals::debug_basic) {
		ostringstream oss;
		oss << " names collected: " << names.size() << "\n";
		oss << " types collected: " << types.size() << "\n";
		oss << " lengths collected: " << lengths.size() << "\n";
		oss << " structures collected: " << structures.size() << "\n";
		BlenderGlobals::Log(oss.str());

// #define DUMPNAMES

#ifdef DUMPNAMES
		for (unsigned int k = 0; k < names.size(); k++) {
			oss.clear();
			oss.str("");
			oss << " name[" << k << "]: " << names[k] << "\n";
			Blender::Log(oss.str());
		}
#endif
	}
	return true;
}
Blender::StructureInfo *Blender::StructureDNA::findStructureType(const char *name) {
	for (unsigned int i = 0; i < structures.size(); i++) {
		if (0 == strcmp(name, types[structures[i].typeIndex].c_str())) {
			return &structures[i];
		}
	}
	return 0;
}

Blender::StructureInfo *Blender::StructureDNA::findStructureType(size_t typeIndex) {
	//char *typeName = types[typeIndex].c_str();
	for (unsigned int i = 0; i < structures.size(); i++) {
		//if (0 == strcmp(typeName, structures[i].)) {
		if (typeIndex == structures[i].typeIndex) {
			return &structures[i];
		}
	}
	return 0;
}
Blender::StructureInfo *Blender::StructureDNA::getStructure(size_t sdnaIndex){
	if(sdnaIndex > structures.size()) return 0;
	return &structures[sdnaIndex];
}


void Blender::StructureDNA::addFileBlock(FileBlockHeader *fbh) {
	blocksByName[types[fbh->spStruct->typeIndex]].push_back(fbh);
	blocksByAddress[fbh->oldMemoryAddress] = fbh;
}
Blender::FileBlockHeader *Blender::StructureDNA::getFileBlockByAddress(int address){
	return blocksByAddress[address];
}
std::vector<Blender::FileBlockHeader *> Blender::StructureDNA::getFileBlocksByName(std::string name){
	return blocksByName[name];
}
Blender::FileBlockHeader * Blender::StructureDNA::getFileBlock(std::string namePath, FileBlockHeader *fbh) {
	if(fbh == 0) return 0;
	try{
		string tmpName = "";
		int address = 0;
		while(!namePath.empty()){
			address = fbh->getInt(namePath.c_str(), this);
			if(address != 0){
				if(tmpName.empty())
					return getFileBlockByAddress(address);
				fbh = getFileBlockByAddress(address);
				return getFileBlock(tmpName.substr(0, tmpName.size()-1), fbh);
			}
			int lastDotPos = namePath.find_last_of(".");
			tmpName += namePath.substr(lastDotPos+1) + ".";
			if(namePath == namePath.substr(0, lastDotPos))
				return 0;
			namePath = namePath.substr(0, lastDotPos);
		}
		return 0;

		unsigned int dotPos = namePath.find_last_of(".");
		while(dotPos != string::npos){
			string name = namePath.substr(0, dotPos);
			address = fbh->getInt(name.c_str(), this);

			if(address == 0){
				dotPos = namePath.find('.', dotPos+1);
				name = namePath.substr(0, dotPos);
				address = fbh->getInt(name.c_str(), this);
				BlenderGlobals::Log(name);
				if(name == namePath && address != 0){
					return getFileBlockByAddress(address);
				}
			}
			if(address == 0) return 0;//fbh;
			
			fbh = getFileBlockByAddress(address);

			namePath = namePath.substr(dotPos+1);
			dotPos = namePath.find('.');
		}
		
			if(address == 0) BlenderGlobals::Log("hjhkd");
		address = fbh->getInt(namePath.c_str(), this);
		if(address == 0)
			return 0;
		fbh = getFileBlockByAddress(address);
		return fbh;
	} catch(int &e){
		return 0;
	}
}

void Blender::StructureDNA::getOffsets(int *offset, string name, FileBlockHeader &fbh) {
	// get offsets for sub structures and a fields
	StructureInfo s = structures[fbh.sdnaIndex];
	unsigned int dotPos = name.find('.');
	for (unsigned int i = 0; i < s.fields.size(); i++) {
		string tocompare = names[s.fields[i].nameIndex];
		if (name.substr(0, dotPos) == tocompare || name.substr(0, dotPos) == tocompare.substr(0, tocompare.find('['))) {
			name = name.substr(dotPos+1);
			dotPos = name.find('.');
			StructureField field = s.fields[i];
			*offset = field.offset;
			StructureInfo *subStructure = findStructureType(field.typeIndex);
			while(dotPos != string::npos || *offset == field.offset){
				for (unsigned int k = 0; k < subStructure->fields.size(); k++) {
					// Blender::Log(names[subStructure->fields[k].nameIndex] + "\n");
					tocompare = names[subStructure->fields[k].nameIndex];
					if (name.substr(0, dotPos-1) == tocompare || name.substr(0, dotPos-1) == tocompare.substr(0, tocompare.find('['))) {
						field = subStructure->fields[k];
						*offset += field.offset;

						name = name.substr(dotPos+1);
						dotPos = name.find('.');
						if(dotPos == string::npos)
							return;
						if(isSimpleType(&field)){
							*offset = -1;
							return;
						}
						subStructure = findStructureType(field.typeIndex);
						break;
					}
				}
				if(*offset == field.offset){
					*offset = -1;
					return;
				}

			}
			return;
		}
	}
	return;
}


int Blender::StructureDNA::getMemberOffset(const char *name, FileBlockHeader &fbh) {
	string n(name);
	size_t pos = n.find('.');
	if (pos != string::npos) {
		int offset = 0;
		getOffsets(&offset, name, fbh);
		return offset;
	}

	StructureInfo s = structures[fbh.sdnaIndex];
	for (unsigned int i = 0; i < s.fields.size(); i++) {
		string tocompare = names[s.fields[i].nameIndex];
		if (strcmp(name, tocompare.c_str()) == 0 || strcmp(name, tocompare.substr(0, tocompare.find('[')).c_str()) == 0) {
			return s.fields[i].offset;
		}
	}
	return -1;
}

int Blender::StructureDNA::getLength(size_t typeIndex) {
	return lengths[typeIndex];
}
std::string Blender::StructureDNA::getType(size_t typeIndex) {
	return types[typeIndex];
}
std::string Blender::StructureDNA::getName(size_t typeIndex) {
	return names[typeIndex];
}

bool Blender::StructureDNA::isPointer(size_t nameIndex) {
	return names[nameIndex][0] == '*';
}
bool Blender::StructureDNA::isArray(size_t nameIndex){
	return (names[nameIndex].find('[') != string::npos && names[nameIndex].find(']') != string::npos);
}
int Blender::StructureDNA::arraySize(size_t nameIndex) {
	string name = names[nameIndex];
	unsigned int start = name.find('[');
	unsigned int end = name.find(']');
	stringstream ss;
	ss << name.substr(start+1, end-start);
	int size, size2;
	ss >> size;
	while(start != string::npos){
		start = name.find('[', start+1);
		end = name.find(']', end+1);
		if(start == string::npos || end == string::npos)
			break;
		ss.clear();
		ss.str("");
		ss << name.substr(start+1, end-start-1);
		ss >> size2;
		size *= size2;
	}
	return size;
}
bool Blender::StructureDNA::isCType(size_t typeIndex){
	return (
		types[typeIndex] == "void" ||
		types[typeIndex] == "char" ||
		types[typeIndex] == "short" ||
		types[typeIndex] == "int" || 
		types[typeIndex] == "float" ||
		types[typeIndex] == "double"
	);
}
bool Blender::StructureDNA::isSimpleType(StructureField *field) {
	if(isPointer(field->nameIndex)) return true;
	return isCType(field->typeIndex);
}



void Blender::printFields(FileBlockHeader *fbh, StructureDNA *sdna, size_t index, std::string name) {
	if(fbh == 0 || sdna == 0 || index > fbh->count-1) return;
	std::stringstream ss;
	ss << "struct ";
	if(name == "")
		ss << fbh->getStructureName(sdna) << " {\n";
	else
		ss << name << " {\n";

	size_t len = sdna->getLength(fbh->spStruct->typeIndex);
	for (unsigned int i = 0; i < fbh->spStruct->fields.size(); i++) {
		int nameIndex = fbh->spStruct->fields[i].nameIndex;
		int typeIndex = fbh->spStruct->fields[i].typeIndex;
		std::string name = sdna->getName(nameIndex),
					type = sdna->getType(typeIndex);
		int offset_field = sdna->getMemberOffset(name.c_str(), *fbh);

		ss << "       " << type << " " << name << " = " ;
		if(type == "char"){
			if(sdna->isArray(nameIndex))
				ss << fbh->getString(offset_field, index, len);
			else if(sdna->isPointer(nameIndex)){
				int addr = fbh->getInt(offset_field, index, len);
				fbh = sdna->getFileBlockByAddress(addr);
				if(fbh)
					ss << fbh->getString(0,0,0);
			}
			else
				ss << (int)*fbh->getString(offset_field, index, len);
		}
		else if(type == "short")
			ss << fbh->getShort(offset_field, index, len);
		else if(type == "int")
				ss << fbh->getInt(offset_field, index, len);
		else if(type == "float"){
			if(sdna->isArray(nameIndex)){
				int size = sdna->arraySize(nameIndex);
				ss << "{";
    			streamsize ssize = ss.precision();
				ss.precision(3);
				for (int j = 0; j < size; j++) {
					ss << fbh->getFloat(offset_field, index+j+6, len);
					if(j +1 != size)
						ss << ", ";
				}
				ss.precision(ssize);
				ss << "}";
			}
			else
				ss << fbh->getFloat(offset_field, index, len);
		}
		// else if(type == "double")
		// 	ss << fbh->getDouble(name.c_str(), sdna);
		else
			ss << sdna->getFileBlock(name.c_str(), fbh);
		ss << ";" << "\n";
	}
	ss << "}*" << fbh->count << ";";
	BlenderGlobals::Log(ss.str());
}
