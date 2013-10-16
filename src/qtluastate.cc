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

#include "config.hh"

#include <cstdlib>

#include <QStringList>

#include <QtLua/State>
#include <QtLua/UserData>
#include <QtLua/Value>
#include <QtLua/ValueRef>
#include <QtLua/Iterator>
#include <QtLua/String>
#include <QtLua/Function>
#include <internal/QObjectWrapper>

#include "qtluaqtlib.hh"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace QtLua {

char State::_key_item_metatable;
char State::_key_this;

/************************************************************************
	lua c functions
************************************************************************/

int State::lua_cmd_iterator(lua_State *st)
{
  try {
    State		*this_ = get_this(st);
    Iterator::ptr	i = Value(1, this_).to_userdata_cast<Iterator>();

    if (i->more())
      {
	i->get_key().push_value();
	i->get_value().push_value();
	i->next();
	return 2;
      }
    else
      {
	lua_pushnil(st);
	return 1;
      }

  } catch (String &e) {
    lua_pushstring(st, e.constData());
    lua_error(st);
  }

  std::abort();
}

int State::lua_cmd_each(lua_State *st)
{
  int idx = 1;

  if (lua_gettop(st) < 1)
    idx = LUA_GLOBALSINDEX;

  try {
    State		*this_ = get_this(st);
    Value		table(idx, this_);
    Iterator::ptr	i = table.new_iterator();

    lua_pushcfunction(st, lua_cmd_iterator);
    i->push_ud(st);
    lua_pushnil(st);

  } catch (String &e) {
    lua_pushstring(st, e.constData());
    lua_error(st);
  }

  return 3;
}

int State::lua_cmd_print(lua_State *st)
{
  try {
    State	*this_ = get_this(st);

    for (int i = 1; i <= lua_gettop(st); i++)
      {
	String s = Value::to_string_p(st, i, true);
	this_->output_str(s);
	this_->output_str("\n");
	qDebug("QtLua print:%s", s.constData());
      }

  } catch (String &e) {
    lua_pushstring(st, e.constData());
    lua_error(st);
  }

  return 0;
}

int State::lua_cmd_plugin(lua_State *st)
{
  try {
    State	*this_ = get_this(st);

    if (lua_gettop(st) < 1 || !lua_isstring(st, 1))
      {
	this_->output_str("Usage: plugin(\"library_filename_without_ext\")\n");
	return 0;
      }

    QTLUA_REFNEW(Plugin, String(lua_tostring(st, 1)) + Plugin::get_plugin_ext())->push_ud(st);
    return 1;

  } catch (String &e) {
    lua_pushstring(st, e.constData());
    lua_error(st);
  }

  return 0;
}

int State::lua_cmd_list(lua_State *st)
{
  try {
    State	*this_ = get_this(st);

    int idx = lua_gettop(st) > 0 ? 1 : LUA_GLOBALSINDEX;

    // display table object content
    const Value	t = Value(idx, this_);

    for (Value::const_iterator i = t.begin(); i != t.end(); i++)
      {
	try {
	  this_->output_str(String("\033[18m") + i.value().type_name_u() + "\033[2m " +
			    i.key().to_string_p(false) + " = " + i.value().to_string_p(true) + "\n");
	} catch (String &e) {
	  this_->output_str(String("\033[18m[Error]\033[2m " +
				   i.key().to_string_p(false) + " = " + e + "\n"));
	}
      }

  } catch (String &e) {
    lua_pushstring(st, e.constData());
    lua_error(st);
  }

  return 0;
}

int State::lua_cmd_help(lua_State *st)
{
  State *this_ = get_this(st);

  if (lua_gettop(st) < 1)
    {
      this_->output_str("Usage: help(function)\n");
      return 0;
    }

  Value v(1, this_);

  if (v.type() == Value::TUserData)
    {
      Function::ptr cmd = v.to_userdata().dynamiccast<Function>();

      if (cmd.valid())
	{
	  this_->output_str(cmd->get_help() + "\n");
	  return 0;
	}
    }

  this_->output_str("Help is only available for QtLua::Function objects\n");
  return 0;
}

// lua item metatable methods

#define LUA_META_2OP_FUNC(n, op)					\
									\
