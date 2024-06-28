#------------------------
# Do not modify this file
#------------------------

all: trader iml

trader: trader.cpp receiver.h
	g++ -std=c++20 trader.cpp -o trader

iml: iml.cpp
	g++ -std=c++20 iml.cpp -o iml

clean:
	rm -rf output.txt trader iml
	rm -rf *~