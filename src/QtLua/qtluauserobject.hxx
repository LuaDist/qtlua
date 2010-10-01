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


#ifndef QTLUAUSEROBJECT_HXX_
#define QTLUAUSEROBJECT_HXX_

#include "qtluauserdata.hxx"
#include "qtluaiterator.hxx"

namespace QtLua {

  template <class T>
  int UserObject<T>::get_entry(const String &name)
  {
    for (size_t i = 0; i < sizeof(T::member_table) / sizeof(T::member_table[0]); i++)
      if (name == T::member_table[i].name)
	return i;
    throw String("No such property `%'.").arg(name);
  }

  template <class T>
  Value UserObject<T>::meta_index(State &ls, const Value &key)
  {
    String name = key.to_string();
    int index = get_entry(name);

    if (!T::member_table[index].get)
      throw String("Unable to get `%' property value").arg(name);

    return (static_cast<T*>(this)->*T::member_table[index].get)(ls);
  }

  template <class T>
  void UserObject<T>::meta_newindex(State &ls, const Value &key, const Value &value)
  {
    String name = key.to_string();
    int index = get_entry(name);

    if (!T::member_table[index].set)
      throw String("Unable to set `%' property value").arg(name);

    (static_cast<T*>(this)->*T::member_table[index].set)(ls, value);
  }

  template <class T>
  Ref<Iterator> UserObject<T>::new_iterator(State &ls)
  {
    return QTLUA_REFNEW(UserObjectIterator, ls, *this);
  }

  template <class T>
  bool UserObject<T>::support(Value::Operation c) const
  {
    switch (c)
      {
      case Value::OpIndex:
      case Value::OpNewindex:
      case Value::OpIterate:
	return true;
      default:
	return false;
      }
  }

  template <class T>
  UserObject<T>::UserObjectIterator::UserObjectIterator(State &ls, const Ref<UserObject<T> > &obj)
    : _ls(ls),
      _obj(obj),
      _index(0)
  {
  }

  template <class T>
  bool UserObject<T>::UserObjectIterator::more() const
  {
    return _index < sizeof(T::member_table) / sizeof(T::member_table[0]);
  }

  template <class T>
  void UserObject<T>::UserObjectIterator::next()
  {
    _index++;
  }

  template <class T>
  Value UserObject<T>::UserObjectIterator::get_key() const
  {
    return Value(_ls, T::member_table[_index].name);
  }

  template <class T>
  Value UserObject<T>::UserObjectIterator::get_value() const
  {
    if (T::member_table[_index].get)
      return (static_cast<T*>(_obj.ptr())->*T::member_table[_index].get)(_ls);
    else
      return QtLua::Value(_ls);
  }

  template <class T>
  ValueRef UserObject<T>::UserObjectIterator::get_value_ref()
  {
    return ValueRef(Value(_ls, _obj.template staticcast<T>()),
		    Value(_ls, T::member_table[_index].name));
  }

}

#endif

