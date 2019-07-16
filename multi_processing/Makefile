all : program1 program2 program3 program4

program1 : program1.cpp
	g++ program1.cpp -o program1

program2 : program2.cpp
	g++ program2.cpp -pthread -lrt -o program2

program3 : program3.cpp
	g++ program3.cpp -pthread -lrt -o program3

program4 : program4.cpp
	g++ program4.cpp -o program4

clean :
	rm program1 program2 program3 program4
