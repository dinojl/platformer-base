platformer.out: Source.o Levels.o
	g++ Source.o -o platformer.out -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17

Source.o: Source.cpp olcPixelGameEngine.h olcPGEX_Sound.h
	g++ -c Source.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17

Levels.o: Levels.cpp
	g++ -c Levels.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17

clean:
	rm *.o *.out