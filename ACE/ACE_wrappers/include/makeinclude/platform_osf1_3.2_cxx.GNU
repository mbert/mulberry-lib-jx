# platform_osf1_3.2_cxx.GNU,v 4.4 2005/07/29 07:16:00 jwillemsen Exp

# This platform macros file is intended to work with Digital UNIX 3.2
# (OSF/1 3.2g) with CXX 5.7 or later.
debug ?= 1

# exceptions=1 is the default on this platform. exceptions=0 has not
# been tested. See the bottom of this file for more. Turning off exceptions
# on this platform can cause bad things to happen.
exceptions ?= 1

# We want to test with rtti=1
rtti ?= 1
threads ?= 1

CC              = cxx
CXX             = $(CC)
CFLAGS          += -threads
#### CCFLAGS         += -x cxx
CCFLAGS         += $(CFLAGS) -w0
DCFLAGS         += -g -O0
#### DLD             = ld -shared -expect_unresolved '*'
DLD             = ld
LD              = $(CXX)
#### LIBS            += -lpthreads -lmach -lsys5 -lxti -ltli -lcxx -lexc -lc -lrt
LIBS            += -qltli_r -ltli -qlrt_r -lrt -qlcxxstd_r -lcxxstd -qlcxx_r \
                   -lcxx -qlots_r -lots -lpthreads -lmach -lexc -lc_r -lc
#### PIC             = -pic
ARFLAGS         = cruv
RANLIB          = /usr/bin/true
#### SOFLAGS         = -shared
SOFLAGS         += -L/usr/lib/cmplrs/cxx -rpath /usr/lib/cmplrs/cxx \
                  -L$(ACE_ROOT)/lib -L./ -g2 -O0 -shared
SOBUILD         = $(RM) $@; $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.o $< && \
                  ln $(VSHDIR)$*.o $@
