/****************************************************************************
** Meta object code from reading C++ file 'qtluatablegridmodel.hh'
**
** Created: Tue Jan 26 17:55:08 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qtluatablegridmodel.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtluatablegridmodel.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtLua__TableGridModel[] = {

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
      43, uint(QtLua::TableGridModel::NumKeysCols),
      55, uint(QtLua::TableGridModel::NumKeysRows),
      67, uint(QtLua::TableGridModel::RowColSwap),
      78, uint(QtLua::TableGridModel::UnquoteHeader),
      92, uint(QtLua::TableGridModel::UnquoteValues),
     106, uint(QtLua::TableGridModel::Editable),
     115, uint(QtLua::TableGridModel::EditFixedType),
     129, uint(QtLua::TableGridModel::EditLuaEval),
     141, uint(QtLua::TableGridModel::EditInsertRow),
     155, uint(QtLua::TableGridModel::EditInsertCol),
     169, uint(QtLua::TableGridModel::EditRemoveRow),
     183, uint(QtLua::TableGridModel::EditRemoveCol),
      43, uint(QtLua::TableGridModel::NumKeysCols),
      55, uint(QtLua::TableGridModel::NumKeysRows),
      67, uint(QtLua::TableGridModel::RowColSwap),
      78, uint(QtLua::TableGridModel::UnquoteHeader),
      92, uint(QtLua::TableGridModel::UnquoteValues),
     106, uint(QtLua::TableGridModel::Editable),
     115, uint(QtLua::TableGridModel::EditFixedType),
     129, uint(QtLua::TableGridModel::EditLuaEval),
     141, uint(QtLua::TableGridModel::EditInsertRow),
     155, uint(QtLua::TableGridModel::EditInsertCol),
     169, uint(QtLua::TableGridModel::EditRemoveRow),
     183, uint(QtLua::TableGridModel::EditRemoveCol),

       0        // eod
};

static const char qt_meta_stringdata_QtLua__TableGridModel[] = {
    "QtLua::TableGridModel\0Attribute\0"
    "Attributes\0NumKeysCols\0NumKeysRows\0"
    "RowColSwap\0UnquoteHeader\0UnquoteValues\0"
    "Editable\0EditFixedType\0EditLuaEval\0"
    "EditInsertRow\0EditInsertCol\0EditRemoveRow\0"
    "EditRemoveCol\0"
};

const QMetaObject QtLua::TableGridModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_QtLua__TableGridModel,
      qt_meta_data_QtLua__TableGridModel, 0 }
};

const QMetaObject *QtLua::TableGridModel::metaObject() const
{
    return &staticMetaObject;
}

void *QtLua::TableGridModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtLua__TableGridModel))
        return static_cast<void*>(const_cast< TableGridModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int QtLua::TableGridModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
