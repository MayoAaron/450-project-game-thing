CPP := g++
SRC := src/*.cpp
HDR := -Iinclude
LD := -lSDL2 -lSDL2_image

run:
	$(CPP) $(HDR) $(SRC) $(LD)
	./a.out
	rm -rf include/*.gch