/****************************************************************************
** Meta object code from reading C++ file 'qtluatabledialog.hh'
**
** Created: Tue Jan 26 18:16:14 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qtluatabledialog.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtluatabledialog.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtLua__TableDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   12, // methods
       2,   62, // properties
       1,   68, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      27,   19,   19,   19, 0x08,
      41,   19,   19,   19, 0x08,
      61,   55,   19,   19, 0x08,
      95,   19,   19,   19, 0x08,
     111,   19,   19,   19, 0x08,
     129,   19,   19,   19, 0x08,
     147,   19,   19,   19, 0x08,
     168,   19,   19,   19, 0x08,
     189,   55,   19,   19, 0x08,

 // properties: name, type, flags
     228,  223, 0x01095003,
     251,  245, 0x87095003,

 // enums: name, flags, count, data
     272, 0x0,    3,   72,

 // enum data: key, value
     281, uint(QtLua::TableDialog::TreeTreeView),
     294, uint(QtLua::TableDialog::TreeTableView),
     308, uint(QtLua::TableDialog::GridTableView),

       0        // eod
};

static const char qt_meta_stringdata_QtLua__TableDialog[] = {
    "QtLua::TableDialog\0\0edit()\0tree_insert()\0"
    "tree_remove()\0index\0"
    "tree_current_changed(QModelIndex)\0"
    "tree_expanded()\0grid_insert_row()\0"
    "grid_remove_row()\0grid_insert_column()\0"
    "grid_remove_column()\0"
    "grid_current_changed(QModelIndex)\0"
    "bool\0resize_on_expand\0float\0"
    "column_margin_factor\0ViewType\0"
    "TreeTreeView\0TreeTableView\0GridTableView\0"
};

const QMetaObject QtLua::TableDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QtLua__TableDialog,
      qt_meta_data_QtLua__TableDialog, 0 }
};

const QMetaObject *QtLua::TableDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QtLua::TableDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtLua__TableDialog))
        return static_cast<void*>(const_cast< TableDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QtLua::TableDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: edit(); break;
        case 1: tree_insert(); break;
        case 2: tree_remove(); break;
        case 3: tree_current_changed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: tree_expanded(); break;
        case 5: grid_insert_row(); break;
        case 6: grid_remove_row(); break;
        case 7: grid_insert_column(); break;
        case 8: grid_remove_column(); break;
        case 9: grid_current_changed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = get_resize_on_expand(); break;
        case 1: *reinterpret_cast< float*>(_v) = get_column_margin_factor(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: set_resize_on_expand(*reinterpret_cast< bool*>(_v)); break;
        case 1: set_column_margin_factor(*reinterpret_cast< float*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
