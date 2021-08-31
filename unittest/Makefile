#disable some warning because UT need test functions not in header file.
CFLAGS = -Wno-parentheses -Wno-format-security -Wno-implicit-function-declaration -c
IFLAGS = -I.. -I../include -I../lib

all:
	gcc plugin_test.c $(IFLAGS) $(CFLAGS) -o plugin_test.o
	gcc mock_helper.c $(IFLAGS) $(CFLAGS) -o mock_helper.o
	gcc ../plugin.c   $(IFLAGS) $(CFLAGS) -o plugin.o
	gcc  plugin_test.o  mock_helper.o plugin.o -o plugin_test  -lc -lcunit 

clean:
	rm *.o