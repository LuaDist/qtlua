# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( qtlua C CXX )
cmake_minimum_required ( VERSION 2.8 )
include ( cmake/dist.cmake )
include ( lua )

# Find Qt4
find_package ( Qt4 REQUIRED )
set ( QT_USE_QTUITOOLS 1 )
include ( ${QT_USE_FILE} )

# Find Lua
find_package ( Lua51 REQUIRED )
include_directories ( ${LUA_INCLUDE_DIR} )

# qtlua library
###########################################################################
# generating config.h file:
message ( "Configure: qtlua - generating config.hh:" )
include ( CheckIncludeFiles )
include ( CheckFunctionExists )
include ( CheckSymbolExists )
include ( CheckLibraryExists )

check_include_files ( dlfcn.h HAVE_DLFCN_H )
check_include_files ( inttypes.h HAVE_INTTYPES_H )
check_library_exists ( m "" "" HAVE_LIBM )
check_include_files ( memory.h HAVE_MEMORY_H )
check_include_files ( stdint.h HAVE_STDINT_H )
check_include_files ( stdlib.h HAVE_STDLIB_H )
check_include_files ( strings.h HAVE_STRINGS_H )
check_include_files ( string.h HAVE_STRING_H )
check_include_files ( "sys/stat.h" HAVE_SYS_STAT_H )
check_include_files ( "sys/types.h" HAVE_SYS_TYPES_H )
check_include_files ( unistd.h HAVE_UNISTD_H )
set ( CMAKE_REQUIRED_INCLUDES ${LUA_INCLUDE_DIR} )
set ( CMAKE_REQUIRED_LIBRARIES ${LUA_LIBRARIES} )
check_function_exists ( lua_gc HAVE_LUA_GC )
check_function_exists ( luaL_openlib HAVE_LUA_OPENLIBS )
check_function_exists ( luaopen_os HAVE_LUA_OSLIB )
check_function_exists ( luaopen_package HAVE_LUA_PACKAGELIB )

check_include_files ( "stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS )
set ( SHLIBEXT ${CMAKE_SHARED_LIBRARY_SUFFIX} )

#2do - is it needed?
#~ 		LT_OBJDIR

set ( PACKAGE_NAME "\"libqtlua\"" )
set ( PACKAGE_TARNAME "\"libqtlua\"" )
set ( PACKAGE_VERSION "\"1.2\"" )
set ( PACKAGE_STRING "\"libqtlua 1.2\"" )
set ( PACKAGE_BUGREPORT "\"\"" )
set ( PACKAGE_URL "\"\"" )

# all is defined, so generate config.hh
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/config.hh.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.hh )

message ( "Configure: qtlua - done." )
# end of generating config.hh file

