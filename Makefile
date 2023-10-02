CPP := g++
SRC :=  src/main.cpp src/engine.cpp
HDR := -Iinclude
LD := -lSDL2 -lSDL2_image

unix:
	$(CPP) $(HDR) $(SRC) $(LD)
	./a.out
	rm -rf include/*.gch
	exit

windows:
	$(CPP) $(HDR) $(SRC) $(LD) -lmingw32 -Llib_windows -static-libgcc -static-libstdc++
	./a.exe