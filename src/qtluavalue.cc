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

#include <cstdlib>
#include <cassert>

#include <QMetaMethod>

#include <QtLua/Value>
#include <QtLua/ValueRef>
#include <QtLua/UserData>
#include <QtLua/String>
#include <QtLua/State>

#include <internal/Member>
#include <internal/QObjectWrapper>
#include <internal/TableIterator>

extern "C" {
#include <lua.h>
}

namespace QtLua {

void Value::check_state() const
{
  if (!_st)
    throw String("Can't perform operations on QtLua::Value which has no associated QtLua::State object");
}

void Value::push_value() const
{
  check_state();
  lua_State *lst = _st->_lst;
  lua_pushlightuserdata(lst, (void*)this);
  lua_rawget(lst, LUA_REGISTRYINDEX);  
}

int Value::empty_fcn(lua_State *st)
{
  return 0;
}

void Value::init_type_value(ValueType type)
{
  check_state();
  lua_State *lst = _st->_lst;
  lua_pushlightuserdata(lst, this);

  switch (type)
    {
    case TNone:
    case TNil:
      lua_pushnil(lst);
      break;

    case TBool:
      lua_pushboolean(lst, false);
      break;

    case TNumber:
      lua_pushnumber(lst, 0.0f);
      break;

    case TString:
      lua_pushstring(lst, "");
      break;

    case TTable:
      lua_newtable(lst);
      break;

    case TFunction:
      lua_pushcfunction(lst, empty_fcn);
      break;

    case TUserData: {
      UserData::ptr ud = QTLUA_REFNEW(QtLua::UserData, );
      ud->push_ud(lst);
      break;
    }

    }

  lua_rawset(lst, LUA_REGISTRYINDEX);
}

Value & Value::operator=(Bool n)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushlightuserdata(lst, (void*)this);
      lua_pushboolean(lst, n);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value & Value::operator=(double n)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushlightuserdata(lst, (void*)this);
      lua_pushnumber(lst, n);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value & Value::operator=(const String &str)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushlightuserdata(lst, this);
      lua_pushlstring(lst, str.constData(), str.size());
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value & Value::operator=(const Ref<UserData> &ud)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushlightuserdata(lst, this);
      ud->push_ud(lst);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value::Value(State &ls, QObject *obj, bool delete_, bool reparent)
  : _st(&ls)
{
  lua_State *lst = _st->_lst;
  lua_pushlightuserdata(lst, this);
  QObjectWrapper::get_wrapper(*_st, obj, reparent, delete_)->push_ud(lst);
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

Value & Value::operator=(QObject *obj)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushlightuserdata(lst, this);
      QObjectWrapper::get_wrapper(*_st, obj)->push_ud(lst);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value & Value::operator=(const QVariant &qv)
{
  if (_st)
    *this = Member::raw_get_object(*_st, qv.type(), qv.constData());
  return *this;
}

bool Value::connect(QObject *obj, const char *signal)
{
  check_state();
  try {
    QObjectWrapper::ptr qow = QObjectWrapper::get_wrapper(*_st, obj);
    QByteArray ns(QMetaObject::normalizedSignature(signal));
    const QMetaObject *mo = obj->metaObject();
    int sigid = mo->indexOfMethod(ns.constData());

    if (sigid < 0 || mo->method(sigid).methodType() != QMetaMethod::Signal)
      return false;

    qow->_lua_connect(sigid, *this);

  } catch (const String &e) {
    return false;
  }
  return true;
}

bool Value::disconnect(QObject *obj, const char *signal)
{
  check_state();
  QObjectWrapper::ptr qow = QObjectWrapper::get_wrapper(*_st, obj);
  QByteArray ns(QMetaObject::normalizedSignature(signal));
  const QMetaObject *mo = obj->metaObject();
  int sigid = mo->indexOfMethod(ns.constData());

  if (sigid < 0 || mo->method(sigid).methodType() != QMetaMethod::Signal)
    return false;

  return qow->_lua_disconnect(sigid, *this);
}

Value::List Value::call (const List &args) const
{
  push_value();
  lua_State *lst = _st->_lst;

  int t = lua_type(lst, -1);

  switch (t)
    {
    case TFunction: {
      int oldtop = lua_gettop(lst);

      if (!lua_checkstack(lst, args.size()))
	throw String("Unable to extend lua stack to handle % arguments").arg(args.size());

      foreach(const Value &v, args)
	v.push_value();

      if (!lua_pcall(lst, args.size(), LUA_MULTRET, 0))
	{
	  Value::List res;

	  for (int i = oldtop; i <= lua_gettop(lst); i++)
	    res += Value(i, _st);

	  lua_pop(lst, lua_gettop(lst) - oldtop + 1);
	  return res;
	}

      String err(lua_tostring(lst, -1));
      lua_pop(lst, 1);
      throw err;
    }

    case TUserData: {
      UserData::ptr ud = UserData::pop_ud(lst);

      if (!ud.valid())
	throw String("Can not call null lua::userdata value.");

      return ud->meta_call(*_st, args);
    }

    default:
      lua_pop(lst, 1);
      throw String("Can not call lua::% value.").arg(lua_typename(lst, t));
    }
}

Value Value::operator[] (const Value &key) const
{
  push_value();
  lua_State *lst = _st->_lst;

  int t = lua_type(lst, -1);

  switch (t)
    {
    case TUserData: {
      UserData::ptr ud = UserData::pop_ud(lst);

      if (!ud.valid())
	throw String("Can not index null lua::userdata value.");

      return ud->meta_index(*_st, key);
    }

    case TTable: {
      key.push_value();
      lua_gettable(lst, -2);
      Value res(-1, _st);
      lua_pop(lst, 2);
      return res;
    }

    default:
      lua_pop(lst, 1);
      throw String("Can not index lua::% value.").arg(lua_typename(lst, t));
    }
}

Ref<Iterator> Value::new_iterator() const
{
  push_value();
  lua_State *lst = _st->_lst;

  switch (int t = lua_type(lst, -1))
    {
    case TUserData: {
      UserData::ptr ud = UserData::pop_ud(lst);

      if (!ud.valid())
	throw String("Can not iterate through null lua::userdata value.");

      return ud->new_iterator(*_st);
    }

    case TTable: {
      Iterator::ptr it = QTLUA_REFNEW(TableIterator, *_st, *this);
      lua_pop(lst, 1);
      return it;
    }

    default:
      lua_pop(lst, 1);
      throw String("Can not iterate through lua::% value.").arg(lua_typename(lst, t));
    }
}

Value & Value::operator=(const Value &lv)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushlightuserdata(lst, this);
      lua_pushnil(lst);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }

