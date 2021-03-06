# platform_linux_icc.GNU,v 1.16 2005/05/16 14:15:30 jwillemsen Exp

# According to Roger Tragin <r.tragin@computer.org>, this file
# should allow ACE to be built on Linux, using the Intel compiler.
#
# This is just a copy of platform_linux.GNU, with a few things changed.

exceptions ?= 1
debug ?= 1
optimize ?= 0
threads ?= 1

ifeq ($(exceptions),1)
  CPPFLAGS += -Kc++eh
endif # exceptions

PLATFORM_XT_CPPFLAGS=
PLATFORM_XT_LIBS=-lXt
LATFORM_XT_LDFLAGS=

PLATFORM_FL_CPPFLAGS=
PLATFORM_FL_LIBS=-lfltk
PLATFORM_FL_LDFLAGS=

PLATFORM_X11_CPPFLAGS=-I/usr/X11R6/include
PLATFORM_X11_LIBS=-lX11
PLATFORM_X11_LDFLAGS=-L/usr/X11R6/lib

PLATFORM_GL_CPPFLAGS=-I/usr/X11R6/include
PLATFORM_GL_LIBS    =-lGL
PLATFORM_GL_LDFLAGS =-L/usr/X11R6/lib

PLATFORM_GTK_CPPFLAGS=$(shell gtk-config --cflags)
PLATFORM_GTK_LIBS    =$(shell gtk-config --libs)
PLATFORM_GTK_LDFLAGS=

# NOTE: we only support wxWindows over GTK
PLATFORM_WX_CPPFLAGS= $(shell wx-config --cxxflags) $(PLATFORM_GTK_CPPFLAGS)
PLATFORM_WX_LIBS    = $(shell wx-config --libs)     $(PLATFORM_GTK_LIBS)
PLATFORM_WX_LDFLAGS = $(shell wx-config --ldflags)  $(PLATFORM_GTK_LDFLAGS)

PLATFORM_TCL_CPPFLAGS ?= -I/usr/include/tcl8.4
PLATFORM_TCL_LIBS ?= -ltcl8.4
PLATFORM_TCL_LDFLAGS =

PLATFORM_TK_CPPFLAGS ?= -I$(shell . /usr/lib/tk8.4/tkConfig.sh && echo -n $$TK_INC_DIR $$TK_DEFS)
PLATFORM_TK_LIBS ?= $(shell . /usr/lib/tk8.4/tkConfig.sh && echo -n $$TK_LIB_FLAG)
PLATFORM_TK_LDFLAGS ?=

PLATFORM_AIO_SUPPORT := \
       $(shell test -e /usr/lib/librt.so && echo -DACE_HAS_AIO_CALLS)

SYSARCH := $(shell uname -m)

ifeq ($(insure),1)
  CC  = insure
  CXX = insure
else
  CC  = icc
  CXX = icc
endif

ifndef CXX_VERSION
  CXX_VERSION := $(shell $(CXX) --version)
endif

ifeq (8.0,$(findstring 8.0,$(CXX_VERSION)))
  CFLAGS += -wd1476,1505
endif
ifeq (8.1,$(findstring 8.1,$(CXX_VERSION)))
  CFLAGS += -wd1476,1505,1572 -no-gcc
endif

CFLAGS  += -w1
ifeq ($(threads),1)
    CPPFLAGS  += -D_REENTRANT $(PLATFORM_AIO_SUPPORT)
endif # threads

CCFLAGS += $(CFLAGS) $(IMPLICIT_TEMPLATES_FLAG) -ip
DCFLAGS += -g
DLD     = $(CXX)
LD      = $(CXX)
LIBS    += -ldl

ifeq ($(threads),1)
  LIBS += -lpthread
  ifeq (-DACE_HAS_AIO_CALLS,$(PLATFORM_AIO_SUPPORT))
    LIBS += -lrt
  endif
endif

PLATFORM_QT_CPPFLAGS ?= -I$(QTDIR)/include
PLATFORM_QT_LIBS ?= -lqt-mt
PLATFORM_QT_LDFLAGS ?= -L$(QTDIR)/lib

OCFLAGS += -O3
ifneq ($(SYSARCH),ia64)
  OCFLAGS += -axK
endif

# Disable floating point optimizer solves problem with
# min/max float values in the TAO_IDL compiler
ifeq ($(SYSARCH),ia64)
  CPPFLAGS += -mp
endif

PIC     = -KPIC
AR      = ar
ARFLAGS = rsuv
RANLIB  = @true
SOFLAGS += $(CPPFLAGS) -shared
SOBUILD = $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.so $<; \
          $(SOLINK.cc) -o $@ $(LDFLAGS) $(VSHDIR)$*.o
PRELIB  = @true

ifeq ($(shared_libs), 1)
  ifneq ($static_libs_only), 1)
    LDFLAGS += -Wl,-E
  endif
endif

# Added line below to support "Executable Shared Object" files (as
# needed by the service configurator).
# Marius Kjeldahl <mariusk@sn.no, marius@funcom.com>
ifeq ($(threads),1)
    ESOBUILD = $(COMPILEESO.cc) $(PIC) -shared -o $(VSHDIR)$*.so $<
    ifndef PRELIB
       PRELIB = @true
    endif # ! PRELIB
endif
