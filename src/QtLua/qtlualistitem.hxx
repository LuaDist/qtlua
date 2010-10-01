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


#ifndef QTLUALISTITEM_HXX_
#define QTLUALISTITEM_HXX_

#include <cassert>

#include "qtluaitem.hxx"

namespace QtLua {

Item::ptr ListItem::get_child(const String &name) const
{
  Item *i = _child_hash.value(name, 0);
  return i ? *i : Item::ptr();
}

const QList<Item::ptr> & ListItem::get_list() const
{
  return _child_list;
}

int ListItem::get_child_count() const
{
  return _child_list.count();
}

void ListItem::remove_name(Item *item)
{
  assert(item->get_parent() == this);
  _child_hash.remove(item->get_name());
}

}

#endif

