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


#include <QtLua/Value>
#include <QtLua/ListItem>
#include <QtLua/ItemModel>
#include <QtLua/String>

#include <internal/ListIterator>

namespace QtLua {

Value ListItem::meta_operation(State &ls, Value::Operation op, const Value &a, const Value &b)
{
  switch (op)
    {
    case Value::OpLen:
      return Value(ls, get_child_count());
    default:
      return UserData::meta_operation(ls, op, a, b);
    }
}

void ListItem::meta_newindex(State &ls, const Value &key, const Value &value)
  
{
  switch (value.type())
    {
    case Value::TNil: {
      Item::ptr kbml = get_child(key.to_string());

      if (kbml.valid())
	{
	  if (!kbml->is_remove_allowed())
	    throw String("Not allowed to remove '%' item from list.").arg(kbml->get_name());

	  kbml->remove();
	}
    } break;

    case Value::TUserData: {
      Item::ptr kbml = value.to_userdata_cast<Item>();

      if (in_parent_path(kbml.ptr()))
	throw String("Item '%' can not have one of its parent as child.").arg(kbml->get_name());

      // remove item with same key if it exist
      Item::ptr old = get_child(key.to_string());

      if (old.valid())
	{
	  if (!old->is_replace_allowed())
	    throw String("Overwriting '%' item with '%' is not allowed.")
	      .arg(old->get_name()).arg(kbml->get_name());
	  old->remove();
	}

      if (kbml->_parent == this)
	{
	  if (!kbml->is_rename_allowed())
	    throw String("Renaming '%' item is not allowed.").arg(kbml->get_name());

	  // just rename
	  kbml->set_name(key.to_string());
	}
      else
	{
	  if (!kbml->is_move_allowed())
	    throw String("Moving '%' item is not allowed.").arg(kbml->get_name());

	  if (!accept_child(kbml))
	    throw String("Item '%' doesn't accept '%' as child.")
	      .arg(get_name()).arg(kbml->get_name());

	  // remove item from other tree if needed
	  if (kbml->_parent)
	    kbml->remove();

	  // rename and insert item
	  kbml->set_name(key.to_string());
	  kbml->insert(*this);
	}

      } break;

    default:
      throw String("Item list can not store a % value.").arg(value.type_name_u());
    }
};

Value ListItem::meta_index(State &ls, const Value &key)
  
{
  Item::ptr item = get_child(key.to_string());

  if (item.valid())
    return Value(ls, item);
  else
    return Value(ls);
}

bool ListItem::meta_contains(State &ls, const Value &key)
{
  try {
    return get_child(key.to_string()).valid();
  } catch (String &e) {
    return false;
  }
}

Iterator::ptr ListItem::new_iterator(State &ls)
{
  return QTLUA_REFNEW(ListIterator, ls, ListItem::ptr(*this));
}

bool ListItem::support(Value::Operation c) const
{
  switch (c)
    {
    case Value::OpIndex:
    case Value::OpNewindex:
    case Value::OpIterate:
    case Value::OpLen:
      return true;
    default:
      return false;
    }
}

void ListItem::change_indexes(int first)
{
  for (int i = first; i < get_child_count(); i++)
    {
      const Item::ptr &item = _child_list[i];

      if (_model)
	{
	  QModelIndex old(item->model_index());
	  _child_list[i]->set_row(i);
	  _model->changePersistentIndex(old, item->model_index());
	}
      else
	{
	  _child_list[i]->set_row(i);
	}
    }
}

void ListItem::remove(Item *item)
{
  assert(item->get_parent() == this);

  _child_hash.remove(item->get_name());
  _child_list.removeAt(item->get_row());
  if (_model)
    _model->changePersistentIndex(item->model_index(), QModelIndex());
  change_indexes(item->get_row());
}

void ListItem::insert(Item *item, int row)
{
  _child_list.insert(row, *item);
  item->set_row(row);
  change_indexes(row + 1);
}

void ListItem::insert_name(Item *item)
{
  QString name = item->_name;

  if (name.size() == 0)
    name += "noname";
  else
    name = name.replace(QRegExp("[^A-Za-z0-9_]"), "_");

  if (_child_hash.contains(name))
    {
      String basename = name.remove(QRegExp("_[0-9]+$"));
      do {
	name = QString().sprintf("%s_%u", basename.constData(), _id_counter++);
      } while (_child_hash.contains(name));
    }

  item->_name = name;

  _child_hash.insert(name, item);
}

bool ListItem::accept_child(const Item::ptr &item) const
{
  return true;
}

int ListItem::get_column_count() const
{
  return 1;
}

ListItem::ListItem()
  : _id_counter(1)
{
}

ListItem::~ListItem()
{
  foreach(Item::ptr tmp, _child_list)
    {
      assert(!tmp->_model);
      tmp->_parent = 0;
      tmp->_row = -1;
    }
}

void ListItem::set_model(ItemModel* model)
{
  if (_model == model)
    return;

  Item::set_model(model);

  foreach(Item::ptr tmp, _child_list)
    tmp->set_model(model);
}

void ListItem::completion_patch(String &path, String &entry, int &offset)
{
  entry += ".";
}

}

