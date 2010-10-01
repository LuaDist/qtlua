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

#include <QMessageBox>
#include <QtLua/TableGridModel>

namespace QtLua {

  TableGridModel::TableGridModel(const Value &table, Attributes attr,
				 bool find_keys, QObject *parent)
    : QAbstractItemModel(parent),
      _st(table.get_state()),
      _attr(attr),
      _table(table),
      _num_row_count(0),
      _num_col_count(0)
  {
    if (find_keys)
      {
	fetch_all_row_keys();
	fetch_all_column_keys();
      }
  }

  TableGridModel::TableGridModel(const Value &table, int row_count, int col_count,
				 Attributes attr, QObject *parent)
    : QAbstractItemModel(parent),
      _st(table.get_state()),
      _attr(attr),
      _table(table),
      _num_row_count(row_count),
      _num_col_count(col_count)
  {
    _attr |= Attributes(NumKeysCols | NumKeysRows);
  }

  TableGridModel::~TableGridModel()
  {
  }

  void TableGridModel::fetch_all_row_keys()
  {
    try {
      if (_attr & NumKeysRows)
	{
	  _num_row_count = _table.len();
	}
      else
	{
	  _row_keys.clear();
	  for (Value::const_iterator i = _table.begin(); i != _table.end(); i++)
	    _row_keys.push_back(i.key());
	}
    } catch (const String &e) {
    }
  }

  void TableGridModel::fetch_all_column_keys()
  {
    Value first(_st);

    try {
      if (_attr & NumKeysRows)
	{
	  first = _table[1];
	}
      else
	{
	  if (_row_keys.empty())
	    return;
	  first = _table[_row_keys[0]];
	}

      if (_attr & NumKeysCols)
	{
	  _num_col_count = first.len();
	}
      else
	{
	  _col_keys.clear();

	  if (!first.is_nil())
	    for (Value::const_iterator i = first.begin(); i != first.end(); i++)
	      _col_keys.push_back(i.key());
	}
    } catch (const String &e) {
    }
  }

  void TableGridModel::set_row_count(int c)
  {
    _attr |= NumKeysRows;
    _num_row_count = c;
  }

  void TableGridModel::set_col_count(int c)
  {
    _attr |= NumKeysCols;
    _num_col_count = c;
  }

  void TableGridModel::add_row_key(const Value &k)
  {
    _attr &= ~NumKeysRows;
    _row_keys.push_back(k);
  }

  void TableGridModel::add_row_key(const String &k)
  {
    _attr &= ~NumKeysRows;
    _row_keys.push_back(Value(_st, k));
  }

  void TableGridModel::add_column_key(const Value &k)
  {
    _attr &= ~NumKeysCols;
    _col_keys.push_back(k);
  }

  void TableGridModel::add_column_key(const String &k)
  {
    _attr &= ~NumKeysCols;
    _col_keys.push_back(Value(_st, k));
  }

  QModelIndex TableGridModel::index(int row, int column, const QModelIndex &parent) const
  {
    if (parent.isValid())
      return QModelIndex();

    if ((_attr & RowColSwap ? column : row) >= row_count())
      return QModelIndex();

    if ((_attr & RowColSwap ? row : column) >= column_count())
      return QModelIndex();

    return createIndex(row, column, 0);
  }

  QModelIndex TableGridModel::parent(const QModelIndex &index) const
  {
    return QModelIndex();
  }

  int TableGridModel::row_count() const
  {
      return _attr & NumKeysRows ? _num_row_count : _row_keys.count();
  }

  int TableGridModel::column_count() const
  {
      return _attr & NumKeysCols ? _num_col_count : _col_keys.count();
  }

  int TableGridModel::rowCount(const QModelIndex &parent) const
  {
    return _attr & RowColSwap ? column_count() : row_count();
  }

  int TableGridModel::columnCount(const QModelIndex &parent) const
  {
    return _attr & RowColSwap ? row_count() : column_count();
  }

  bool TableGridModel::hasChildren(const QModelIndex & parent) const
  {
    return false;
  }

