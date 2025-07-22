
test: src/bigint.cpp src/test.cpp 
	clang++ -std=c++23 -g -o test src/test.cpp src/bigint.cpp
clean:
	rm -r test bigint *.dSYM