int State::lua_meta_item_##n(lua_State *st)				\
{									\
  int		x = lua_gettop(st);					\
  State		*this_ = get_this(st);					\
									\
  try {									\
    Value	a(1, this_);						\
    Value	b(2, this_);						\
									\
    if (a.type() == Value::TUserData)					\
      a.to_userdata()->meta_operation(*this_, op, a, b).push_value();	\
    else if (b.type() == Value::TUserData)				\
      b.to_userdata()->meta_operation(*this_, op, a, b).push_value();	\
    else								\
      std::abort();							\
									\
  } catch (String &e) {							\
    lua_pushstring(st, e.constData());					\
    lua_error(st);							\
  }									\
									\
  return lua_gettop(st) - x;						\
}

#define LUA_META_1OP_FUNC(n, op)					\
									\
int State::lua_meta_item_##n(lua_State *st)				\
{									\
  int		x = lua_gettop(st);					\
  State		*this_ = get_this(st);					\
									\
  try {									\
    Value	a(1, this_);						\
									\
     a.to_userdata()->meta_operation(*this_, op, a, a).push_value();	\
									\
  } catch (String &e) {							\
    lua_pushstring(st, e.constData());					\
    lua_error(st);							\
  }									\
									\
  return lua_gettop(st) - x;						\
}

LUA_META_2OP_FUNC(add, Value::OpAdd)
LUA_META_2OP_FUNC(sub, Value::OpSub)
LUA_META_2OP_FUNC(mul, Value::OpMul)
LUA_META_2OP_FUNC(div, Value::OpDiv)
LUA_META_2OP_FUNC(mod, Value::OpMod)
LUA_META_2OP_FUNC(pow, Value::OpPow)
LUA_META_1OP_FUNC(unm, Value::OpUnm)
LUA_META_2OP_FUNC(concat, Value::OpConcat)
LUA_META_1OP_FUNC(len, Value::OpLen)
LUA_META_2OP_FUNC(eq, Value::OpEq)
LUA_META_2OP_FUNC(lt, Value::OpLt)
LUA_META_2OP_FUNC(le, Value::OpLe)

int State::lua_meta_item_index(lua_State *st)
{
  int		x = lua_gettop(st);
  State		*this_ = get_this(st);

  try {
    UserData::ptr ud = UserData::get_ud(st, 1);

    if (!ud.valid())
      throw String("Can not index null lua::userdata value.");

    Value	op(2, this_);

    Value v = ud->meta_index(*this_, op);
    assert(v._st->_lst == st);
    v.push_value();

  } catch (String &e) {
    lua_pushstring(st, e.constData());
    lua_error(st);
  }

  return lua_gettop(st) - x;
}

int State::lua_meta_item_newindex(lua_State *st)
{
  int		x = lua_gettop(st);
  State		*this_ = get_this(st);

  try {
    UserData::ptr ud = UserData::get_ud(st, 1);

    if (!ud.valid())
      throw String("Can not index null lua::userdata value.");

    Value	op1(2, this_);
    Value	op2(3, this_);

    ud->meta_newindex(*this_, op1, op2);

  } catch (String &e) {
    lua_pushstring(st, e.constData());
    lua_error(st);
  }

  return lua_gettop(st) - x;
}

int State::lua_meta_item_call(lua_State *st)
{
  int		n = lua_gettop(st);
  State		*this_ = get_this(st);

  try {
    UserData::ptr ud = UserData::get_ud(st, 1);

    if (!ud.valid())
      throw String("Can not call null lua::userdata value.");

    Value::List	args;

    for (int i = 2; i <= lua_gettop(st); i++)
      args.append(Value(i, this_));

    args = ud->meta_call(*this_, args);

    if (!lua_checkstack(st, args.size()))
      throw String("Unable to extend lua stack to handle % return values").arg(args.size());

    foreach(const Value &v, args)
      {
	assert(v._st->_lst == st);
	v.push_value();
      }

  } catch (String &e) {
    lua_pushstring(st, e.constData());
    lua_error(st);
  }

  return lua_gettop(st) - n;
}

int State::lua_meta_item_gc(lua_State *st)
{
  UserData::get_ud(st, 1).~Ref<UserData>();

  return 0;
}

/************************************************************************/

