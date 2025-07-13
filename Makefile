all:
	clang++ -g -o test test.cpp bigint.cpp
clean:
	rm -r test bigint *.dSYM
