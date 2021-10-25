platformer.out: Source.o
	g++ Source.o -o platformer.out -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17

Source.o: Source.cpp
	g++ -c Source.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17

clean:
	rm *.o *.out