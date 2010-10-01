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


#ifndef QTLUAVALUEREF_HH_
#define QTLUAVALUEREF_HH_

#include "qtluavalue.hh"

namespace QtLua {

  class State;

  /**
   * @short Lua value reference object class
   * @header QtLua/ValueRef
   * @module {Base}
   *
   * This class acts as a reference to a lua value stored in a lua
   * table (or userdata value). It stores two lua values: a table
   * along with a key value.
   *
   * This is mainly used in the @ref State, @ref Value and
   * @ref Value::iterator classes to allow modification of lua tables with
   * the C++ square bracket operator functions.
   */
  class ValueRef : public Value
  {
    friend class Value;
    friend class State;

  public:
    /** Construct reference with given table and key. */
    inline ValueRef(const Value &table, const Value &key);
    /** Construct reference with given table and key. */
    template <typename T>
    inline ValueRef(const Value &table, const T &key);

    ValueRef(const ValueRef &ref);

    /** Assign new value to referenced value. */
    const ValueRef & operator=(const Value &v) const;
    /** Assign new value to referenced value. */
    inline const ValueRef & operator=(const ValueRef &v) const;
    /** Assign new boolean to referenced value. */
    inline const ValueRef & operator=(Bool n) const;
    /** Assign new number to referenced value. */
    inline const ValueRef & operator=(double n) const;
    /** Assign new number to referenced value. */
    inline const ValueRef & operator=(int n) const;
    /** Assign new string to referenced value. */
    inline const ValueRef & operator=(const String &str) const;
    /** Assign new user data to referenced value. */
    inline const ValueRef & operator=(const Ref<UserData> &ud) const;
    /** Assign a wrapped QObject to reference. */
    inline const ValueRef & operator=(QObject *obj) const;

  private:
    void init(const Value &table);
    void push_value() const;

    Value _key;
  };

}

#endif

