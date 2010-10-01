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

#ifndef QTLUAITEM_HXX_
#define QTLUAITEM_HXX_

#include <cassert>

#include "qtluauserdata.hxx"
#include "qtluaitemmodel.hh"

namespace QtLua {

QModelIndex Item::model_index() const
{
  assert(_model);
  return _model->createIndex(_row, 0, (void*)this);
}

void Item::set_row(int row)
{
  _row = row;
}

int Item::get_row() const
{
  return _row;
}

ListItem * Item::get_parent() const
{
  return _parent;
}

const String & Item::get_name() const
{
  return _name;
}

ItemModel * Item::get_model() const
{
  return _model;
}

}

#endif

