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


#include <QIcon>

#include <QtLua/Item>
#include <QtLua/ListItem>
#include <QtLua/ItemModel>

namespace QtLua {

ItemModel::ItemModel(ListItem::ptr root, QObject *parent)
  : QAbstractItemModel(parent),
    _root(root)
{
  assert(!_root->get_model());
  _root->set_model(this);
}

ItemModel::~ItemModel()
{
  _root->set_model(0);
}

Item::ptr ItemModel::get_item(const QModelIndex &index)
{
  return *static_cast<Item*>(index.internalPointer());
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  Item *item = static_cast<Item*>(index.internalPointer());

  if (index.column())
    return item->get_data(index.column(), role);

  switch (role)
    {
    case Qt::DisplayRole:
      return QVariant(item->get_name());

    case Qt::DecorationRole:
      return item->get_icon();

    default:
      return QVariant();
    }
}

Qt::ItemFlags ItemModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::ItemIsDropEnabled;

  Item *item = static_cast<Item*>(index.internalPointer());

  if (index.column())
    return item->get_flags(index.column());

  Qt::ItemFlags res = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (item->is_rename_allowed())
    res |= Qt::ItemIsEditable;

  if (item->is_move_allowed())
    res |= Qt::ItemIsDragEnabled;

  if (dynamic_cast<ListItem*>(item))
    res |= Qt::ItemIsDropEnabled;

  return res;
}

Item::ptr ItemModel::from_mimedata(const QMimeData *data)
{
  return Item::ptr();
}

bool ItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
			     int row, int column, const QModelIndex &parent)
{
  ListItem *pi = parent.isValid()
    ? static_cast<ListItem*>(parent.internalPointer())
    : _root.ptr();

  if (!pi)		// parent is not a ListItem ?
    return false;

  if (row < 0)
    row = 0;

  const ItemQMimeData *d = dynamic_cast<const ItemQMimeData*>(data);

  if (!d)
    // external mime object drop
    {
      Item::ptr i = from_mimedata(data);

      if (i.valid())
	{
	  i->insert(*pi);
	  return true;
	}
    }

  else
    // internal existing items drop
    {
      switch (action)
	{
	case Qt::IgnoreAction:
	  return true;

	case Qt::MoveAction:
	  emit layoutAboutToBeChanged();

	  foreach(Item::ptr i, d->_itemlist)
	    {
	      if (!i->is_move_allowed() || !pi->accept_child(i))
		continue;

	      // handle case where deleted item shifts row offset
	      if (pi == i->_parent && row > i->_row)
		row--;

	      i->_parent->remove(i.ptr());
	      i->_parent = pi;
	      assert(row <= pi->get_child_count());
	      i->_row = row;
	      pi->insert(i.ptr(), row++);
	      pi->insert_name(i.ptr());
	    }

	  emit layoutChanged();
	  return true;

	default:
	  break;
	}
    }

  return false;
}

QStringList ItemModel::mimeTypes() const
{
  QStringList types;
  types << QString("application/qtlua.item");
  return types;
}

QMimeData * ItemModel::mimeData(const QModelIndexList &indexes) const
{
  ItemQMimeData *r = new ItemQMimeData();

  foreach(const QModelIndex &index, indexes)
    r->_itemlist.push_back(*static_cast<Item*>(index.internalPointer()));

  r->setData("application/qtlua.item", "");

  return r;
}

Qt::DropActions ItemModel::supportedDropActions() const
{
  return Qt::MoveAction /* | Qt::CopyAction*/;
}

QVariant ItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

QModelIndex ItemModel::index(int row, int column, const QModelIndex &parent) const
{
  ListItem *p;

  if (!parent.isValid())
    p = _root.ptr();
  else
    p = static_cast<ListItem*>(parent.internalPointer());

  int c = p->get_column_count();
  assert(c > 0);

  if (column < c && row < p->_child_list.size())
    return createIndex(row, column, p->_child_list[row].ptr());
  else
    return QModelIndex();
}

QModelIndex ItemModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  Item *c = static_cast<Item*>(index.internalPointer());
  ListItem *p = c->get_parent();

  if (!p || p == _root.ptr())
    return QModelIndex();

  return createIndex(p->get_row(), 0, p);
}

int ItemModel::rowCount(const QModelIndex &parent) const
{
  Item *p;

  if (!parent.isValid())
    p = _root.ptr();
  else
    p = static_cast<Item*>(parent.internalPointer());

  return p->get_child_count();
}

int ItemModel::columnCount(const QModelIndex &parent) const
{
  ListItem *p;

  if (!parent.isValid())
    p = _root.ptr();
  else
    p = static_cast<ListItem*>(parent.internalPointer());

  return p->get_column_count();
}

bool ItemModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  Item *item = static_cast<Item*>(index.internalPointer());

  if (index.column())
    return item->set_data(index.column(), role);

  switch (role)
    {
    case Qt::EditRole:
      item->set_name(value.toString());
      return true;

    default:
      return false;
    }
}

Value ItemModel::get_selection(State &ls, const QAbstractItemView &view)
{
  assert(dynamic_cast<ItemModel*>(view.model()));
  QItemSelectionModel *sm = view.selectionModel();

  if (!sm || !sm->hasSelection())
    return Value(ls);

  Value table(ls, Value::TTable);

  int i = 1;
  foreach(const QModelIndex &index, sm->selectedIndexes())
    {
      Value entry(ls, *static_cast<Item*>(index.internalPointer()));

      table[i++] = entry;
    }

  return table;
}

}

