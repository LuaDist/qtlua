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

    Copyright (C) 2010, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/


#ifndef QTLUA_TABLETREEMODEL_HH_
#define QTLUA_TABLETREEMODEL_HH_

#include <QAbstractItemModel>
#include <QPointer>

#include <QtLua/Value>

namespace QtLua {

  class TableTreeKeys;

  /**
   * @short Qt Model/View lua table model class
   * @header QtLua/TableTreeModel
   * @module {Model/View}
   *
   * This class can be used to expose lua tables content to Qt view
   * widgets in a flat or hierarchical manner.
   *
   * Lua tables and @ref UserData objects with valid table operations
   * are handled.
   *
   * Lua tables can be edited from Qt views using this model. The
   * @ref Attribute flags can be used to finely control which editing
   * actions are allowed. User input may be evaluated as a lua
   * expression when editing a table entry.
   *
   * Lua tables change may @b not update the model on the fly and the
   * @ref update function must be called to refresh views on heavy
   * modifications. This is partially due to lack of lua mechanism to
   * implement efficient table change event.
   *
   * Usage example:
   * @example examples/cpp/mvc/tabletreeview.cc:1
   *
   * @image qtlua_tabletreemodel.png
   *
   * @see TableDialog
   */

  class TableTreeModel : public QAbstractItemModel
  {
    Q_OBJECT;
    Q_ENUMS(Attribute);
    Q_FLAGS(Attributes);

  public:

    /** Specifies @ref TableTreeModel behavior for a given lua table @showvalue */
    enum Attribute
      {
	Recursive	= 0x00000001,	//< Expose nested tables too.
	UserDataIter	= 0x00000002,	//< Iterate over UserData objects too.
	HideType	= 0x00000004,	//< Do not show entry type in an additionnal column.
	UnquoteKeys	= 0x00000008,	//< Strip double quotes from string keys
	UnquoteValues	= 0x00000010,	//< Strip double quotes from string values

	Editable	= 0x00001000,	//< Allow editing exposed lua tables.
	EditFixedType	= 0x00002000,	//< Prevent value type change when editing.
	EditLuaEval	= 0x00004000,	//< Evaluate user input as a lua expression.
	EditInsert	= 0x00008000,	//< Allow insertion of new entries.
	EditRemove	= 0x00010000,	//< Allow deletion of existing entries.
	EditKey		= 0x00020000,	//< Allow entry key update.
	EditAll		= 0x00040000,	//< Editable, EditInsert, EditRemove and EditKey allowed
      };

    Q_DECLARE_FLAGS(Attributes, Attribute);

    /** Create a new lua table model. */
    TableTreeModel(const Value &root, Attributes attr, QObject *parent = 0);

    ~TableTreeModel();

    /** Clear cached table content and reset model. */
    void update();

    /** Get lua value at given model index */
    Value get_value(const QModelIndex &index) const;

    /** Get supported operations for entry at given @ref QModelIndex */
    Attributes get_attr(const QModelIndex &index) const;

  public:
    /** @internal Columns ids */
    enum ColumnId
      {
	ColKey = 0, ColValue = 1, ColType = 2,
      };

    /** @multiple @internal */
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    QModelIndex buddy(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    bool hasChildren(const QModelIndex & parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool removeRows(int row, int count, const QModelIndex &parent);
    bool insertRows(int row, int count, const QModelIndex &parent);
    /** */

  private:

    void check_state() const;
    TableTreeKeys * table_from_index(const QModelIndex &index) const;

    QPointer<State> _st;
    TableTreeKeys *_table;
  };

  Q_DECLARE_OPERATORS_FOR_FLAGS(TableTreeModel::Attributes);

}

#endif

