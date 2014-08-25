CPPS = GLFiles/GLMesh.cpp GLFiles/GLArmature.cpp GLFiles/GLBlenderWorld.cpp GLFiles/Environment.cpp GLFiles/Events.cpp 
LIBS =  -ls_blenderFile -ls_GLUtils -lglut -lGLEW -lGL -lGLU
HS = GLFiles/GLMesh.h GLFiles/GLArmature.h GLFiles/GLBlenderWorld.h GLFiles/Environment.h

all: 
	./make.sh

blenderExe: bin/stats

bin/stats: main.cpp blender/libs/libs_blenderFile.a blender/libs/libs_GLUtils.a $(CPPS) $(HS)
	rm -f bin/stats
	g++ -o bin/stats main.cpp $(CPPS) -L./blender/libs/ $(LIBS) -I./blender/
