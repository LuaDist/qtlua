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


#include <QtLua/ListItem>
#include <QtLua/String>
#include <QtLua/Item>
#include <QtLua/ItemModel>

namespace QtLua {

bool Item::in_parent_path(Item *item)
{
  Item *	my_path = this;

  while (my_path)
    {
      if (item == my_path)
	return true;
      my_path = my_path->_parent;
    }

  return false;
}

Item::Item(const String &name)
  : UserData(), _name(name), _parent(0), _model(0), _row(-1)
{
}

Item::Item(const Item &item)
  : UserData(item), _name(item._name), _parent(0), _model(0), _row(-1)
{
}

Item::~Item()
{
  assert(!_parent);
  assert(!_model);
}

void Item::move(const Ref<ListItem> &parent)
{
  if (_parent)
    remove();
  insert(parent);
}

void Item::insert(const Ref<ListItem> &parent)
{
  assert(!_parent);

  _parent = parent.ptr();
  set_model(parent->_model);

  _row = _parent->get_child_count();

  if (_model)
    emit _model->layoutAboutToBeChanged();

  _parent->insert(this, _row);
  _parent->insert_name(this);

  if (_model)
    emit _model->layoutChanged();
}

void Item::remove()
{
  assert(_parent);
  Item::ptr this_ = *this;

  ItemModel *model = _model;

  if (model)
    emit model->layoutAboutToBeChanged();

  _parent->remove(this);
  set_model(0);

  if (model)
    emit model->layoutChanged();

  _parent = 0;
  _row = -1;
}

void Item::set_model(ItemModel* model)
{
  _model = model;
}

void Item::set_name(const String &name)
{
  if (_parent)
    _parent->remove_name(this);

  _name = name;

  if (_parent)
    _parent->insert_name(this);

  if (_model)
    emit _model->dataChanged(model_index(), model_index());
}

void Item::data_changed(int column) const
{
  if (_model)
    emit _model->dataChanged(model_index(column), model_index(column));  
}

Item * Item::get_child_row(int row) const
{
  return 0;
}

inline int Item::get_child_count() const
{
  return 0;
}

bool Item::is_move_allowed() const
{
  return is_rename_allowed();
}

bool Item::is_rename_allowed() const
{
  return true;
}

bool Item::is_remove_allowed() const
{
  return true;
}

bool Item::is_replace_allowed() const
{
  return is_remove_allowed();
}

QIcon &	Item::get_icon() const
{
  static QIcon i = QIcon();

  return i;
}

bool Item::set_data(int column, int role)
{
  return false;
}

QVariant Item::get_data(int column, int role) const
{
  return QVariant();
}

Qt::ItemFlags Item::get_flags(int column) const
{
  return Qt::ItemIsEnabled;
}

}

