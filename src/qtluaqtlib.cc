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

#include <QFile>
#include <QUiLoader>
#include <QWidget>

#include <QColorDialog>
#include <QFileDialog>
#include <QErrorMessage>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>
#include <QTranslator>

#include <QMenu>
#include <QMenuBar>

#include <QtLua/State>
#include <QtLua/Function>
#include <internal/QObjectWrapper>
#include <QtLua/QHashProxy>
#include <QtLua/TableDialog>

#include <internal/Method>
#include <internal/MetaCache>
#include <internal/QMetaObjectWrapper>

#include "qtluaqtlib.hh"

namespace QtLua {

  typedef QMap<String, QMetaObjectWrapper > qmetaobject_table_t;

  class QMetaObjectTable : public QHashProxyRo<qmetaobject_table_t>
  {
  public:
    QMetaObjectTable()
      : QHashProxyRo<qmetaobject_table_t>(_mo_table)
    {
      for (const QMetaObject **mo = meta_object_table; *mo; mo++)
	{
	  String name((*mo)->className());
	  name.replace(':', '_');
	  _mo_table.insert(name, QMetaObjectWrapper(*mo));
	}
    }

  private:
    void completion_patch(String &path, String &entry, int &offset)
    {
      entry += ".";
    };

    qmetaobject_table_t _mo_table;
  };

  void qtluaopen_qt(State &ls)
  {
    static QMetaObjectTable meta;

    ls.set_global("qt.meta", Value(ls, meta));

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	meta_call_check_args(args, 3, 4, Value::TUserData, Value::TString, Value::TNone, Value::TString);

	QObjectWrapper::ptr sigqow = args[0].to_userdata_cast<QObjectWrapper>();
	QObject &sigobj = sigqow->get_object();
	Method::ptr sig = MetaCache::get_meta(sigobj).get_member_throw<Method>(args[1].to_string());
	int sigindex = sig->get_index();
	QMetaMethod sigmm = sigobj.metaObject()->method(sigindex);

	if (sigmm.methodType() != QMetaMethod::Signal)
	  throw String("First argument is not a signal method.");

	switch (args.size())
	  {
	  case 3: {
	    // connect qt signal to lua function
	    sigqow->_lua_connect(sigindex, args[2]);
	    return Value::List();
	  }

	  case 4: {
	    // connect qt signal to qt slot
	    QObject &sloobj = args[2].to_userdata_cast<QObjectWrapper>()->get_object();	
	    Method::ptr slo = MetaCache::get_meta(sloobj).get_member_throw<Method>(args[3].to_string());
	    int sloindex = slo->get_index();
	    QMetaMethod slomm = sloobj.metaObject()->method(sloindex);

	    if (!QMetaObject::checkConnectArgs(sigmm.signature(), slomm.signature()))
	      throw String("Unable to connect signal/slot, incompatible argument types.");

	    if (QMetaObject::connect(&sigobj, sigindex, &sloobj, sloindex))
	      return Value::List();
	  }

	  }

	throw String("Unable to connect signal/slot.");
      }

      String get_description() const
      {
	return "connect a Qt signal to a Qt slot or lua function";
      }