  _st = lv._st;

  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushlightuserdata(lst, this);
      lv.push_value();
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }

  return *this;
}

Value::Value(const Value &lv)
  : _st(lv._st)
{
  if (!_st)
    return;

  lua_State *lst = _st->_lst;
  lua_pushlightuserdata(lst, this);
  lv.push_value();
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

Value::Value(const State *ls, const Value &lv)
  : _st(const_cast<State*>(ls))
{
  assert(_st == lv._st);

  if (!_st)
    return;

  lua_State *lst = _st->_lst;
  lua_pushlightuserdata(lst, this);
  lv.push_value();
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

Value::Value(const State &ls, const Value &lv)
  : _st(const_cast<State*>(&ls))
{
  assert(_st == lv._st);

  lua_State *lst = _st->_lst;
  lua_pushlightuserdata(lst, this);
  lv.push_value();
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

Value::~Value()
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushlightuserdata(lst, this);
      lua_pushnil(lst);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
}

Value::Bool Value::to_boolean() const
{
  push_value();
  lua_State *lst = _st->_lst;
  Bool res = (Bool)lua_toboolean(lst, -1);
  lua_pop(lst, 1);
  return res;
}

Value::ValueType Value::type() const
{
  if (!_st)
    return TNil;

  push_value();
  lua_State *lst = _st->_lst;
  int res = lua_type(lst, -1);
  lua_pop(lst, 1);
  return (Value::ValueType)res;
}

String Value::type_name() const
{
  return String("lua::") + lua_typename(NULL, type());
}

String Value::type_name(enum Value::ValueType v)
{
  return String("lua::") + lua_typename(NULL, v);
}

String Value::type_name_u() const
{
  if (!_st)
    return "lua::nil";

  lua_State *lst = _st->_lst;

  push_value();
  int t = lua_type(lst, -1);

  if (t == TUserData)
    {
#ifndef QTLUA_NO_USERDATA_CHECK
      try {
#endif
	UserData::ptr ud = UserData::get_ud(lst, -1);
	lua_pop(lst, 1);
	if (ud.valid())
	  return ud->get_type_name();
#ifndef QTLUA_NO_USERDATA_CHECK
      } catch (const String &e) {
      }
#endif
    }

  lua_pop(lst, 1);
  return String("lua::") + lua_typename(lst, t);
}

Value::Value(int index, const State *st)
  : _st(const_cast<State*>(st))
{
  lua_State *lst = _st->_lst;

  lua_pushlightuserdata(lst, this);
  if (index < 0 && index != LUA_GLOBALSINDEX)
    index--;
  lua_pushvalue(lst, index);
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

void Value::convert_error(ValueType type) const
{
  lua_State *lst = _st->_lst;

  int type_b = lua_type(lst, -1);

  lua_pop(lst, 1);

  throw String("Can not convert lua::% value to lua::%.")
    .arg(lua_typename(lst, type_b)).arg(lua_typename(lst, (int)type));
}

lua_Number Value::to_number() const
{
  push_value();
  lua_State *lst = _st->_lst;

  switch (lua_type(lst, -1))
    {
    case LUA_TBOOLEAN:
    case LUA_TNUMBER: {
      lua_Number res = lua_tonumber(lst, -1);
      lua_pop(lst, 1);
      return res;
    }

    case LUA_TSTRING: {
      char *end;
      lua_Number res = strtod(lua_tostring(lst, -1), &end);
      lua_pop(lst, 1);

      if (!*end)
	return res;
    }

    }

  convert_error(TNumber);
  std::abort();
}

String Value::to_string() const
{
  push_value();
  lua_State *lst = _st->_lst;

  const char	*str = lua_tostring(lst, -1);

  if (str)
    {
      String res(lua_tostring(lst, -1), lua_strlen(lst, -1));
      lua_pop(lst, 1);
      return res;
    }

  convert_error(TString);
  std::abort();
}

String Value::to_string_p(bool quote_string) const
{
  push_value();
  lua_State *lst = _st->_lst;

  String res(to_string_p(lst, -1, quote_string));
  lua_pop(lst, 1);
  return res;
}

String Value::to_string_p(lua_State *st, int index, bool quote_string)
{
  switch (lua_type(st, index))
    {
    case TNone:
      return "(none)";

    case TNil:
      return "(nil)";

    case TBool: {
      String res(lua_toboolean(st, index) ? "true" : "false");
      return res;
    }

    case TNumber: {
      String res;
      res.setNum(lua_tonumber(st, index));
      return res;
    }

    case TString:
      if (quote_string)
	return String("\"") + lua_tostring(st, index) + "\"";
      else
	return String(lua_tostring(st, index));

    case TUserData: {
#ifndef QTLUA_NO_USERDATA_CHECK
      try {
#endif
	UserData::ptr ud = UserData::get_ud(st, index);
	String res(ud.valid() ? ud->get_value_str() : ud->UserData::get_value_str());
	return res;
#ifndef QTLUA_NO_USERDATA_CHECK
      } catch (const String &e) {
	// goto default
      }
#endif
    }

    default: {
      String res;
      res.setNum((qulonglong)lua_topointer(st, index), 16);
      return String("(%:%)").arg(lua_typename(NULL, lua_type(st, index))).arg(res);
    }

    }
}

const char * Value::to_cstring() const
{
  push_value();
  lua_State *lst = _st->_lst;

  const char	*str = lua_tostring(lst, -1);

  if (str)
    {
      lua_pop(lst, 1);
      return str;
    }

  convert_error(TString);
  std::abort();
}

UserData::ptr Value::to_userdata() const
{
  push_value();
  lua_State *lst = _st->_lst;

  if (lua_type(lst, -1) == LUA_TUSERDATA)
    return UserData::pop_ud(lst);

  convert_error(TUserData);
  std::abort();
}

UserData::ptr Value::to_userdata_null() const
{
  push_value();
  lua_State *lst = _st->_lst;

  if (lua_type(lst, -1) == LUA_TUSERDATA)
    {
#ifndef QTLUA_NO_USERDATA_CHECK
      try {
#endif
	UserData::ptr ptr = UserData::get_ud(lst, -1);
	lua_pop(lst, 1);
	return ptr;
#ifndef QTLUA_NO_USERDATA_CHECK
      } catch (const String &e) {
      }
#endif
    }

  lua_pop(lst, 1);
  return UserData::ptr();
}

QObject *Value::to_qobject() const
{
  QObjectWrapper::ptr ow = to_userdata_cast<QObjectWrapper>();

  if (!ow.valid())
    throw String("Can not convert % type to QObject.").arg(type_name());

  return &ow->get_object();
}

QVariant Value::to_qvariant() const
{
  switch (type())
    {
    case TNone:
    case TNil:
      return QVariant();
    case TBool:
      return QVariant(to_boolean());
    case TNumber:
      return QVariant(to_number());
    case TString:
      return QVariant(to_string());

    default:
      throw String("Can not convert % type to QVariant.").arg(type_name());
    }
}

static int lua_writer(lua_State *L, const void* p, size_t sz, void* pv)
{
  QByteArray *ba = (QByteArray*)pv;
  ba->append((const char*)p, (int)sz);
  return 0;
}

QByteArray Value::to_bytecode() const
{
  push_value();
  lua_State *lst = _st->_lst;

  if (lua_type(lst, -1) == LUA_TFUNCTION)
    {
      QByteArray bytecode;
      int status = lua_dump(lst, &lua_writer, &bytecode);	
      lua_pop(lst, 1);
      if (status)
	throw QtLua::String("Unable to dump function bytecode");
      return bytecode;
    }

  convert_error(TUserData);
  std::abort();
}

int Value::len() const
{
  push_value();
  lua_State *lst = _st->_lst;
  size_t res;

  switch (lua_type(lst, -1))
    {
    case TString:
    case TTable:
      res = lua_objlen(lst, -1);
      break;

    case TUserData:
      try {
	UserData::ptr ptr = UserData::get_ud(lst, -1);
	res = ptr->meta_operation(*_st, Value::OpLen, *this, *this).to_integer();
	break;
      } catch (const String &s) {
      }

    default:
      res = 0;
    }

  lua_pop(lst, 1);
  return res;
}

bool Value::support(Operation c) const
{
  push_value();
  lua_State *lst = _st->_lst;
  bool res;

  switch (lua_type(lst, -1))
    {
    case TNone:
    case TNil:
      res = false;
      break;

    case TBool:
      switch (c)
	{
	case Value::OpEq:
	  res = true;
	  break;
	default:
	  res = false;
	  break;
	}
      break;

    case TNumber:
      switch (c)
	{
	case Value::OpAdd:
	case Value::OpSub:
	case Value::OpMul:
	case Value::OpDiv:
	case Value::OpMod:
	case Value::OpPow:
	case Value::OpUnm:
	case Value::OpEq:
	case Value::OpLt:
	case Value::OpLe:
	  res = true;
	  break;
	default:
	  res = false;
	  break;
	}
      break;

    case TString:
      switch (c)
	{
	case Value::OpLen:
	case Value::OpConcat:
	case Value::OpEq:
	case Value::OpLt:
	case Value::OpLe:
	  res = true;
	  break;
	default:
	  res = false;
	  break;
	}
      break;

    case TTable:
      switch (c)
	{
	case Value::OpEq:
	case Value::OpLen:
	case Value::OpIterate:
	case Value::OpIndex:
	case Value::OpNewindex:
	  res = true;
	  break;
	default:
	  res = false;
	  break;
	}
      break;

    case TFunction:
      switch (c)
	{
	case Value::OpEq:
	case Value::OpCall:
	  res = true;
	  break;
	default:
	  res = false;
	  break;
	}
      break;

    case TUserData:
      try {
	UserData::ptr ptr = UserData::get_ud(lst, -1);
	res = ptr->support(c);
      } catch (const String &s) {
	res = false;
      }
      break;
    }

  lua_pop(lst, 1);
  return res;
}

bool Value::operator==(const Value &lv) const
{
  if (lv._st != _st)
    return false;

  lv.push_value();
  push_value();

  lua_State *lst = _st->_lst;
  bool res;

  if ((lua_type(lst, -1) == TUserData) &&
      (lua_type(lst, -2) == TUserData))
    {
#ifndef QTLUA_NO_USERDATA_CHECK
      try {
#endif
	UserData::ptr a = UserData::get_ud(lst, -1);
	UserData::ptr b = UserData::get_ud(lst, -2);

	res = (a.valid() == b.valid()) && (!a.valid() || (*a == *b));
#ifndef QTLUA_NO_USERDATA_CHECK
      } catch (const String &e) {
	res = lua_rawequal(lst, -1, -2);
      }
#endif
    }
  else
    {
      res = lua_equal(lst, -1, -2);
    }

  lua_pop(lst, 2);
  return res;
}

bool Value::operator<(const Value &lv) const
{
  if (lv._st != _st)
    return false;

  lv.push_value();
  push_value();

  lua_State *lst = _st->_lst;
  bool res;

  if ((lua_type(lst, -1) == TUserData) &&
      (lua_type(lst, -2) == TUserData))
    {
#ifndef QTLUA_NO_USERDATA_CHECK
      try {
#endif
	UserData::ptr a = UserData::get_ud(lst, -1);
	UserData::ptr b = UserData::get_ud(lst, -2);

	res = a.valid() && b.valid() && (*a < *b);
#ifndef QTLUA_NO_USERDATA_CHECK
      } catch (const String &e) {
	res = lua_topointer(lst, -1) < lua_topointer(lst, -2);
      }
#endif
    }
  else
    {
      if (lua_type(lst, -1) == lua_type(lst, -2))
	res = lua_lessthan(lst, -1, -2);
      else
	res = lua_type(lst, -1) < lua_type(lst, -2);
    }

  lua_pop(lst, 2);
  return res;
}

bool Value::operator==(const String &str) const
{
  push_value();

  lua_State *lst = _st->_lst;
  bool res = false;

  if (lua_isstring(lst, -1))
    {
      String s(lua_tostring(lst, -1), lua_strlen(lst, -1));

      res = (str == s);
    }

  lua_pop(lst, 1);

  return res;
}

bool Value::operator==(const char *str) const
{
  push_value();

  lua_State *lst = _st->_lst;
  bool res = false;

  if (lua_isstring(lst, -1))
    res = !strcmp(lua_tostring(lst, -1), str);

  lua_pop(lst, 1);

  return res;
}

bool Value::operator==(double n) const
{
  push_value();

  lua_State *lst = _st->_lst;
  bool res = false;

  if (lua_isnumber(lst, -1))
    res = lua_tonumber(lst, -1) == n;

  lua_pop(lst, 1);

  return res;
}

uint Value::qHash(lua_State *lst, int index)
{
  switch (lua_type(lst, index))
    {
    case LUA_TBOOLEAN:
      return lua_toboolean(lst, index);

    case LUA_TNUMBER: {
      lua_Number n = lua_tonumber(lst, index);
      return *(uint*)&n;
    }

    case LUA_TSTRING:
      return ::qHash(String(lua_tostring(lst, index), lua_strlen(lst, index)));

    case LUA_TUSERDATA: {
#ifndef QTLUA_NO_USERDATA_CHECK
      try {
#endif
	QtLua::Ref<UserData> ud = UserData::get_ud(lst, index);
	return (uint)(long)ud.ptr();
#ifndef QTLUA_NO_USERDATA_CHECK
      } catch (...) {
	return (uint)(long)lua_touserdata(lst, index);
      }
#endif
      break;
    }

    default:
      return (uint)(long)lua_topointer(lst, index);
    }
}

uint qHash(const Value &lv)
{
  lv.push_value();

  lua_State *lst = lv._st->_lst;
  uint	res;

  res = Value::qHash(lst, -1);
  lua_pop(lst, 1);
  return res;
}

}