  ValueRef TableGridModel::get_value_ref(const QModelIndex &index) const
  {
    int row = index.row();
    int col = index.column();

    if (_attr & RowColSwap)
      std::swap(row, col);

    // check column bounds
    if (_attr & NumKeysCols)
      {
	if (col >= _num_col_count)
	  goto bound_err;
      }
    else
      {
	if (col >= _col_keys.count())
	  goto bound_err;
      }

    // check row bounds and build valueref
    if (_attr & NumKeysRows)
      {
	if (row >= _num_row_count)
	  goto bound_err;

	if (_attr & NumKeysCols)
	  return ValueRef(_table[row + 1], col + 1);
	else
	  return ValueRef(_table[row + 1], _col_keys[col]);
      }
    else
      {
	if (row >= _row_keys.count())
	  goto bound_err;

	if (_attr & NumKeysCols)
	  return ValueRef(_table[_row_keys[row]], col + 1);
	else
	  return ValueRef(_table[_row_keys[row]], _col_keys[col]);
      }

  bound_err:
    throw String("invalid table index for QtLua::TableGridModel access");
  }

  QVariant TableGridModel::data(const QModelIndex &index, int role) const
  {
    if (!index.isValid())
      return QVariant();

    switch (role)
      {
      case Qt::DisplayRole:
	try {
	  return QVariant(get_value_ref(index).to_string_p(!(_attr & UnquoteValues)));
	} catch (const String &e) {
	  return QVariant();	  
	}

      default:
	return QVariant();
      }
  }

  bool TableGridModel::set_value_ref(const ValueRef &ref, const QByteArray &input)
  {
    Value::ValueType oldtype = ref.type();

    try {
      Value newvalue(_st.eval_expr(_attr & EditLuaEval, input));
      Value::ValueType newtype = newvalue.type();

      // convert to string type when enforced
      if ((_attr & EditFixedType) &&
	  oldtype == Value::TString &&
	  newtype != Value::TString)
	{
	  newvalue = newvalue.to_string_p(false);
	  newtype = Value::TString;
	}

      // check type change
      if ((_attr & EditFixedType) &&
	  (oldtype != Value::TNil) && (oldtype != newtype))
	throw String("% value type must be preserved.").arg(Value::type_name(oldtype));

      ref = newvalue;

      return true;

    } catch (const String &s) {
      QMessageBox::critical(0, "Error", String("Value update error: ") + s);

      return false;
    }
  }

  bool TableGridModel::setData(const QModelIndex & index, const QVariant & value, int role)
  {
    if (!index.isValid())
      return false;

    if (role != Qt::EditRole)
      return false;

    if (!value.canConvert(QVariant::ByteArray))
      return false;

    try {
      return set_value_ref(get_value_ref(index), value.toByteArray());
    } catch (const String &e) {
      return false;
    }
  }

  QVariant TableGridModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (role != Qt::DisplayRole)
      return QVariant();

    if (_attr & RowColSwap)
      {
	if (orientation == Qt::Vertical)
	  orientation = Qt::Horizontal;
	else
	  orientation = Qt::Vertical;
      }

    switch (orientation)
      {
      case Qt::Vertical:
	if (_attr & NumKeysRows)
	  return QVariant(section + 1);
	else if (section < _row_keys.count())
	  return QVariant(_row_keys[section].to_string_p(!(_attr & UnquoteHeader)));
	break;

      case Qt::Horizontal:
	if (_attr & NumKeysCols)
	  return QVariant(section + 1);
	else if (section < _col_keys.count())
	  return QVariant(_col_keys[section].to_string_p(!(_attr & UnquoteHeader)));
	break;
      }

