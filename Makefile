CPPS = GLFiles/Environment.cpp GLFiles/Events.cpp 
LIBS =  -ls_blenderFile -ls_GLUtils -lglut -lGLEW -lGL -lGLU
HS = GLFiles/GLBlenderWorld.h GLFiles/Environment.h

all: bin/stats

bin/stats: main.cpp blender/libs/libs_blenderFile.a blender/libs/libs_GLUtils.a $(CPPS) $(HS)
	rm -f bin/stats
	g++ -o bin/stats main.cpp $(CPPS) -L./blender/libs/ $(LIBS) -I./blender/
