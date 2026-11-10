all:
	g++ mainCaralho.cpp caralho.cpp -o rode -lfreeglut -lopengl32 -lglu32
	./rode
