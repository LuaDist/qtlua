/****************************************************************************
** Meta object code from reading C++ file 'qtluatabletreemodel.hh'
**
** Created: Tue Jan 26 17:55:08 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qtluatabletreemodel.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtluatabletreemodel.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtLua__TableTreeModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       2,   12, // enums/sets
       0,    0, // constructors

 // enums: name, flags, count, data
      22, 0x0,   12,   20,
      32, 0x1,   12,   44,

 // enum data: key, value
      43, uint(QtLua::TableTreeModel::Recursive),
      53, uint(QtLua::TableTreeModel::UserDataIter),
      66, uint(QtLua::TableTreeModel::HideType),
      75, uint(QtLua::TableTreeModel::UnquoteKeys),
      87, uint(QtLua::TableTreeModel::UnquoteValues),
     101, uint(QtLua::TableTreeModel::Editable),
     110, uint(QtLua::TableTreeModel::EditFixedType),
     124, uint(QtLua::TableTreeModel::EditLuaEval),
     136, uint(QtLua::TableTreeModel::EditInsert),
     147, uint(QtLua::TableTreeModel::EditRemove),
     158, uint(QtLua::TableTreeModel::EditKey),
     166, uint(QtLua::TableTreeModel::EditAll),
      43, uint(QtLua::TableTreeModel::Recursive),
      53, uint(QtLua::TableTreeModel::UserDataIter),
      66, uint(QtLua::TableTreeModel::HideType),
      75, uint(QtLua::TableTreeModel::UnquoteKeys),
      87, uint(QtLua::TableTreeModel::UnquoteValues),
     101, uint(QtLua::TableTreeModel::Editable),
     110, uint(QtLua::TableTreeModel::EditFixedType),
     124, uint(QtLua::TableTreeModel::EditLuaEval),
     136, uint(QtLua::TableTreeModel::EditInsert),
     147, uint(QtLua::TableTreeModel::EditRemove),
     158, uint(QtLua::TableTreeModel::EditKey),
     166, uint(QtLua::TableTreeModel::EditAll),

       0        // eod
};

static const char qt_meta_stringdata_QtLua__TableTreeModel[] = {
    "QtLua::TableTreeModel\0Attribute\0"
    "Attributes\0Recursive\0UserDataIter\0"
    "HideType\0UnquoteKeys\0UnquoteValues\0"
    "Editable\0EditFixedType\0EditLuaEval\0"
    "EditInsert\0EditRemove\0EditKey\0EditAll\0"
};

const QMetaObject QtLua::TableTreeModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_QtLua__TableTreeModel,
      qt_meta_data_QtLua__TableTreeModel, 0 }
};

const QMetaObject *QtLua::TableTreeModel::metaObject() const
{
    return &staticMetaObject;
}

void *QtLua::TableTreeModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtLua__TableTreeModel))
        return static_cast<void*>(const_cast< TableTreeModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int QtLua::TableTreeModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
