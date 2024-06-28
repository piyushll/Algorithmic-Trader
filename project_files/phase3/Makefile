all: iml.cpp trader.cpp
	g++ -std=c++20 iml.cpp -lpthread -o iml
	g++ -std=c++20 trader.cpp -lpthread -o trader

clean:
	rm -rf trader iml output.txt
	rm -rf *~
