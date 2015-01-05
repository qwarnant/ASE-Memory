# $Id: Makefile,v 1.2 2009/11/17 10:02:37 marquet Exp $
##############################################################################

ROOTDIR=/home/antoine/workspace/

CC	=  gcc 
CFLAGS	=  -m32 -Wall -ansi -pedantic
CFLAGS  += -std=iso9899:1999
CFLAGS  += -g -w
LIBDIR  =  $(ROOTDIR)/lib
INCDIR  =  $(ROOTDIR)/include
CFLAGS  += -I$(INCDIR)
LIBS    = -lhardware

###------------------------------
### Main targets 
###------------------------------------------------------------
BINARIES= mmu_manager oracle
OBJECTS	= $(addsuffix .o,\
	  mmu swap)

all: $(BINARIES) $(OBJECTS)


###------------------------------
### Binaries
###------------------------------------------------------------
mmu_manager: mmu_manager.o user_process.o matrix.o $(OBJECTS)
oracle: oracle.o matrix.o $(OBJECTS)

% : %.o
	$(CC) -m32 -o $@ $^ -L$(LIBDIR) $(LIBS)

###------------------------------
### #include dependences 
###------------------------------------------------------------
# you may fill these lines with "make depend"
swap.o: swap.c swap.h mmu.h hardware.h
mmu.o: mmu.c mmu.h swap.h config.h hardware.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
	$(RM) .swap_file
realclean: clean 
depend : 
	$(CC) $(CFLAGS) -MM $(INCDIR) *.c 