    return QVariant();
  }

  bool TableGridModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
  {
    if (role != Qt::EditRole)
      return false;

    if (!value.canConvert(QVariant::ByteArray))
      return false;

    if (_attr & RowColSwap)
      {
	if (orientation == Qt::Vertical)
	  orientation = Qt::Horizontal;
	else
	  orientation = Qt::Vertical;
      }

    try {

      switch (orientation)
	{
	case Qt::Vertical:
	  if (_attr & NumKeysRows)
	    return set_value_ref(ValueRef(_table, section + 1), value.toByteArray());
	  else
	    return set_value_ref(ValueRef(_table, _row_keys[section]), value.toByteArray());

	case Qt::Horizontal:
	  // Not implemented yet
	  break;
	}

    } catch (const String &e) {
      return false;
    }

    return false;
  }

  Qt::ItemFlags TableGridModel::flags(const QModelIndex &index) const
  {
    if (!index.isValid())
      return Qt::ItemFlags();

    Qt::ItemFlags flags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    if (_attr & Editable)
      flags |= Qt::ItemIsEditable;

    return flags;
  }

  bool TableGridModel::removeRows(int row, int count, const QModelIndex &parent)
  {
    if (_attr & RowColSwap)
      return remove_columns(row, count, parent);
    else
      return remove_rows(row, count, parent);
  }

  bool TableGridModel::insertRows(int row, int count, const QModelIndex &parent)
  {
    if (_attr & RowColSwap)
      return insert_columns(row, count, parent);
    else
      return insert_rows(row, count, parent);
  }

  bool TableGridModel::removeColumns(int column, int count, const QModelIndex &parent)
  {
    if (_attr & RowColSwap)
      return remove_rows(column, count, parent);
    else
      return remove_columns(column, count, parent);
  }

  bool TableGridModel::insertColumns(int column, int count, const QModelIndex &parent)
  {
    if (_attr & RowColSwap)
      return insert_rows(column, count, parent);
    else
      return insert_columns(column, count, parent);
  }

  bool TableGridModel::remove_rows(int row, int count, const QModelIndex &parent)
  {
    if (!(_attr & EditRemoveRow))
      return false;

    if (parent.isValid())
      return false;

    if (_attr & RowColSwap)
      beginRemoveColumns(parent, row, row + count - 1);
    else
      beginRemoveRows(parent, row, row + count - 1);

    if (_attr & NumKeysRows)
      {
	int i;

	// shift all tail rows
	for (i = row + 1; i <= _num_row_count - count; i++)
	  QTLUA_PROTECT(_table[i] = _table[i + count]);

	// remove tail keys
	for (; i <= _num_row_count; i++)
	  QTLUA_PROTECT(_table[i] = Value(_st));

	_num_row_count -= count;
      }
    else
      {
	QTLUA_PROTECT(_table[_row_keys[row]] = Value(_st));
	_row_keys.removeAt(row);
      }

    if (_attr & RowColSwap)
      endRemoveColumns();
    else
      endRemoveRows();

    return true;
  }

  Value TableGridModel::new_row_table(State &st) const
  {
    return Value(st, Value::TTable);
  }

  bool TableGridModel::insert_rows(int row, int count, const QModelIndex &parent)
  { 
    if (!(_attr & EditInsertRow))
      return false;

    if (parent.isValid())
      return false;

    if (_attr & RowColSwap)
      beginInsertColumns(parent, row, row + count - 1);
    else
      beginInsertRows(parent, row, row + count - 1);

    if (_attr & NumKeysRows)
      {
	// shift all tail rows
	for (int i = _num_row_count; i > row; i--)
	  QTLUA_PROTECT(_table[i + count] = _table[i]);

	// add new nested tables for rows
	for (int i = row + 1; i <= row + count; i++)
	  QTLUA_PROTECT(_table[i] = new_row_table(_st));

	_num_row_count += count;
      }
    else
      {
	for (int i = 0; i < count; i++)
	  {
	    int k = row + 1;

	    // find nil key
	    QTLUA_PROTECT(
			  for (; !_table[k].is_nil(); k++)
			    ;
			  );

	    _row_keys.insert(row + i, Value(_st, k));
	    QTLUA_PROTECT(_table[k] = new_row_table(_st));
	  }
      }

    if (_attr & RowColSwap)
      endInsertColumns();
    else
      endInsertRows();

    return true;
  }

  bool TableGridModel::remove_columns(int row, int count, const QModelIndex &parent)
  { 
    // FIXME not implemented yet
    return false;
  }

  bool TableGridModel::insert_columns(int row, int count, const QModelIndex &parent)
  { 
    // FIXME not implemented yet
    return false;
  }

}