bool State::set_global_r(const String &name, const Value &value, int tblidx)
{
  int len = name.indexOf('.', 0);

  if (len < 0)
    {
      // set value in table if last
      lua_pushstring(_lst, name.constData());
      value.push_value();
      lua_settable(_lst, tblidx);

      return true;
    }
  else
    {
      // find intermediate value in path
      String prefix(name.mid(0, len));

      lua_pushstring(_lst, prefix.constData());
      lua_gettable(_lst, tblidx);

      if (lua_isnil(_lst, -1))
	{
	  // create intermediate table
	  lua_pop(_lst, 1);
	  lua_pushstring(_lst, prefix.constData());
	  lua_newtable(_lst);

	  if (set_global_r(name.mid(len + 1), value, lua_gettop(_lst)))
	    {
	      lua_settable(_lst, tblidx);
	      return true;
	    }
	  else
	    {
	      lua_pop(_lst, 2);
	      return false;
	    }
	}
      else if (lua_istable(_lst, -1))
	{
	  // use existing intermediate table
	  bool res = set_global_r(name.mid(len + 1), value, lua_gettop(_lst));
	  lua_pop(_lst, 1);
	  return res;
	}
      else
	{
	  // bad existing intermediate value
	  lua_pop(_lst, 1);
	  return false;
	}
    }
}

void State::set_global(const String &name, const Value &value)
{
  if (!set_global_r(name, value, LUA_GLOBALSINDEX))
    throw String("Unable to set lua global variable.");
}

void State::get_global_r(const String &name, Value &value, int tblidx) const
{
  int len = name.indexOf('.', 0);

  if (len < 0)
    {
      // get value from table if last
      lua_pushstring(_lst, name.constData());
      lua_gettable(_lst, tblidx);
      value = Value(-1, this);
      lua_pop(_lst, 1);
    }
  else
    {
      // find intermediate value in path
      String prefix(name.mid(0, len));

      lua_pushstring(_lst, prefix.constData());
      lua_gettable(_lst, tblidx);

      if (lua_istable(_lst, -1))
	{
	  get_global_r(name.mid(len + 1), value, lua_gettop(_lst));
	  lua_pop(_lst, 1);
	}

      lua_pop(_lst, 1);
    }
}

Value State::get_global(const String &path) const
{
  Value res(const_cast<State&>(*this));
  get_global_r(path, res, LUA_GLOBALSINDEX);
  return res;
}

Value State::operator[] (const Value &key) const
{
  key.push_value();
  lua_gettable(_lst, LUA_GLOBALSINDEX);
  Value res(-1, this);
  lua_pop(_lst, 1);
  return res;
}

ValueRef State::operator[] (const Value &key)
{
  return ValueRef(Value(LUA_GLOBALSINDEX, this), key);
}

