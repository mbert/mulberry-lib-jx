##  Process this file with automake to create Makefile.in
##
## Makefile.am,v 1.10 2005/03/18 22:16:09 jtc Exp
##

includedir = @includedir@/ace/QoS
pkgconfigdir = @libdir@/pkgconfig

ACE_BUILDDIR = $(top_builddir)
ACE_ROOT = $(top_srcdir)


## Makefile.QoS.am

if BUILD_QOS

lib_LTLIBRARIES = libACE_QoS.la

libACE_QoS_la_CPPFLAGS = \
  -I$(ACE_ROOT) \
  -I$(ACE_BUILDDIR) \
  -DACE_QoS_BUILD_DLL

libACE_QoS_la_SOURCES = \
  QoS_Decorator.cpp \
  QoS_Manager.cpp \
  QoS_Session_Factory.cpp \
  QoS_Session_Impl.cpp \
  SOCK_Dgram_Mcast_QoS.cpp

libACE_QoS_la_LDFLAGS = \
 -version-number @ACE_MAJOR@:@ACE_MINOR@:@ACE_BETA@

libACE_QoS_la_LIBADD = \
  $(ACE_BUILDDIR)/ace/libACE.la

nobase_include_HEADERS = \
  ACE_QoS_Export.h \
  QoS_Decorator.h \
  QoS_Manager.h \
  QoS_Session.h \
  QoS_Session_Factory.h \
  QoS_Session_Impl.h \
  QoS_Session_Impl.i \
  SOCK_Dgram_Mcast_QoS.h \
  SOCK_Dgram_Mcast_QoS.i

pkgconfig_DATA = ACE_QoS.pc

ACE_QoS.pc: ${top_builddir}/config.status ${srcdir}/ACE_QoS.pc.in
	${top_builddir}/config.status --file $@:${srcdir}/ACE_QoS.pc.in

endif BUILD_QOS

EXTRA_DIST = \
  ACE_QoS.pc.in


## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
	-rm -f gcctemp.c gcctemp so_locations *.ics
	-rm -rf cxx_repository ptrepository ti_files
	-rm -rf templateregistry ir.out
	-rm -rf ptrepository SunWS_cache Templates.DB