      String get_help() const
      {
	return ("usage: qt.connect(qobjectwrapper, \"qt_signal_name\", qobjectwrapper, \"qt_slot_name\")\n"
		"       qt.connect(qobjectwrapper, \"qt_signal_name\", lua_function)\n");
      }
    } connect;

    connect.register_(ls, "qt.connect");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	meta_call_check_args(args, 2, 4, Value::TUserData, Value::TString, Value::TNone, Value::TString);

	QObjectWrapper::ptr sigqow = args[0].to_userdata_cast<QObjectWrapper>();
	QObject &sigobj = sigqow->get_object();
	Method::ptr sig = MetaCache::get_meta(sigobj).get_member_throw<Method>(args[1].to_string());
	int sigindex = sig->get_index();

	switch (args.size())
	  {
	  case 2:
	    // disconnect qt signal from all lua functions
	    sigqow->_lua_disconnect_all(sigindex);
	    return Value::List();

	  case 3:
	    // disconnect qt signal from lua function
	    return Value(ls, (Value::Bool)sigqow->_lua_disconnect(sigindex, args[2]));

	  case 4: {
	    // disconnect qt signal from qt slot
	    QObject &sloobj = args[2].to_userdata_cast<QObjectWrapper>()->get_object();	
	    Method::ptr slo = MetaCache::get_meta(sloobj).get_member_throw<Method>(args[3].to_string());
	    int sloindex = slo->get_index();

	    return Value(ls, (Value::Bool)QMetaObject::disconnect(&sigobj, sigindex, &sloobj, sloindex));
	  }

	  }

	abort();
      }

      String get_description() const
      {
	return "disconnect a Qt signal";
      }

      String get_help() const
      {
	return ("usage: qt.disconnect(qobjectwrapper, \"qt_signal_name\", qobjectwrapper, \"qt_slot_name\")\n"
		"       qt.disconnect(qobjectwrapper, \"qt_signal_name\", lua_function)\n"
		"       qt.disconnect(qobjectwrapper, \"qt_signal_name\")\n");
      }
    } disconnect;

    disconnect.register_(ls, "qt.disconnect");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	static QUiLoader uil;

	meta_call_check_args(args, 1, 2, Value::TString, Value::TUserData);
	QWidget *p = 0;

	if (args.size() > 1)
	  {
	    p = dynamic_cast<QWidget *>(&args[1].to_userdata_cast<QObjectWrapper>()->get_object());
	    if (!p)
	      throw String("Parent QObject is not a QWidget.");
	  }

	QFile f(args[0].to_qstring());
	QObject *w = uil.load(&f, p);

	if (!w)
	  throw String("Unable to load '%' ui file.").arg(f.fileName());

	return Value(ls, w, true);
      }

      String get_description() const
      {
	return "load a Qt ui file";
      }

      String get_help() const
      {
	return ("usage: qt.load_ui(\"file.ui\", parent_qobjectwrapper)\n"
		"usage: qt.load_ui(\"file.ui\")\n");
      }

    } load_ui;

    load_ui.register_(ls, "qt.load_ui");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	static QUiLoader uil;

	meta_call_check_args(args, 1, 3, Value::TString, Value::TString, Value::TUserData);
	QWidget *p = 0;
	String classname(args[0].to_string());
	String name;

	if (args.size() > 2)
	  {
	    p = dynamic_cast<QWidget *>(&args[2].to_userdata_cast<QObjectWrapper>()->get_object());
	    if (!p)
	      throw String("Parent QObject is not a QWidget.");
	  }

	if (args.size() > 1)
	  name = args[1].to_string();

	QObject *w = uil.createWidget(classname.to_qstring(), p, name.to_qstring());

	if (!w)
	  throw String("Unable to create % type widget.").arg(classname);

	return Value(ls, w, true);
      }

      String get_description() const
      {
	return "dynamically create a new Qt Widget";
      }

      String get_help() const
      {
	return ("usage: qt.new_widget(\"QtClassName\", [ \"name\", parent_qobjectwrapper ] )");
      }

    } new_widget;

    new_widget.register_(ls, "qt.new_widget");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	meta_call_check_args(args, 2, 2, Value::TUserData, Value::TString);

	QObjectWrapper::ptr qow = args[0].to_userdata_cast<QObjectWrapper>();
	QObject &obj = qow->get_object();
	QObject *result;
	String name = args[1].to_string();

	if (QMenu *menu = dynamic_cast<QMenu*>(&obj))
	  result = menu->addMenu(name.to_qstring());
	else if (QMenuBar *menubar = dynamic_cast<QMenuBar*>(&obj))
	  result = menubar->addMenu(name.to_qstring());
	else
	  throw String("Bad menu owner object type");

	result->setObjectName(QString("menu") + name.to_qstring());
	return QtLua::Value(ls, result, false);
      }

      String get_description() const
      {
	return "Add a QMenu to a QMenu or QMenuBar";
      }

      String get_help() const
      {
	return ("usage: qt.menu.addmenu( parent, name )");
      }

    } menu_addmenu;

    menu_addmenu.register_(ls, "qt.menu.addmenu");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	meta_call_check_args(args, 2, 2, Value::TUserData, Value::TString);

	QObjectWrapper::ptr qow = args[0].to_userdata_cast<QObjectWrapper>();
	QObject &obj = qow->get_object();
	QObject *result;
	String name = args[1].to_string();

	if (QMenu *menu = dynamic_cast<QMenu*>(&obj))
	  result = menu->addAction(name.to_qstring());
	else if (QMenuBar *menubar = dynamic_cast<QMenuBar*>(&obj))
	  result = menubar->addAction(name.to_qstring());
	else
	  throw String("Bad menu owner object type");

	result->setObjectName(QString("action") + name.to_qstring());
	return QtLua::Value(ls, result, false);
      }

      String get_description() const
      {
	return "Add a QAction to a QMenu or QMenuBar";
      }

      String get_help() const
      {
	return ("usage: qt.menu.addaction( parent, name )");
      }

    } menu_addaction;

    menu_addaction.register_(ls, "qt.menu.addaction");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	meta_call_check_args(args, 1, 1, Value::TUserData);

	QObjectWrapper::ptr qow = args[0].to_userdata_cast<QObjectWrapper>();
	QObject &obj = qow->get_object();
	QAction *action;
	QMenu *menu = 0;

	if ((action = dynamic_cast<QAction*>(&obj)))
	  ;
	else if ((menu = dynamic_cast<QMenu*>(&obj)))
	  action = menu->menuAction();
	else
	  throw String("Unable to find associated QAction");

	if (QWidget *parent = dynamic_cast<QWidget*>(obj.parent()))
	  parent->removeAction(action);
	else
	  throw String("QAction has no parent");

	delete action;
	if (menu)
	  delete menu;
	return QtLua::Value(ls);
      }

      String get_description() const
      {
	return "Remove a QAction from a QMenu or QMenuBar";
      }

      String get_help() const
      {
	return ("usage: qt.menu.addaction( parent, name )");
      }

    } menu_remove;

    menu_remove.register_(ls, "qt.menu.remove");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, new QFileDialog(), true);
      }

      String get_description() const
      {
	return "dynamically create a new QFileDialog object";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.new_file_dialog()");
      }

    } dialog_new_file_dialog;

    dialog_new_file_dialog.register_(ls, "qt.dialog.new_file_dialog");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, QFileDialog::getExistingDirectory(QApplication::activeWindow(),
			   get_arg<QString>(args, 0, ""),
			   get_arg<QString>(args, 1, ""),
			   (QFileDialog::Option)get_arg<int>(args, 2, QFileDialog::ShowDirsOnly)
			 ));
      }

      String get_description() const
      {
	return "wrap QFileDialog::getExistingDirectory function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.get_existing_directory( [ \"caption\", \"directory\", options ] )");
      }

    } dialog_get_existing_directory;

    dialog_get_existing_directory.register_(ls, "qt.dialog.get_existing_directory");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, QFileDialog::getOpenFileName(QApplication::activeWindow(),
			   get_arg<QString>(args, 0, ""),
			   get_arg<QString>(args, 1, ""),
			   get_arg<QString>(args, 2, ""), 0,
			   (QFileDialog::Option)get_arg<int>(args, 3, 0)
			 ));
      }

      String get_description() const
      {
	return "wrap QFileDialog::getOpenFileName function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.get_open_filename( [ \"caption\", \"directory\", \"filter\", options ] )");
      }

    } dialog_get_open_filename;

    dialog_get_open_filename.register_(ls, "qt.dialog.get_open_filename");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, QFileDialog::getOpenFileNames(QApplication::activeWindow(),
			   get_arg<QString>(args, 0, ""),
			   get_arg<QString>(args, 1, ""),
			   get_arg<QString>(args, 2, ""), 0,
			   (QFileDialog::Option)get_arg<int>(args, 3, 0)
			 ));
      }

      String get_description() const
      {
	return "wrap QFileDialog::getOpenFileNames function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.get_open_filenames( [ \"caption\", \"directory\", \"filter\", options ] )");
      }

    } dialog_get_open_filenames;

    dialog_get_open_filenames.register_(ls, "qt.dialog.get_open_filenames");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, QFileDialog::getSaveFileName(QApplication::activeWindow(),
			   get_arg<QString>(args, 0, ""),
			   get_arg<QString>(args, 1, ""),
			   get_arg<QString>(args, 2, ""), 0,
			   (QFileDialog::Option)get_arg<int>(args, 3, 0)
			 ));
      }

      String get_description() const
      {
	return "wrap QFileDialog::getSaveFileName function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.get_save_filename( [ \"caption\", \"directory\", \"filter\", options ] )");
      }

    } dialog_get_save_filename;

    dialog_get_save_filename.register_(ls, "qt.dialog.get_save_filename");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	QColor init(Qt::white);

	if (args.count() >= 3)
	  init = QColor(get_arg<int>(args, 0, 0), get_arg<int>(args, 1, 0), get_arg<int>(args, 2, 0));

	QColor c = QColorDialog::getColor(init, QApplication::activeWindow());

	return c.isValid() ? Value::List(Value(ls, c.red()), Value(ls, c.green()), Value(ls, c.blue()))
	                   : Value::List();
      }

      String get_description() const
      {
	return "wrap QColorDialog::getColor function, returns rgb triplet in [0, 255] range";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.get_color( [ init_red, init_green, init_blue ] )");
      }

    } dialog_get_color;

    dialog_get_color.register_(ls, "qt.dialog.get_color");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, new QErrorMessage(), true);
      }

      String get_description() const
      {
	return "dynamically create a new QErrorMessage object";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.new_error_message()");
      }

    } dialog_new_error_message;

    dialog_new_error_message.register_(ls, "qt.dialog.new_error_message");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	bool ok;
	double v = QInputDialog::getDouble(QApplication::activeWindow(),
				 get_arg<QString>(args, 0, ""),
				 get_arg<QString>(args, 1, ""),
				 get_arg<double>(args, 2, 0),
				 get_arg<double>(args, 3, -2147483647),
				 get_arg<double>(args, 4, 2147483647),
				 get_arg<int>(args, 5, 1),
				 &ok
				 );
	return ok ? Value(ls, v) : Value(ls);
      }

      String get_description() const
      {
	return "wrap QInputDialog::getDouble function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.get_double(\"title\", \"label\", value, min, max, decimals)");
      }

    } dialog_get_double;

    dialog_get_double.register_(ls, "qt.dialog.get_double");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	bool ok;
	int v = QInputDialog::getInteger(QApplication::activeWindow(),
				 get_arg<QString>(args, 0, ""),
				 get_arg<QString>(args, 1, ""),
				 get_arg<int>(args, 2, 0),
				 get_arg<int>(args, 3, -2147483647),
				 get_arg<int>(args, 4, 2147483647),
				 get_arg<int>(args, 5, 1),
				 &ok
				 );
	return ok ? Value(ls, v) : Value(ls);
      }

      String get_description() const
      {
	return "wrap QInputDialog::getInteger function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.get_integer(\"title\", \"label\", value, min, max, step)");
      }

    } dialog_get_integer;

    dialog_get_integer.register_(ls, "qt.dialog.get_integer");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	bool ok;
	QString v = QInputDialog::getText(QApplication::activeWindow(),
				 get_arg<QString>(args, 0, ""),
				 get_arg<QString>(args, 1, ""),
				 QLineEdit::Normal,
				 get_arg<QString>(args, 2, ""),
				 &ok
				 );
	return ok ? Value(ls, v) : Value(ls);
      }

      String get_description() const
      {
	return "wrap QInputDialog::getText function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.get_text(\"title\", \"label\", \"text\")");
      }

    } dialog_get_text;

    dialog_get_text.register_(ls, "qt.dialog.get_text");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	bool ok;
	QString v = QInputDialog::getItem(QApplication::activeWindow(),
				 get_arg<QString>(args, 3, ""),
				 get_arg<QString>(args, 4, ""),
				 get_arg<QList<QString> >(args, 0),
				 get_arg<int>(args, 1, 0),
				 get_arg<Value::Bool>(args, 2, Value::False),
				 &ok
				 );
	return ok ? Value(ls, v) : Value(ls);
      }

      String get_description() const
      {
	return "wrap QInputDialog::getItem function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.get_item( { \"item\", \"item\", ... }, [ default, editable, \"title\", \"label\" ] )");
      }

    } dialog_get_item;

    dialog_get_item.register_(ls, "qt.dialog.get_item");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, new QMessageBox(), true);
      }

      String get_description() const
      {
	return "dynamically create a new QMessageBox object";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.new_message_box()");
      }

    } dialog_new_message_box;

    dialog_new_message_box.register_(ls, "qt.dialog.new_message_box");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	QMessageBox::about(QApplication::activeWindow(),
			   get_arg<QString>(args, 1, ""),
			   get_arg<QString>(args, 0));
	return Value(ls);
      }

      String get_description() const
      {
	return "wrap QMessageBox::about function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.msg_about( \"text\" [ , \"title\" ] )");
      }

    } dialog_msg_about;

    dialog_msg_about.register_(ls, "qt.dialog.msg_about");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, QMessageBox::critical(QApplication::activeWindow(),
					       get_arg<QString>(args, 1, ""),
					       get_arg<QString>(args, 0),
					       (QMessageBox::StandardButtons)get_arg<int>(args, 2, QMessageBox::Ok),
					       (QMessageBox::StandardButton)get_arg<int>(args, 3, QMessageBox::NoButton)));
      }

      String get_description() const
      {
	return "wrap QMessageBox::critical function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.msg_critical( \"text\" [ , \"title\", buttons, default_button ] )");
      }

    } dialog_msg_critical;

    dialog_msg_critical.register_(ls, "qt.dialog.msg_critical");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, QMessageBox::information(QApplication::activeWindow(),
					       get_arg<QString>(args, 1, ""),
					       get_arg<QString>(args, 0),
					       (QMessageBox::StandardButtons)get_arg<int>(args, 2, QMessageBox::Ok),
					       (QMessageBox::StandardButton)get_arg<int>(args, 3, QMessageBox::NoButton)));
      }

      String get_description() const
      {
	return "wrap QMessageBox::information function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.msg_information( \"text\" [ , \"title\", buttons, default_button ] )");
      }

    } dialog_msg_information;

    dialog_msg_information.register_(ls, "qt.dialog.msg_information");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, QMessageBox::question(QApplication::activeWindow(),
					       get_arg<QString>(args, 1, ""),
					       get_arg<QString>(args, 0),
					       (QMessageBox::StandardButtons)get_arg<int>(args, 2, QMessageBox::Ok),
					       (QMessageBox::StandardButton)get_arg<int>(args, 3, QMessageBox::NoButton)));
      }

      String get_description() const
      {
	return "wrap QMessageBox::question function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.msg_question( \"text\" [ , \"title\", buttons, default_button ] )");
      }

    } dialog_msg_question;

    dialog_msg_question.register_(ls, "qt.dialog.msg_question");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, QMessageBox::warning(QApplication::activeWindow(),
					       get_arg<QString>(args, 1, ""),
					       get_arg<QString>(args, 0),
					       (QMessageBox::StandardButtons)get_arg<int>(args, 2, QMessageBox::Ok),
					       (QMessageBox::StandardButton)get_arg<int>(args, 3, QMessageBox::NoButton)));
      }

      String get_description() const
      {
	return "wrap QMessageBox::warning function";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.msg_warning( \"text\" [ , \"title\", buttons, default_button ] )");
      }

    } dialog_msg_warning;

    dialog_msg_warning.register_(ls, "qt.dialog.msg_warning");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	meta_call_check_args(args, 1, 3, Value::TNone, Value::TNumber, Value::TNumber);

	return Value(ls, new TableDialog(args[0],
					 (TableDialog::ViewType)get_arg<int>(args, 1), 0,
					 get_arg<int>(args, 2, 0), 0), true);
      }

      String get_description() const
      {
	return "dynamically create a new QtLua::TableDialog";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.new_table_dialog( table , viewtype, [ attributes ] )");
      }

    } new_table_dialog;

    new_table_dialog.register_(ls, "qt.dialog.new_table_dialog");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	meta_call_check_args(args, 1, 3, Value::TNone, Value::TNumber, Value::TString);

	TableDialog::tree_tree_dialog(QApplication::activeWindow(),
				      get_arg<QString>(args, 2, ""), args[0],
				      (TableTreeModel::Attributes)get_arg<int>(args, 1, 0)
				      );

	return Value::List();
      }

      String get_description() const
      {
	return "expose a lua table in a QTreeView";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.tree_treeview( table [ , attributes, \"title\" ] )");
      }

    } dialog_tree_treeview;

    dialog_tree_treeview.register_(ls, "qt.dialog.tree_treeview");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	meta_call_check_args(args, 1, 3, Value::TNone, Value::TNumber, Value::TString);

	TableDialog::tree_table_dialog(QApplication::activeWindow(),
				      get_arg<QString>(args, 2, ""), args[0],
				      (TableTreeModel::Attributes)get_arg<int>(args, 1, 0)
				      );

	return Value::List();
      }

      String get_description() const
      {
	return "expose a lua table in a QTreeView";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.tree_tableview( table [ , attributes, \"title\" ] )");
      }

    } dialog_tree_tableview;

    dialog_tree_tableview.register_(ls, "qt.dialog.tree_tableview");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	meta_call_check_args(args, 1, 5, Value::TNone, Value::TNumber,
			     Value::TString, Value::TTable, Value::TTable);
	Value::List rk, *rkptr = 0;
	Value::List ck, *ckptr = 0;

	if (args.count() >= 5)
	  {
	    rk = args[4].to_qlist<Value>();
	    if (!rk.empty())
	      rkptr = &rk;
	  }

	if (args.count() >= 4)
	  {
	    ck = args[3].to_qlist<Value>();
	    if (!ck.empty())
	      ckptr = &ck;
	  }

	TableDialog::grid_table_dialog(QApplication::activeWindow(),
				       get_arg<QString>(args, 2, ""), args[0],
				       (TableGridModel::Attributes)get_arg<int>(args, 1, 0),
				       ckptr, rkptr
				       );

	return Value::List();
      }

      String get_description() const
      {
	return "expose 2 dimensions nested lua tables in a QTableView";
      }

      String get_help() const
      {
	return ("usage: qt.dialog.grid_tableview( table [ , attributes, \"title\", {column keys}, {row keys} ] )");
      }

    } dialog_grid_tableview;

    dialog_grid_tableview.register_(ls, "qt.dialog.grid_tableview");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	return Value(ls, QCoreApplication::translate(get_arg<String>(args, 0),
						     get_arg<String>(args, 1),
						     get_arg<String>(args, 2, ""),
						     QCoreApplication::UnicodeUTF8,
						     get_arg<int>(args, 3, -1)));
      }

      String get_description() const
      {
	return "Translate utf8 text using the QCoreApplication::translate function.";
      }

      String get_help() const
      {
	return ("usage: qt.tr(\"context\", \"text\", [ \"disambiguation\", n ])");
      }

    } qt_tr;

    qt_tr.register_(ls, "qt.tr");

    //////////////////////////////////////////////////////////////////////

    static class : public Function
    {
      Value::List meta_call(State &ls, const Value::List &args)
      {
	String filename(get_arg<String>(args, 0));
	QTranslator *qtr = new QTranslator();

	if (!qtr->load(filename))
	  {
	    delete qtr;
	    throw String("Unable to load translation file `%'").arg(filename);
	  }

	QCoreApplication::installTranslator(qtr);
	return Value(ls, qtr, true);
      }

      String get_description() const
      {
	return "Install a translation file and return associated QTranslator object.";
      }

      String get_help() const
      {
	return ("usage: qt.translator(\"filename\")");
      }

    } qt_translator;

    qt_translator.register_(ls, "qt.translator");

  }

}

