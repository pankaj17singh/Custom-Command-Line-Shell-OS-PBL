CXX = g++
CXXFLAGS = -o

myshell: main.cpp parser.cpp builtins.cpp executor.cpp
	$(CXX) $(CXXFLAGS) -o myshell main.cpp parser.cpp builtins.cpp executor.cpp

clean:
	rm -f myshell
