
ACLOCAL_AMFLAGS = -I build

SUBDIRS = src tools test doc

if EXAMPLES
SUBDIRS += examples
endif

EXTRA_DIST = bootstrap COPYING.LESSER

doc: doc/manual doc/internals

doc/manual: doc/manual.conf doc/top.mkdoc doc/features.mkdoc
	mkdoc -I src qt_4.5.mkdoclib --conf-file doc/manual.conf src/QtLua/*.hh src/internal/*.hh
	touch $@

doc/internals: doc/internals.conf doc/top.mkdoc doc/features.mkdoc
	mkdoc -I src qt_4.5.mkdoclib --conf-file doc/internals.conf src/QtLua/*.hh src/internal/*.hh
	touch $@

doc/www: doc/web.conf doc/web.mkdoc doc/features.mkdoc
	mkdoc --conf-file doc/web.conf
	touch $@

.PHONY: doc

