CPPS =  Blender.cpp BlenderGlobals.cpp Mesh.cpp FileBlockHeader.cpp StructureDNA.cpp Object.cpp Armature.cpp
HS =    Blender.h   BlenderGlobals.h   Mesh.h   DNA.h Data3D.h Scene.h AnimData.h    Object.h   Armature.h

all: stats staticLib sharedLib

stats: bin/stats
staticLib: libs/libs_blenderFile.a
sharedLib: libs/libblenderFile.so

bin/stats: main.cpp $(CPPS) $(HS)
	make cleanRun
	g++ -o bin/stats main.cpp  $(CPPS)

libs/libblenderFile.so: $(CPPS) $(HS)
	g++ -Wall -shared -fPIC -o libs/libblenderFile.so $(CPPS)

libs/libs_blenderFile.a: $(CPPS) $(HS)
	g++ -Wall -c $(CPPS)
	ar rcs libs/libs_blenderFile.a $(CPPS:.cpp=.o)
	rm -f *.o

cleanRun:
	rm -f stats
	rm -f *.o
