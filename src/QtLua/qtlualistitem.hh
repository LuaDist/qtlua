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


#ifndef QTLUALISTITEM_HH_
#define QTLUALISTITEM_HH_

#include <QHash>
#include <QList>

#include "qtluaitem.hh"
#include "qtluaiterator.hh"

namespace QtLua {

class ItemModel;

  /**
   * @short Qt Model/View list item class
   * @header QtLua/ListItem
   * @module {Model/View}
   *
   * This class together with the @ref Item and @ref ItemModel classes
   * enable easy use of list or hierarchical data structures that can be
   * viewed and modified from lua script, Qt view widgets and C++
   * code.
   *
   * @ref ListItem objects are @ref Item objects with pointer list to
   * children objects. It can be accessed as tables from lua script.
   *
   * See @ref ItemModel for example.
   */

class ListItem : public Item
{
  friend class Item;
  friend class ItemModel;

public:

  QTLUA_REFTYPE(ListItem);

  ListItem();
  ~ListItem();

  /** Find a child item from name. */
  inline Item::ptr get_child(const String &name) const;

  /** Get child items list */
  inline const QList<Item::ptr> & get_list() const;

  /** Get number of childs */
  inline int get_child_count() const;

protected:

  /**
   * This function can be reimplemented to allow or deny items
   * membership when inserted from lua script or Qt view.
   *
   * @return true if item is allowed to be a child member.
   */
  virtual bool accept_child(const Item::ptr &item) const;
  virtual Value meta_operation(State &ls, Value::Operation op, const Value &a, const Value &b);
  virtual bool support(Value::Operation c) const;

private:

  void meta_newindex(State &ls, const Value &key, const Value &value);
  Value meta_index(State &ls, const Value &key);
  Iterator::ptr new_iterator(State &ls);
  void completion_patch(String &path, String &entry, int &offset);

  void set_model(ItemModel* model);

  void change_indexes(int first);
  void insert(Item *item, int row);
  void insert_name(Item *item);
  void remove(Item *item);
  inline void remove_name(Item *item);

  QHash<String,Item*> _child_hash;
  QList<Item::ptr> _child_list;
  int _id_counter;
};

}

#endif

