/****************************************************************************
** Meta object code from reading C++ file 'qtluaitemselectionmodel.hh'
**
** Created: Thu Jan 21 18:28:27 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qtluaitemselectionmodel.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtluaitemselectionmodel.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtLua__ItemSelectionModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      41,   27,   26,   26, 0x0a,
     115,   97,   26,   26, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QtLua__ItemSelectionModel[] = {
    "QtLua::ItemSelectionModel\0\0index,command\0"
    "select(QModelIndex,QItemSelectionModel::SelectionFlags)\0"
    "selection,command\0"
    "select(QItemSelection,QItemSelectionModel::SelectionFlags)\0"
};

const QMetaObject QtLua::ItemSelectionModel::staticMetaObject = {
    { &QItemSelectionModel::staticMetaObject, qt_meta_stringdata_QtLua__ItemSelectionModel,
      qt_meta_data_QtLua__ItemSelectionModel, 0 }
};

const QMetaObject *QtLua::ItemSelectionModel::metaObject() const
{
    return &staticMetaObject;
}

void *QtLua::ItemSelectionModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtLua__ItemSelectionModel))
        return static_cast<void*>(const_cast< ItemSelectionModel*>(this));
    return QItemSelectionModel::qt_metacast(_clname);
}

int QtLua::ItemSelectionModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QItemSelectionModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: select((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< QItemSelectionModel::SelectionFlags(*)>(_a[2]))); break;
        case 1: select((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< QItemSelectionModel::SelectionFlags(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
