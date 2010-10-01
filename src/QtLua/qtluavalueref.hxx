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


#ifndef QTLUAVALUEREF_HXX_
#define QTLUAVALUEREF_HXX_

#include <cassert>

#include "qtluavalue.hxx"

namespace QtLua {

  ValueRef::ValueRef(const Value &table, const Value &key)
    : Value(table._st),
      _key(key)
  {
    assert(table._st == key._st);
    init(table);
  }

  template <typename T>
  ValueRef::ValueRef(const Value &table, const T &key)
    : Value(table._st),
      _key(Value(table._st))
  {
    _key = key;
    init(table);
  }

  const ValueRef & ValueRef::operator=(const ValueRef &ref) const
  {
    *this = static_cast<const Value &>(ref);
    return *this;
  }

  const ValueRef & ValueRef::operator=(Bool n) const
  {
    *this = Value(_st, n);
    return *this;
  }

  const ValueRef & ValueRef::operator=(double n) const
  {
    *this = Value(_st, n);
    return *this;
  }

  const ValueRef & ValueRef::operator=(int n) const
  {
    *this = Value(_st, (double)n);
    return *this;
  }

  const ValueRef & ValueRef::operator=(const String &str) const
  {
    *this = Value(_st, str);
    return *this;
  }

  const ValueRef & ValueRef::operator=(const UserData::ptr &ud) const
  {
    *this = Value(_st, ud);
    return *this;
  }

  const ValueRef & ValueRef::operator=(QObject *obj) const
  {
    *this = Value(_st, obj);
    return *this;
  }

}

#endif

