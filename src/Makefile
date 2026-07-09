CXX = g++
CXXFLAGS = -std=c++17 -Wall -I includes
SRC = main.cpp lexer.cpp parser.cpp sementic.cpp codegen.cpp
TARGET = compiler

build:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) && ./$(TARGET) test.sans
	nasm -f macho64 output.asm -o output.o
	ld -o program output.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _main -arch x86_64
	./program

debug:
	$(CXX) $(CXXFLAGS) -g $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) output.asm output.o program