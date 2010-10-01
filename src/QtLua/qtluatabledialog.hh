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


#ifndef QTLUA_TABLEDIALOG_HH_
#define QTLUA_TABLEDIALOG_HH_

#include <QAbstractItemView>
#include <QDialog>

#include <QtLua/TableTreeModel>
#include <QtLua/TableGridModel>

namespace QtLua {

  /**
   * @short Qt Model/View lua table view dialog
   * @header QtLua/TableDialog
   * @module {Model/View}
   *
   * This dialog class use the @ref TableTreeModel or @ref
   * TableGridModel classes to show and enable edition of lua tables
   * in @ref QTreeView or @ref QTableView widgets.
   *
   * The dialog has some edition buttons depending on model edition
   * attributes.
   *
   * The @ref QtLib lua library provides functions to invoke these
   * dialogs from lua code.
   *
   * @see TableTreeModel @see TableGridModel
   */

  class TableDialog : public QDialog
  {
    Q_OBJECT;
    Q_ENUMS(ViewType);
    Q_PROPERTY(bool resize_on_expand READ get_resize_on_expand WRITE set_resize_on_expand);
    Q_PROPERTY(float column_margin_factor READ get_column_margin_factor WRITE set_column_margin_factor);

  public:

    /** Specify model and view to use for the @ref TableDialog dialog */
    enum ViewType
      {
	TreeTreeView,		//< Use @ref TableTreeModel with a @ref QTreeView
	TreeTableView,		//< Use @ref TableTreeModel with a @ref QTableView
	GridTableView,		//< Use @ref TableGridModel with a @ref QTableView
      };

    /**
     * Create a table dialog.
     *
     * @param table lua table to expose
     * @param type dialog type
     * @param model mvc model to use, a default model is created if @ref NULL.
     * @param attr model attributes, control display and edit options
     *
     * @see tree_tree_dialog @see tree_table_dialog @see grid_table_dialog
     */
    TableDialog(const Value &table, enum ViewType type,
		QAbstractItemModel *model = 0,
		int attr = 0, QWidget *parent = 0);

    /** Return pointer to model */
    inline QAbstractItemModel *get_model() const;
    /** Return pointer to view */
    inline QAbstractItemView *get_view() const;

    /** Set keys column resize to content on node expand */
    void set_resize_on_expand(bool roe);
    /** Get current resize on expand state */
    bool get_resize_on_expand() const;

    /** Set additionnal column width factor */
    void set_column_margin_factor(float cmf);
    /** Get additionnal column width factor */
    float get_column_margin_factor() const;

    /**
     * @multiple {2}
     * Shortcut function to display a modal lua table dialog. A @ref TableTreeModel model is used.
     *
     * @param parent parent widget
     * @param title dialog window title
     * @param table lua table to expose
     * @param attr model attributes, control display and edit options
     */
    static void tree_tree_dialog(QWidget *parent, const QString &title, const Value &table, 
				 TableTreeModel::Attributes attr = TableTreeModel::Recursive);

    static void tree_table_dialog(QWidget *parent, const QString &title, const Value &table, 
				  TableTreeModel::Attributes attr = TableTreeModel::Recursive);

    /**
     * Shortcut function to display a modal lua table dialog. A @ref TableGridModel model is used.
     *
     * @param parent parent widget
     * @param title dialog window title
     * @param table lua table to expose
     * @param attr model attributes, control display and edit options
     * @param colkeys list of lua value to use as column keys,
     *  use @ref TableGridModel::fetch_all_column_keys if @ref NULL.
     * @param rowkeys list of lua value to use as row keys,
     *  use @ref TableGridModel::fetch_all_row_keys if @ref NULL.
     */
    static void grid_table_dialog(QWidget *parent, const QString &title, const Value &table,
				  TableGridModel::Attributes attr = TableGridModel::Attributes(),
				  const Value::List *colkeys = 0, const Value::List *rowkeys = 0);

  private slots:
    void edit() const;
    void tree_insert() const;
    void tree_remove() const;
    void tree_current_changed(const QModelIndex & index) const;
    void tree_expanded() const;

    void grid_insert_row() const;
    void grid_remove_row() const;
    void grid_insert_column() const;
    void grid_remove_column() const;
    void grid_current_changed(const QModelIndex & index) const;

  protected:
    virtual QSize sizeHint() const;

    QAbstractItemModel *_model;
    QAbstractItemView *_view;
    QPushButton *_eb, *_rb, *_ib, *_rc, *_ic;
    bool _resize_on_expand;
    float _column_margin_factor;
  };

}

#endif

