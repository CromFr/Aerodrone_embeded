#Configure this
CC := g++ 

CCOPTIONS := -std=c++11 -Wall -fexceptions -static -g -DTRG_DEBUG

DIRBIN = ./bin/make
DIROBJ = ./obj/make

DIRINC := ../api/include/x86_64
DIRLIB := ../api/lib
LINKLIB := 
TMPDIR := ./obj/make
#



CCFILE = $(CC) $(CCOPTIONS) -I $(DIRINC) -c


all: ConfigFile Device MotorHdl NetCtrl SensorHdl StabCtrl main
	$(CC) -L$(DIRLIB) -o $(DIRBIN)/Aerodrone_srv `ls $(TMPDIR)/*.o` $(LIBS) -lwiringPi_dis
	
	
	
install_libraries: $@
	#Install WiringPi
	mkdir -p ./libraries/src
	mkdir ./libraries/include
	mkdir ./libraries/lib
	cd libraries/src
	git clone https://github.com/WiringPi/WiringPi.git
	cd WiringPi/wiringPi
	make -j4
	mv *.a ../../../lib
	mv *.h ../../../include
	



	
ConfigFile:
	$(CCFILE) ConfigFile.cpp -o $(DIROBJ)/ConfigFile.o
	
Device:
	$(CCFILE) Device.cpp -o $(DIROBJ)/Device.o
	
MotorHdl:
	$(CCFILE) MotorHdl.cpp -o $(DIROBJ)/MotorHdl.o
	
NetCtrl:
	$(CCFILE) NetCtrl.cpp -o $(DIROBJ)/NetCtrl.o
	
SensorHdl:
	$(CCFILE) SensorHdl.cpp -o $(DIROBJ)/SensorHdl.o
	
StabCtrl:
	$(CCFILE) StabCtrl.cpp -o $(DIROBJ)/StabCtrl.o
	
main:
	$(CCFILE) main.cpp -o $(DIROBJ)/main.o
	
	
clean: $@
	rm obj/make/*.o
	
	
	
	
