include $(top_srcdir)/build/autotroll.mk

SUBDIRS = QtLua internal

lib_LTLIBRARIES = libqtlua.la

BUILT_SOURCES = QtLua/qtluaconsole.moc.cc QtLua/qtluaitemselectionmodel.moc.cc \
		QtLua/qtluaitemmodel.moc.cc QtLua/qtluatabletreemodel.moc.cc \
		QtLua/qtluatabledialog.moc.cc QtLua/qtluastate.moc.cc	\
		QtLua/qtluatablegridmodel.moc.cc			\
		qtluaqtlib.moc.cc

libqtlua_la_SOURCES = $(BUILT_SOURCES) qtluaconsole.cc qtluaenum.cc		\
	qtluafunction.cc qtluaitem.cc qtluaitemmodel.cc	qtlualistitem.cc	\
	qtlualistiterator.cc qtluamember.cc qtluametacache.cc qtluamethod.cc	\
	qtluaplugin.cc qtluaqobjectwrapper.cc qtluaqtlib.cc qtluastate.cc	\
	qtluatableiterator.cc qtluauserdata.cc qtluavalue.cc			\
	qtluavalueref.cc qtluaqobjectiterator.cc qtluaenumiterator.cc		\
	qtluaproperty.cc qtluaqmetaobjecttable.cc qtluaqmetaobjectwrapper.cc	\
	qtluaitemselectionmodel.cc qtluaqtlib.hh qtluatabletreekeys.cc		\
	qtluatabletreemodel.cc qtluatabledialog.cc qtluatablegridmodel.cc

libqtlua_la_CXXFLAGS = $(QT_CXXFLAGS) $(AM_CXXFLAGS)
libqtlua_la_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS)
libqtlua_la_LDFLAGS  = $(QT_LDFLAGS) $(LDFLAGS) $(libtool_flags) -release $(PACKAGE_VERSION)
libqtlua_la_LIBADD   = $(QT_LIBS) $(LIBADD)

