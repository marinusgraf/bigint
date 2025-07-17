all:
	clang++ -std=c++23 -g -o test test.cpp bigint.cpp
clean:
	rm -r test bigint *.dSYM
