CXX_FLAGS=-std=c++20 -g -Wall -Werror -Wpedantic -o
CXX=clang++

main: src/main.cpp src/bigint.cpp
	$(CXX) $(CXX_FLAGS) $@ $^ 

clean:
	rm -r test bigint *.dSYM main
