DIR = H:\\SFML-2.5.1
CFLAGS = -I H:\\SFML-2.5.1\include
LFLAGS = -L H:\\SFML-2.5.1\lib
LIBS = -lsfml-window  -lsfml-graphics -lsfml-system -lsfml-network
robot.exe: robot.o
	g++ $(LFLAGS) -o robot robot.o $(LIBS)
robot.o: robot.cpp
	g++  -c $(CFLAGS) robot.cpp