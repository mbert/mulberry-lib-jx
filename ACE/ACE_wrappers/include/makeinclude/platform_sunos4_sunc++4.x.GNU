# platform_sunos4_sunc++4.x.GNU,v 4.9 2002/06/12 23:34:18 kitty Exp

# SunOS 4.x (Solaris 1.x) with SunC++ 4.x

debug = 1

CC              = cc
CXX             = CC
CCFLAGS		+= $(CFLAGS)
DCFLAGS		+= -g
DLD             = $(CXX)
LD		= $(CXX)
LIBS            +=
LDFLAGS		+= $(PIC)
AR              = CC
ARFLAGS         = -xar -o
PIC             = -PIC
RANLIB          = ranlib
SOFLAGS         += -G $(CPPFLAGS)
SOBUILD         = $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.o $<; \
                  $(SOLINK.cc) $(PIC) -o $@ $(LDFLAGS) $(VSHDIR)$*.o

#### Create template repository to avoid compiler warning.
TEMPLATE_REPOSITORY = Templates.DB
