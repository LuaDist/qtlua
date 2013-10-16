/*
    This file is part of LibQtLua.

    LibQtLua is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LibQtLua is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LibQtLua.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (C) 2008, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/

// This example show how to use a QHashProxy object to access a QHash
// object from lua script.

#include <iostream>

#include <QHash>
#include <QVariant>

#include <QtLua/State>
#include <QtLua/QHashProxy>

int main()
{
  try {
    typedef QHash<QtLua::String, QVariant> Container;

    // QHash we want to access from lua
    Container hash;

    // Hash proxy which provides access to our QHash from lua
    QtLua::QHashProxy<Container> proxy(hash);

    QtLua::State state;
    state.openlib(QtLua::QtLuaLib);

    // Declare a lua global variable using our hash proxy
    state["hash"] = proxy;

    // Insert a value in QHash directly
    hash.insert("key0", QVariant());
    hash.insert("key1", QVariant("value"));
    hash.insert("key2", QVariant(42.21));

    // Read/Write in QHash from lua using the proxy object
    state.exec_statements("hash.key3 = hash.key1");

    // Read back value in QHash inserted from lua script
    QVariant value = hash.value("key3");

    std::cout << value.toByteArray().data() << std::endl;

    // Remove key2 entry from lua script
    state.exec_statements("hash.key1 = nil");

    // Iterate through QHash from lua script
    state.exec_statements("for key, value in each(hash) do print(key, value) end");

    hash.clear();

  } catch (QtLua::String &e) {
    std::cerr << e.constData() << std::endl;
  }

}

