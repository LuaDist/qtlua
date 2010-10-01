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

    Copyright (C) 2009, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/

#include <QtLua/ItemSelectionModel>
#include <QtLua/ItemModel>
#include <QtLua/Item>
#include <QtLua/ListItem>

namespace QtLua {

  void ItemSelectionModel::select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command)
  {
    select(QItemSelection(index, index), command);
  }

  void ItemSelectionModel::select_childs(const QModelIndex &index, QItemSelection &selection)
  { 
    Item *i = static_cast<Item*>(index.internalPointer());

    if (ListItem *l = dynamic_cast<ListItem*>(i))
      foreach(const Item::ptr &c, l->get_list())
	{
	  selection.select(c->model_index(), c->model_index());
	  select_childs(c->model_index(), selection);
	}
  }

  void ItemSelectionModel::select_parents(const QModelIndex &index, QItemSelection &selection)
  {
    Item *i = static_cast<Item*>(index.internalPointer());

    while (Item *p = i->get_parent())
      {
	selection.select(p->model_index(), p->model_index());
	i = p;
      }
  }

  void ItemSelectionModel::select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command)
  {
    if ((command & (QItemSelectionModel::Select | QItemSelectionModel::Toggle)) &&
	!(command & QItemSelectionModel::Clear))
      {
	QItemSelection sel;

	sel.merge(selection, QItemSelectionModel::ClearAndSelect);

	// Remove all selected child items of selected parents from new selection
	foreach (const QModelIndex &index, selection.indexes())
	  {
	    QItemSelection rm;

	    select_childs(index, rm);
	    sel.merge(rm, QItemSelectionModel::Deselect);
	  }

	// Deselect all parents and childs items of newly selected items
	QItemSelection desel;

	foreach (const QModelIndex &index, sel.indexes())
	  {
	    select_childs(index, desel);
	    select_parents(index, desel);
	  }

	// FIXME QItemSelectionModel::select is called twice
	QItemSelectionModel::select(sel, command);
	QItemSelectionModel::select(desel, QItemSelectionModel::Deselect);
      }
    else
      {
	QItemSelectionModel::select(selection, command);
      }
  }

}

