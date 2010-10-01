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

#ifndef QTLUAUSEROBJECT_HH_
#define QTLUAUSEROBJECT_HH_

#include "qtluauserdata.hh"
#include "qtluaiterator.hh"

namespace QtLua {

  /**
   * @short Lua userdata objects with properties
   * @header QtLua/UserObject
   * @module {Base}
   *
   * This base class can be used to create C++ objects with named
   * properties accessible from lua script. This is a lightweight
   * alternative to writting a @ref QObject based class when only @tt
   * set/get properties mechanism is needed.
   *
   * Each property can have a @tt set and @tt get
   * accessor functions registered through a static const array.
   * Property member and accessor functions can be user defined or declared
   * using the @ref #QTLUA_PROPERTY_GET , @ref #QTLUA_PROPERTY_SET ,
   * @ref #QTLUA_PROPERTY_ACCESSORS or  @ref #QTLUA_PROPERTY macros as shown
   * on the example below :
   *
   * @example examples/cpp/userdata/userobject.cc:1
   */
  template <class T>
  class UserObject : public UserData
  {
  public:
    QTLUA_REFTYPE(UserObject);

  private:
    /** @internal */
    class UserObjectIterator : public Iterator
    {
    public:
      QTLUA_REFTYPE(UserObjectIterator);
      UserObjectIterator(State &ls, const Ref<UserObject> &obj);

    private:
      bool more() const;
      void next();
      Value get_key() const;
      Value get_value() const;
      ValueRef get_value_ref();

      State &_ls;
      Ref<UserObject> _obj;
      size_t _index;
    };

    int get_entry(const String &name);

  public:

    /** Property member entry */
    struct member_s
    {
      const char *name;		//< Lua property name
      void (T::*set)(State &ls, const Value &value); //< Pointer to property set accesor function for lua
      Value (T::*get)(State &ls); //< Pointer to property get accesor function for lua
    };

    Value meta_index(State &ls, const Value &key);
    void meta_newindex(State &ls, const Value &key, const Value &value);
    Ref<Iterator> new_iterator(State &ls);
    bool support(Value::Operation c) const;

    /** 
     * Define a simple inline get accessor function for the specified member
     * @showcontent
     */
#define QTLUA_PROPERTY_GET(member)		\
  inline QtLua::Value get_##member(QtLua::State &ls)	\
  {						\
    return QtLua::Value(ls, member);		\
  }

    /** 
     * Define a simple inline set accessor function for the specified member
     * @showcontent
     */
#define QTLUA_PROPERTY_SET(member)				\
  inline void set_##member(QtLua::State &ls, const QtLua::Value &value)\
  {								\
    member = value;						\
  }

    /** 
     * Define simple inline accessors function for the specified member
     * @showcontent
     */
#define QTLUA_PROPERTY_ACCESSORS(member)	\
  QTLUA_PROPERTY_GET(member)			\
  QTLUA_PROPERTY_SET(member)

    /** 
     * Declare a member of given type and define a simple inline accessors
     * function for the specified member
     * @showcontent
     */
#define QTLUA_PROPERTY(type, member)		\
  type member;					\
  QTLUA_PROPERTY_ACCESSORS(member);

    /**
     * Property table entry with get and set accessors
     */
#define QTLUA_PROPERTY_ENTRY(class_, member)	\
  { #member, &class_::set_##member, &class_::get_##member }

    /**
     * Property table entry with get accessor only
     */
#define QTLUA_PROPERTY_ENTRY_GET(class_, member)	\
  { #member, 0, &class_::get_##member }

    /**
     * Property table entry with set accessor only
     */
#define QTLUA_PROPERTY_ENTRY_SET(class_, member)	\
  { #member, &class_::set_##member, 0 }

  };
}

#endif

