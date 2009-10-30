#-*- Makefile -*-

#----------------------------------------------------------------------------
#	macros.GNU,v 4.3 2003/11/21 13:18:00 dhinton Exp
#
#	Local and nested target definitions
#----------------------------------------------------------------------------

TARGETS_LOCAL  = \
	all.local \
	debug.local \
	profile.local \
	optimize.local \
	install.local \
	deinstall.local \
	clean_idl_stubs.local \
	clean.local \
	realclean.local \
	clobber.local \
	depend.local \
	rcs_info.local \
	idl_stubs.local

TARGETS_NESTED   = \
	$(TARGETS_LOCAL:.local=.nested)

