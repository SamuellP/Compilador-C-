CC=gcc
CXX=g++
DEBUG=-ggdb
CFLAGS= -c
LDFLAGS=
CXXFLAGS=-std=gnu++11 -c
OPT=-O0

all:	compile

compile:
	flex++ -d -oLexical.cpp Lexical.l
	bison++ -t -oParser.cpp -dv Parser.y	
	g++ -c Util.cpp
	g++ -c Tree.cpp
	g++ -c Lexical.cpp
	g++ -c Parser.cpp
	g++ -c Semantic.cpp
	g++ -c Hash.cpp
	g++ -c Simbolo.cpp
	g++ -g CodeGenerator.cpp `/home/samuel/libraries/llvm-3.2/build/Release+Asserts/bin/llvm-config --cxxflags --ldflags --libs core` -o Main Util.o Tree.o Lexical.o Parser.o Semantic.o Hash.o Simbolo.o

clean:
	rm -f *.o
	rm -f Parser.cpp Lexical.cpp
	rm -f Parser.h Parser.output
	rm -f out.txt
	rm -f Main
	
