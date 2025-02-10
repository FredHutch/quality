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

# Numerical Recipes-specific options
#
# The path to the Numerical Recipes source files. The default is the same
# directory as the QUALITY sources. Do not forget to end the path with a '/'

NRSOURCEPATH= ./NumericalRecipes/Recipes/

# The path to the Numerical Recipes header files. The default is to set it
# identical to the source path. If your setup is different, uncomment the
# second line and set it as appropriate. Don't forget the '/' at the end.

NRINCLUDESPATH= ./NumericalRecipes/OtherCprograms/
#NRINCLUDESPATH=

#
# I can't imagine you'd need to change anything past this point.
# Except one item: if your Numerical Recipes filenames are lowercase,
# you'll probably need to reflect that difference in lines 38-42.
#

SRCS= CQApp.cpp CQAnalys.cpp CPCRStat.cpp CFile.cpp
OBJS= $(SRCS:.cpp=.o)

quality: $(OBJS) main.cpp
	$(CC) $(CFLAGS) $(DEFINES) main.cpp \
		$(NRSOURCEPATH)NRUTIL.C \
		$(NRSOURCEPATH)GAMMLN.C \
		$(NRSOURCEPATH)GAMMQ.C \
		$(NRSOURCEPATH)GCF.C \
		$(NRSOURCEPATH)GSER.C \
		$(OBJS) $(LIBS) -I$(NRINCLUDESPATH) -o quality

CQApp.o: CQApp.cpp CQApp.h
	$(CC) $(CFLAGS) $(DEFINES) -c CQApp.cpp -o CQApp.o

CQAnalys.o: CQAnalys.cpp CQAnalys.h
	$(CC) $(CFLAGS) $(DEFINES) -DNRUSED -c CQAnalys.cpp -I$(NRINCLUDESPATH) -o CQAnalys.o

CPCRStat.o: CPCRStat.cpp CPCRStat.h
	$(CC) $(CFLAGS) $(DEFINES) -c CPCRStat.cpp -o CPCRStat.o

CFile.o: CFile.cpp CFile.h
	$(CC) $(CFLAGS) $(DEFINES) -c CFile.cpp -o CFile.o

clean:
	rm -f *.o ;\
	rm -f core

distclean: clean
	rm -f quality
