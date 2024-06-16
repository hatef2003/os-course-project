main.out: main.o named_pipe.o building.out bill.out raw_data
	g++ main.o named_pipe.o -o main.out
main.o : main.cpp common.cpp
	g++ -c main.cpp -o main.o
building.out: building.o named_pipe.o
	g++ building.o named_pipe.o -o building.out
bill.out: named_pipe.o bill.o
	g++ named_pipe.o bill.o -o bill.out
named_pipe.o : named_pipe.cpp named_pipe.hpp common.cpp
	g++ -c named_pipe.cpp -o named_pipe.o
bill.o: bill.cpp common.cpp named_pipe.hpp
	g++ -c bill.cpp -o bill.o
building.o:building.cpp common.cpp named_pipe.hpp
	g++ -c building.cpp -o building.o
raw_data: raw_data.cpp common.cpp
	g++ raw_data.cpp -o raw_data

clean:
	rm -r *.out
	rm -r *.o