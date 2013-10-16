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

#ifndef QTLUALISTPROXY_HH_
#define QTLUALISTPROXY_HH_

#include <QPointer>

#include "qtluauserdata.hh"
#include "qtluaiterator.hh"

namespace QtLua {

  /**
   * @short QList read only access wrapper for lua script
   * @header QtLua/QListProxy
   * @module {Container proxies}
   *
   * This template class may be used to expose an attached @ref QList
   * container object to lua script for read access. The @ref
   * QListProxy class may be used for read/write access.
   *
   * See @ref QListProxy class documentation for details and examples.
   */

template <class Container>
class QListProxyRo : public UserData
{
public:
  QTLUA_REFTYPE(QListProxyRo);

  /** Create a @ref QListProxy object with no attached container */
  QListProxyRo();
  /** Create a @ref QListProxy object and attach given container */
  QListProxyRo(Container &list);

  /** Attach or detach container. argument may be NULL */
  void set_container(Container *list);

  Value meta_index(State &ls, const Value &key);
  bool meta_contains(State &ls, const Value &key);
  Ref<Iterator> new_iterator(State &ls);
  Value meta_operation(State &ls, Value::Operation op, const Value &a, const Value &b);
  bool support(Value::Operation c) const;

private:

  String get_type_name() const;

  /**
   * @short QListProxyRo iterator class
   * @internal
   */
  class ProxyIterator : public Iterator
  {
  public:
    QTLUA_REFTYPE(ProxyIterator);
    ProxyIterator(State *ls, const Ref<QListProxyRo> &proxy);

  private:
    bool more() const;
    void next();
    Value get_key() const;
    Value get_value() const;
    ValueRef get_value_ref();

    QPointer<State> _ls;
    typename QListProxyRo::ptr _proxy;
    typename Container::const_iterator _it;
    unsigned int _i;
  };

protected:
  Container *_list;
};

  /**
   * @short QList access wrapper for lua script
   * @header QtLua/QListProxy
   * @module {Container proxies}
   *
   * This template class may be used to expose an attached @ref QList
   * container object to lua script for read and write access. The
   * @ref QListProxyRo class may be used for read only access.
   *
   * Containers may be attached and detached from the wrapper object
   * to solve cases where we want to destroy the container when lua
   * still holds references to the wrapper object. When no container
   * is attached access will raise an error.
   *
   * First entry has index 1. Lua @tt nil value is returned if no such
   * entry exists on table read. A @tt nil value write will delete
   * entry at given index. Write access at list size + 1 append a new
   * entry to the list.
   *
   * Lua operator @tt # returns the container entry count. Lua
   * operator @tt - returns a lua table copy of the container.
   *
   * The following example show how a @ref QList object can be
   * accessed from both C++ and lua script directly:
   *
   * @example examples/cpp/proxy/qlistproxy_string.cc:1|2|3
   */

template <class Container>
class QListProxy : public QListProxyRo<Container>
{
  using QListProxyRo<Container>::_list;

public:
  QTLUA_REFTYPE(QListProxy);

  /** Create a @ref QListProxy object */
  QListProxy();
  /** Create a @ref QListProxy object */
  QListProxy(Container &list);

  void meta_newindex(State &ls, const Value &key, const Value &value);
  bool support(enum Value::Operation c);
};

}

#endif

