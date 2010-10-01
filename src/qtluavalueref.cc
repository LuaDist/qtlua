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
    lua_pushlightuserdata(_st, this);
    table.push_value();

    int t = lua_type(_st, -1);

    switch (t)
      {
      case TUserData:
      case TTable:
	lua_rawset(_st, LUA_REGISTRYINDEX);
	break;

      default:
	lua_pop(_st, 2);
	throw String("Can not make value reference with lua::% type as table.").arg(lua_typename(_st, t));
      }
  }

  ValueRef::ValueRef(const ValueRef &ref)
    : Value(ref._st),
      _key(ref._key)
  {
    lua_pushlightuserdata(_st, this);
    lua_pushlightuserdata(_st, (void*)&ref);
    lua_rawget(_st, LUA_REGISTRYINDEX);
    lua_rawset(_st, LUA_REGISTRYINDEX);
  }

  void ValueRef::push_value() const
  {
    // get table object
    lua_pushlightuserdata(_st, (void*)this);
    lua_rawget(_st, LUA_REGISTRYINDEX);  

    int t = lua_type(_st, -1);

    switch (t)
      {
      case TUserData:
	try {
	  UserData::ptr ud = UserData::get_ud(_st, -1);
	  ud->meta_index(*State::get_this(_st), _key).push_value();
	  lua_remove(_st, -2);

	} catch (const String &e) {
	  lua_pop(_st, 1);
	  lua_pushnil(_st);
	}
	break;

      case TTable:
	_key.push_value();
	lua_gettable(_st, -2);
	lua_remove(_st, -2);
	break;

      default:
	abort();
      }
  }

  const ValueRef & ValueRef::operator=(const Value &v) const
  {
    lua_pushlightuserdata(_st, (void*)this);
    lua_rawget(_st, LUA_REGISTRYINDEX);

    switch (lua_type(_st, -1))
      {
      case TUserData: {
	UserData::ptr ud = UserData::pop_ud(_st);
	ud->meta_newindex(*State::get_this(_st), _key, v);
	break;
      }

      case TTable:
	_key.push_value();
	if (lua_isnil(_st, -1))
	  {
	    lua_pop(_st, 2);
	  }
	else
	  {
	    v.push_value();
	    lua_settable(_st, -3);
	    lua_pop(_st, 1);
	  }
	break;

      default:
	std::abort();
      }

    return *this;
  }

}

