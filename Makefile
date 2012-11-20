all: main.o ConfigFile.o
	g++ obj/make/*.o -lpthread -o bin/make/Aerodrone_rpi

main.o: main.cpp
	g++ -c main.cpp -o obj/make/main.o

ConfigFile.o: ConfigFile.cpp
	g++ -c ConfigFile.cpp -o obj/make/ConfigFile.o