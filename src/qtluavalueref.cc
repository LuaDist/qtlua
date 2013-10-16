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

#include <cassert>
#include <cstdlib>

#include <QtLua/ValueRef>

extern "C" {
#include <lua.h>
}

namespace QtLua {

  void ValueRef::init(const Value &table)
  {
    check_state();
    lua_State *lst = _st->_lst;

    lua_pushlightuserdata(lst, this);
    table.push_value();

    int t = lua_type(lst, -1);

    switch (t)
      {
      case TUserData:
      case TTable:
	lua_rawset(lst, LUA_REGISTRYINDEX);
	break;

      default:
	lua_pop(lst, 2);
	throw String("Can not make value reference with lua::% type as table.").arg(lua_typename(lst, t));
      }
  }

  ValueRef::ValueRef(const ValueRef &ref)
    : Value(*ref._st),
      _key(ref._key)
  {
    if (!_st)
      return;

    lua_State *lst = _st->_lst;

    lua_pushlightuserdata(lst, this);
    lua_pushlightuserdata(lst, (void*)&ref);
    lua_rawget(lst, LUA_REGISTRYINDEX);  
    lua_rawset(lst, LUA_REGISTRYINDEX);
  }

  void ValueRef::push_value() const
  {
    check_state();
    lua_State *lst = _st->_lst;

    // get table object
    lua_pushlightuserdata(lst, (void*)this);
    lua_rawget(lst, LUA_REGISTRYINDEX);  

    int t = lua_type(lst, -1);

    switch (t)
      {
      case TUserData:
	try {
	  UserData::ptr ud = UserData::get_ud(lst, -1);
	  ud->meta_index(*_st, _key).push_value();
	  lua_remove(lst, -2);

	} catch (const String &e) {
	  lua_pop(lst, 1);
	  lua_pushnil(lst);
	}
	break;

      case TTable:
	_key.push_value();
	lua_gettable(lst, -2);
	lua_remove(lst, -2);
	break;

      default:
	abort();
      }
  }

  const ValueRef & ValueRef::operator=(const Value &v) const
  {
    if (!_st)
      return *this;

    lua_State *lst = _st->_lst;

    lua_pushlightuserdata(lst, (void*)this);
    lua_rawget(lst, LUA_REGISTRYINDEX);  

    switch (lua_type(lst, -1))
      {
      case TUserData: {
	UserData::ptr ud = UserData::pop_ud(lst);
	ud->meta_newindex(*_st, _key, v);
	break;
      }

      case TTable:
	_key.push_value();
	if (lua_isnil(lst, -1))
	  {
	    lua_pop(lst, 2);
	  }
	else
	  {
	    v.push_value();
	    lua_settable(lst, -3);
	    lua_pop(lst, 1);
	  }
	break;

      default:
	std::abort();
      }

    return *this;
  }

}

