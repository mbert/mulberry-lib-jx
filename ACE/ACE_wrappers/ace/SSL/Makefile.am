##  Process this file with automake to create Makefile.in
##
## Makefile.am,v 1.14 2005/03/18 22:16:09 jtc Exp
##

includedir = @includedir@/ace/SSL
pkgconfigdir = @libdir@/pkgconfig

ACE_BUILDDIR = $(top_builddir)
ACE_ROOT = $(top_srcdir)


## Makefile.SSL.am

if BUILD_SSL

lib_LTLIBRARIES = libACE_SSL.la

libACE_SSL_la_CPPFLAGS = \
  -I$(ACE_ROOT) \
  -I$(ACE_BUILDDIR) \
  -I$(SSL_ROOT)/include \
  -I/usr/kerberos/include \
  -DACE_SSL_BUILD_DLL

libACE_SSL_la_SOURCES = \
  SSL_Asynch_BIO.cpp \
  SSL_Asynch_Stream.cpp \
  SSL_Context.cpp \
  SSL_SOCK.cpp \
  SSL_SOCK_Acceptor.cpp \
  SSL_SOCK_Connector.cpp \
  SSL_SOCK_Stream.cpp

libACE_SSL_la_LDFLAGS = \
 -version-number @ACE_MAJOR@:@ACE_MINOR@:@ACE_BETA@

libACE_SSL_la_LIBADD = \
  $(ACE_BUILDDIR)/ace/libACE.la \
  -lssl \
  -lcrypto

nobase_include_HEADERS = \
  SSL_Asynch_BIO.h \
  SSL_Asynch_Stream.h \
  SSL_Context.h \
  SSL_Context.inl \
  SSL_Export.h \
  SSL_SOCK.h \
  SSL_SOCK.i \
  SSL_SOCK_Acceptor.h \
  SSL_SOCK_Acceptor.i \
  SSL_SOCK_Connector.h \
  SSL_SOCK_Connector.i \
  SSL_SOCK_Stream.h \
  SSL_SOCK_Stream.i \
  sslconf.h

pkgconfig_DATA = ACE_SSL.pc

ACE_SSL.pc: ${top_builddir}/config.status ${srcdir}/ACE_SSL.pc.in
	${top_builddir}/config.status --file $@:${srcdir}/ACE_SSL.pc.in

endif BUILD_SSL

EXTRA_DIST = \
  ACE_SSL.pc.in


## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
	-rm -f gcctemp.c gcctemp so_locations *.ics
	-rm -rf cxx_repository ptrepository ti_files
	-rm -rf templateregistry ir.out
	-rm -rf ptrepository SunWS_cache Templates.DB
