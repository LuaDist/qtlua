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

#ifndef QTLUA_ITEMSELECTIONMODEL_HH_
#define QTLUA_ITEMSELECTIONMODEL_HH_

#include <QItemSelectionModel>
#include "qtluaitemmodel.hh"

namespace QtLua {

  /**
   * @short Qt Model/View exclusive selection model class
   * @header QtLua/ItemSelectionModel
   * @module {Model/View}
   *
   * This class can be used with @ref ItemModel if you want to
   * prevent multiple selection which contains childs and associated
   * parents at the same time.
   */

  class ItemSelectionModel : public QItemSelectionModel
  {
    Q_OBJECT;

  public:
    inline ItemSelectionModel(ItemModel *model);
    inline ItemSelectionModel(ItemModel *model, QObject *parent);

  public slots:
    virtual void select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);
    virtual void select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command);

  private:
    static void select_childs(const QModelIndex &index, QItemSelection &selection);
    static void select_parents(const QModelIndex &index, QItemSelection &selection);
  };

}

#endif

