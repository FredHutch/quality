# The name of your C++ compiler

CC= g++

# Any necessary/desired compiler flags. My default of -O is to optimize
# code generation, but you may wish to disable that by commenting the line
# out and using the line following to give CFLAGS an empty value.

CFLAGS= -O
#CFLAGS=

# If you're trying to compile this on a DOS machine, you'll probably
# want to use this define to set the command-line switch char to '/'
# Otherwise, it defaults to '-' (the Unix char)

DEFINES=
#DEFINES= -DCOMPILE_DOS  #uncomment to use DOS cmd-line '/' switch char

# Add any libraries that need to be linked in on your system. The default of
# -lm is for the math library, so we can call functions like square root.

LIBS= -lm

#
# I can't imagine you'd need to change anything past this point.
#

SRCS= CQApp.cpp CQAnalys.cpp CPCRStat.cpp CFile.cpp
OBJS= $(SRCS:.cpp=.o)

quality: $(OBJS) main.cpp
	$(CC) $(CFLAGS) $(DEFINES) main.cpp $(OBJS) $(LIBS) -o quality

CQApp.o: CQApp.cpp CQApp.h
	$(CC) $(CFLAGS) $(DEFINES) -c CQApp.cpp -o CQApp.o

CQAnalys.o: CQAnalys.cpp CQAnalys.h
	$(CC) $(CFLAGS) $(DEFINES) -c CQAnalys.cpp -o CQAnalys.o

CPCRStat.o: CPCRStat.cpp CPCRStat.h
	$(CC) $(CFLAGS) $(DEFINES) -c CPCRStat.cpp -o CPCRStat.o

CFile.o: CFile.cpp CFile.h
	$(CC) $(CFLAGS) $(DEFINES) -c CFile.cpp -o CFile.o

clean:
	rm -f *.o ;\
	rm -f core

distclean: clean
	rm -f quality
