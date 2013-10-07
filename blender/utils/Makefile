CPPS = GLobject.cpp Data.cpp Matrix4.cpp config.cpp 
LIBS = -lglut -lGLEW -lGL -lGLU
name = GLUtils
all: makeStaticLib

makeSharedLib: 
	g++ -Wall -shared -fPIC -o ../libs/lib$(name).so $(CPPS)
makeStaticLib:
	g++ -Wall -c $(CPPS)
	ar rcs ../libs/libs_$(name).a $(CPPS:.cpp=.o)
	rm -f *.o