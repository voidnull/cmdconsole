console_test: cmdconsole.cpp console_test.cpp

console_test: cmdconsole.cpp console_test.cpp
	$(CC) -c *.c
	$(CXX) -c *.cpp
	ar rcs libcmdconsole.a linenoise.o cmdconsole.o
	$(CXX) -Wall -o console_test libcmdconsole.a console_test.cpp

clean:
	rm *.o *.a
	rm -f console_test