State::State()
{
  assert(Value::TNone == LUA_TNONE);
  assert(Value::TNil == LUA_TNIL);
  assert(Value::TBool == LUA_TBOOLEAN);
  assert(Value::TNumber == LUA_TNUMBER);
  assert(Value::TString == LUA_TSTRING);
  assert(Value::TTable == LUA_TTABLE);
  assert(Value::TFunction == LUA_TFUNCTION);
  assert(Value::TUserData == LUA_TUSERDATA);

  _lst = lua_open();

  if (!_lst)
    throw std::bad_alloc();

  //lua_atpanic(_lst, lua_panic);

  // creat metatable for UserData events

  lua_pushlightuserdata(_lst, &_key_item_metatable);
  lua_newtable(_lst);

#define LUA_META_BIND(n)			\
  lua_pushstring(_lst, "__" #n);			\
  lua_pushcfunction(_lst, lua_meta_item_##n);	\
  lua_rawset(_lst, -3);

  LUA_META_BIND(add);
  LUA_META_BIND(sub);
  LUA_META_BIND(mul);
  LUA_META_BIND(div);
  LUA_META_BIND(mod);
  LUA_META_BIND(pow);
  LUA_META_BIND(unm);
  LUA_META_BIND(concat);
  LUA_META_BIND(len);
  LUA_META_BIND(eq);
  LUA_META_BIND(lt);
  LUA_META_BIND(le);
  LUA_META_BIND(index);
  LUA_META_BIND(newindex);
  LUA_META_BIND(call);
  LUA_META_BIND(gc);

  lua_rawset(_lst, LUA_REGISTRYINDEX);

  // pointer to this

  lua_pushlightuserdata(_lst, &_key_this);
  lua_pushlightuserdata(_lst, this);
  lua_rawset(_lst, LUA_REGISTRYINDEX);
}

State::~State()
{
  // disconnect all Qt slots while associated Value objects are still valid
  foreach(QObjectWrapper *w, _whash)
    w->_lua_disconnect_all();

  // lua state close
  lua_close(_lst);

  // wipe QObjectWrapper objects
  wrapper_hash_t::const_iterator i;

  while ((i = _whash.begin()) != _whash.end())
    i.value()->_drop();
}

int State::lua_panic(lua_State *st)
{
  String err(lua_tostring(st, -1));
  lua_pop(st, 1);
  throw err;
}

Value State::eval_expr(bool use_lua, const String &expr)
{
  // Use lua to transform user input to lua value
  if (use_lua)
    {
      Value::List res = exec_statements(String("return ") + expr);

      if (res.empty())
	throw String("lua expression `%' returned no value").arg(expr);

      return res[0];
    }

    // Do not use lua, only handle string and number cases
    else
      {
	bool ok = false;
	double number = expr.toDouble(&ok);

	if (ok)
	  return Value(*this, number);
	else
	  {
	    // exprip double quotes if any
	    if (expr.size() > 1 && expr.startsWith('"') && expr.endsWith('"'))
	      return Value(*this, String(expr.mid(1, expr.size() - 2)));
	    else
	      return Value(*this, expr);
	  }
      }
}

struct lua_reader_state_s
{
  QIODevice *_io;
  QByteArray _read_buf;
};

static const char * lua_reader(lua_State *st, void *data, size_t *size)
{
  struct lua_reader_state_s *rst = (struct lua_reader_state_s *)data;

  rst->_read_buf = rst->_io->read(4096);
  *size = rst->_read_buf.size();
  return rst->_read_buf.constData();
}

Value::List State::exec_chunk(QIODevice &io)
{
  struct lua_reader_state_s rst;
  rst._io = &io;

  if (lua_load(_lst, &lua_reader, &rst, ""))
    {
      String err(lua_tostring(_lst, -1));
      lua_pop(_lst, 1);
      throw err;
    }

  int oldtop = lua_gettop(_lst);

  if (lua_pcall(_lst, 0, LUA_MULTRET, 0))
    {
      String err(lua_tostring(_lst, -1));
      lua_pop(_lst, 1);
      throw err;
    }

  Value::List res;
  for (int i = oldtop; i <= lua_gettop(_lst); i++)
    res += Value(i, this);
  lua_pop(_lst, lua_gettop(_lst) - oldtop + 1);

  return res;
}

Value::List State::exec_statements(const String & statement)
{
  if (luaL_loadbuffer(_lst, statement.constData(), statement.size(), ""))
    {
      String err(lua_tostring(_lst, -1));
      lua_pop(_lst, 1);
      throw err;
    }

  int oldtop = lua_gettop(_lst);

  if (lua_pcall(_lst, 0, LUA_MULTRET, 0))
    {
      String err(lua_tostring(_lst, -1));
      lua_pop(_lst, 1);
      throw err;
    }

  Value::List res;
  for (int i = oldtop; i <= lua_gettop(_lst); i++)
    res += Value(i, this);
  lua_pop(_lst, lua_gettop(_lst) - oldtop + 1);

  return res;
}

void State::exec(const QString &statement)
{
  try {
    exec_statements(statement);

  } catch (QtLua::String &e) {
    output_str(String("\033[7merror\033[2m: ") + e.constData() + "\n");
  }

  gc_collect();
}

void State::gc_collect()
{
#ifdef HAVE_LUA_GC
  lua_gc(_lst, LUA_GCCOLLECT, 0);
#else
  lua_setgcthreshold(_lst, 0);
#endif
}

void State::reg_c_function(const char *name, lua_CFunction f)
{
  lua_pushstring(_lst, name);
  lua_pushcfunction(_lst, f);
  lua_rawset(_lst, LUA_GLOBALSINDEX);
}

State * State::get_this(lua_State *st)
{
  void *data;

  lua_pushlightuserdata(st, &_key_this);
  lua_rawget(st, LUA_REGISTRYINDEX);
  data = lua_touserdata(st, -1);
  lua_pop(st, 1);

  return static_cast<State*>(data);
}

#define QTLUA_LUA_CALL(st, f)		\
  lua_pushcfunction(st, f);		\
  lua_call(st, 0, 0);

void State::openlib(Library lib)
{
  switch (lib)
    {
    case BaseLib:
      QTLUA_LUA_CALL(_lst, luaopen_base);
      return;
    case PackageLib:
#ifdef HAVE_LUA_PACKAGELIB
      QTLUA_LUA_CALL(_lst, luaopen_package);
#endif
      return;
    case StringLib:
      QTLUA_LUA_CALL(_lst, luaopen_string);
      return;
    case TableLib:
      QTLUA_LUA_CALL(_lst, luaopen_table);
      return;
    case MathLib:
      QTLUA_LUA_CALL(_lst, luaopen_math);
      return;
    case IoLib:
      QTLUA_LUA_CALL(_lst, luaopen_io);
      return;
#ifdef HAVE_LUA_OSLIB
    case OsLib:
      QTLUA_LUA_CALL(_lst, luaopen_os);
      return;
#endif
    case DebugLib:
      QTLUA_LUA_CALL(_lst, luaopen_debug);
      return;
    case AllLibs:
#ifdef HAVE_LUA_OPENLIBS
      luaL_openlibs(_lst);
#else
      QTLUA_LUA_CALL(_lst, luaopen_base);
      QTLUA_LUA_CALL(_lst, luaopen_string);
      QTLUA_LUA_CALL(_lst, luaopen_table);
      QTLUA_LUA_CALL(_lst, luaopen_math);
      QTLUA_LUA_CALL(_lst, luaopen_io);
      QTLUA_LUA_CALL(_lst, luaopen_debug);
#endif
      qtluaopen_qt(*this);
    case QtLuaLib:
      reg_c_function("print", lua_cmd_print);
      reg_c_function("list", lua_cmd_list);
      reg_c_function("each", lua_cmd_each);
      reg_c_function("help", lua_cmd_help);
      reg_c_function("plugin", lua_cmd_plugin);
      return;
    case QtLib:
      qtluaopen_qt(*this);
      return;
    }
}

void State::lua_do(void (*func)(lua_State *st))
{
  func(_lst);
}

void State::fill_completion_list_r(String &path, const String &prefix,
				   QStringList &list, const Value &tbl,
				   int &cursor_offset)
{
  int len = strcspn(prefix.constData(), ":.");

  if (len == prefix.size())
    {
      String lastentry, tpath(path);

      // enumerate table object
      for (Value::const_iterator i = tbl.begin(); i != tbl.end(); i++)
	{
	  const Value &k = i.key();

	  // ignore non string keys
	  if (k.type() != Value::TString)
	    continue;

	  String entry = k.to_string();

	  if (entry.startsWith(prefix))
	    {
	      try {
		const Value &v = i.value();

		// add operator for known types
		switch (v.type())
		  {
		  case Value::TTable:

		    v.push_value();
		    lua_pushnil(_lst);
		    if (lua_next(_lst, -2))
		      {
			if (lua_type(_lst, -2) == Value::TString)
			  entry += ".";
			else
			  {
			    entry += "[]";
			    cursor_offset = -1;
			  }
			lua_pop(_lst, 2);
		      }
		    lua_pop(_lst, 1);

		    break;

		  case Value::TFunction:
		    entry += "()";
		    cursor_offset = -1;
		    break;

		  case Value::TUserData:
		    v.to_userdata()->completion_patch(tpath, entry, cursor_offset);
		  default:
		    break;
		  }
	      } catch (String &e) {
		/* can not access value */
	      }

	      lastentry = entry;
	      list.push_back(path.to_qstring() + entry.to_qstring());
	    }
	}

      // apply path patch only if single match
      if (list.size() == 1)
	list[0] = tpath.to_qstring() + lastentry.to_qstring();
    }

  if (list.empty())
    {
      // find intermediate values in path
      String next = prefix.mid(0, len);

      try {
	path += next;
	if (len < prefix.size())
	  path += prefix[len];
	fill_completion_list_r(path, prefix.mid(len + 1), list, tbl[next], cursor_offset);
      } catch (...) {
      }

    }
}

void State::fill_completion_list(const QString &prefix, QStringList &list, int &cursor_offset)
{
  String path;

  fill_completion_list_r(path, prefix, list, Value(LUA_GLOBALSINDEX, this), cursor_offset);
}

}

