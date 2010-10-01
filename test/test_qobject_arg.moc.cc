/****************************************************************************
** Meta object code from reading C++ file 'test_qobject_arg.hh'
**
** Created: Fri Jan 22 19:47:04 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "test_qobject_arg.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'test_qobject_arg.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MyObjectUD[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      15,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   12,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MyObjectUD[] = {
    "MyObjectUD\0\0ud\0ud_arg(Ref<UserData>)\0"
    "ud_slot(Ref<UserData>)\0"
};

const QMetaObject MyObjectUD::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MyObjectUD,
      qt_meta_data_MyObjectUD, 0 }
};

const QMetaObject *MyObjectUD::metaObject() const
{
    return &staticMetaObject;
}

void *MyObjectUD::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyObjectUD))
        return static_cast<void*>(const_cast< MyObjectUD*>(this));
    return QObject::qt_metacast(_clname);
}

int MyObjectUD::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: ud_arg((*reinterpret_cast< Ref<UserData>(*)>(_a[1]))); break;
        case 1: ud_slot((*reinterpret_cast< Ref<UserData>(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MyObjectUD::ud_arg(Ref<UserData> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_MyObjectQO[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      14,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   31,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MyObjectQO[] = {
    "MyObjectQO\0\0o\0qo_arg(QObject*)\0qo\0"
    "qo_slot(QObject*)\0"
};

const QMetaObject MyObjectQO::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MyObjectQO,
      qt_meta_data_MyObjectQO, 0 }
};

const QMetaObject *MyObjectQO::metaObject() const
{
    return &staticMetaObject;
}

void *MyObjectQO::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyObjectQO))
        return static_cast<void*>(const_cast< MyObjectQO*>(this));
    return QObject::qt_metacast(_clname);
}

int MyObjectQO::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: qo_arg((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 1: qo_slot((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MyObjectQO::qo_arg(QObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
