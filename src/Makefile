CXX = g++
CXXFLAGS = -std=c++17 -Wall -I includes
SRC = main.cpp lexer.cpp parser.cpp
TARGET = compiler

build:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) && ./$(TARGET) test.sans

debug:
	$(CXX) $(CXXFLAGS) -g $(SRC) -o $(TARGET)

run: build
	./$(TARGET) $(FILE)

clean:
	rm -f $(TARGET)
