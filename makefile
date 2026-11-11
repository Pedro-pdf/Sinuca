all:
	g++ main.cpp loader.cpp elementos.cpp -o rode -lfreeglut -lopengl32 -lglu32
	./rode
