/****************************************************************************
** Meta object code from reading C++ file 'radxtranslator.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "radxtranslator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'radxtranslator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RadxTranslator[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   36,   40,   40, 0x0a,
      41,   36,   40,   40, 0x0a,
      62,   36,   40,   40, 0x0a,
      83,   36,   40,   40, 0x0a,
     107,   40,   40,   40, 0x0a,
     122,   40,   40,   40, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RadxTranslator[] = {
    "RadxTranslator\0select1dVar(QString)\0"
    "str\0\0select2dVar(QString)\0"
    "select3dVar(QString)\0selectColorMap(QString)\0"
    "update_frame()\0updateSliders()\0"
};

void RadxTranslator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RadxTranslator *_t = static_cast<RadxTranslator *>(_o);
        switch (_id) {
        case 0: _t->select1dVar((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->select2dVar((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->select3dVar((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->selectColorMap((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->update_frame(); break;
        case 5: _t->updateSliders(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RadxTranslator::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RadxTranslator::staticMetaObject = {
    { &BaseTranslator::staticMetaObject, qt_meta_stringdata_RadxTranslator,
      qt_meta_data_RadxTranslator, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RadxTranslator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RadxTranslator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RadxTranslator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RadxTranslator))
        return static_cast<void*>(const_cast< RadxTranslator*>(this));
    return BaseTranslator::qt_metacast(_clname);
}

int RadxTranslator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseTranslator::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