add_definitions ( -DHAVE_CONFIG_H )
include_directories ( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories ( ${CMAKE_CURRENT_BINARY_DIR}/src )
# hack for stupid #include "../config.hh"
###########################################################################

include_directories ( src src/internal src/QtLua )

set ( QTLUA_LIB_SRCS src/qtluaconsole.cc src/qtluaenum.cc src/qtluafunction.cc src/qtluaitem.cc
  src/qtluaitemmodel.cc src/qtlualistitem.cc src/qtlualistiterator.cc src/qtluamember.cc
  src/qtluametacache.cc src/qtluamethod.cc src/qtluaplugin.cc src/qtluaqobjectwrapper.cc
  src/qtluaqtlib.cc src/qtluastate.cc src/qtluatableiterator.cc src/qtluauserdata.cc
  src/qtluavalue.cc src/qtluavalueref.cc src/qtluaqobjectiterator.cc src/qtluaenumiterator.cc
  src/qtluaproperty.cc src/qtluaqmetaobjecttable.cc src/qtluaqmetaobjectwrapper.cc
  src/qtluaitemselectionmodel.cc src/qtluatabletreekeys.cc src/qtluatabletreemodel.cc
  src/qtluatabledialog.cc src/qtluatablegridmodel.cc )

# Generate moc files
set ( MOC_HEADERS src/QtLua/qtluaconsole.hh src/QtLua/qtluaitemselectionmodel.hh
  src/QtLua/qtluaitemmodel.hh src/QtLua/qtluatabletreemodel.hh src/QtLua/qtluatabledialog.hh
  src/QtLua/qtluastate.hh src/QtLua/qtluatablegridmodel.hh src/qtluaqtlib.hh )
qt4_wrap_cpp ( MOC_OUTFILES ${MOC_HEADERS} )
add_library ( qtlua ${QTLUA_LIB_SRCS} ${MOC_OUTFILES} )
target_link_libraries ( qtlua ${QT_LIBRARIES} ${LUA_LIBRARIES} )

# qtlua app
add_executable ( qtlua_app tools/qtlua/qtlua.cc )
target_link_libraries ( qtlua_app qtlua )
set_target_properties ( qtlua_app PROPERTIES OUTPUT_NAME qtlua CLEAN_DIRECT_OUTPUT
  1 )
# tests
include_directories ( test )

add_executable ( test_value test/test_value.cc )
target_link_libraries ( test_value qtlua )

add_executable ( test_table test/test_table.cc )
target_link_libraries ( test_table qtlua )

# Generate moc files
qt4_wrap_cpp ( MOC_OUTFILES_TEST test/test_qobject_arg.hh )
add_executable ( test_qobject_arg test/test_qobject_arg.cc ${MOC_OUTFILES_TEST} )
target_link_libraries ( test_qobject_arg qtlua )

add_executable ( test_item test/test_item.cc )
target_link_libraries ( test_item qtlua )

# examples
# console
qt4_wrap_cpp ( MOC_OUTFILES_CONSOLE examples/cpp/console/console.hh )
add_executable ( console examples/cpp/console/console_fcn.cc ${MOC_OUTFILES_CONSOLE} )
target_link_libraries ( console qtlua )

# mvc
qt4_wrap_cpp ( MOC_OUTFILES_TREEVIEW examples/cpp/mvc/itemtreeview.hh )
add_executable ( itemtreeview examples/cpp/mvc/itemtreeview.cc ${MOC_OUTFILES_TREEVIEW} )
target_link_libraries ( itemtreeview qtlua )

qt4_wrap_cpp ( MOC_OUTFILES_TABLEGRIDVIEW examples/cpp/mvc/tablegridview.hh )
add_executable ( tablegridview examples/cpp/mvc/tablegridview.cc ${MOC_OUTFILES_TABLEGRIDVIEW} )
target_link_libraries ( tablegridview qtlua )
qt4_wrap_cpp ( MOC_OUTFILES_TABLETREEVIEW examples/cpp/mvc/tabletreeview.hh )
add_executable ( tabletreeview examples/cpp/mvc/tabletreeview.cc ${MOC_OUTFILES_TABLETREEVIEW} )
target_link_libraries ( tabletreeview qtlua )

qt4_wrap_cpp ( MOC_OUTFILES_PLUGIN examples/cpp/plugin/plugin.hh )
add_library ( plugin MODULE examples/cpp/plugin/plugin.cc ${MOC_OUTFILES_PLUGIN} )
set_target_properties ( plugin PROPERTIES PREFIX "" )
# plugin
add_executable ( plugin_app examples/cpp/plugin/plugin_load.cc )
target_link_libraries ( plugin_app qtlua )
set_target_properties ( plugin_app PROPERTIES OUTPUT_NAME plugin CLEAN_DIRECT_OUTPUT
  1 )

# proxy
add_executable ( qhashproxy_value examples/cpp/proxy/qhashproxy_value.cc )
target_link_libraries ( qhashproxy_value qtlua )
add_executable ( qlinkedlistproxy_string examples/cpp/proxy/qlinkedlistproxy_string.cc )
target_link_libraries ( qlinkedlistproxy_string qtlua )
add_executable ( qlistproxy_string examples/cpp/proxy/qlistproxy_string.cc )
target_link_libraries ( qlistproxy_string qtlua )
add_executable ( qmapproxy_string examples/cpp/proxy/qmapproxy_string.cc )
target_link_libraries ( qmapproxy_string qtlua )
add_executable ( qvectorproxy_string examples/cpp/proxy/qvectorproxy_string.cc )
target_link_libraries ( qvectorproxy_string qtlua )
add_executable ( qvectorproxy_userdata examples/cpp/proxy/qvectorproxy_userdata.cc )
target_link_libraries ( qvectorproxy_userdata qtlua )

# qobject
add_executable ( qobject_iter examples/cpp/qobject/qobject_iter.cc )
target_link_libraries ( qobject_iter qtlua )
add_executable ( qobject_owner examples/cpp/qobject/qobject_owner.cc )
target_link_libraries ( qobject_owner qtlua )

# userdata
add_executable ( function examples/cpp/userdata/function.cc )
target_link_libraries ( function qtlua )
add_executable ( ref examples/cpp/userdata/ref.cc )
target_link_libraries ( ref qtlua )
# value
add_executable ( global examples/cpp/value/global.cc )
target_link_libraries ( global qtlua )
add_executable ( iterate examples/cpp/value/iterate.cc )
target_link_libraries ( iterate qtlua )

# install
install_executable ( qtlua_app )
install_library ( qtlua )
install_header( src/QtLua )
install ( TARGETS test_value test_table test_qobject_arg test_item DESTINATION ${INSTALL_TEST} COMPONENT Test )
install_example ( examples/lua/hello INTO lua )
install ( TARGETS console itemtreeview tablegridview tabletreeview plugin plugin_app
  console plugin plugin_app qhashproxy_value qlinkedlistproxy_string qlistproxy_string
  qmapproxy_string qvectorproxy_string qvectorproxy_userdata qobject_iter qobject_owner
  function ref global iterate DESTINATION ${INSTALL_EXAMPLE}/cpp COMPONENT Example )
