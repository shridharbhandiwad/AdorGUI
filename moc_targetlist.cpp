/****************************************************************************
** Meta object code from reading C++ file 'targetlist.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "targetlist.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'targetlist.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TargetListWidget_t {
    QByteArrayData data[18];
    char stringdata0[229];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TargetListWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TargetListWidget_t qt_meta_stringdata_TargetListWidget = {
    {
QT_MOC_LITERAL(0, 0, 16), // "TargetListWidget"
QT_MOC_LITERAL(1, 17, 14), // "targetSelected"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 15), // "TargetDetection"
QT_MOC_LITERAL(4, 49, 6), // "target"
QT_MOC_LITERAL(5, 56, 18), // "targetCountChanged"
QT_MOC_LITERAL(6, 75, 5), // "count"
QT_MOC_LITERAL(7, 81, 13), // "statusChanged"
QT_MOC_LITERAL(8, 95, 6), // "status"
QT_MOC_LITERAL(9, 102, 14), // "onNewDetection"
QT_MOC_LITERAL(10, 117, 13), // "DetectionData"
QT_MOC_LITERAL(11, 131, 9), // "detection"
QT_MOC_LITERAL(12, 141, 14), // "refreshDisplay"
QT_MOC_LITERAL(13, 156, 19), // "onTargetItemClicked"
QT_MOC_LITERAL(14, 176, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(15, 193, 4), // "item"
QT_MOC_LITERAL(16, 198, 12), // "updateStatus"
QT_MOC_LITERAL(17, 211, 17) // "cleanupOldTargets"

    },
    "TargetListWidget\0targetSelected\0\0"
    "TargetDetection\0target\0targetCountChanged\0"
    "count\0statusChanged\0status\0onNewDetection\0"
    "DetectionData\0detection\0refreshDisplay\0"
    "onTargetItemClicked\0QListWidgetItem*\0"
    "item\0updateStatus\0cleanupOldTargets"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TargetListWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    1,   57,    2, 0x06 /* Public */,
       7,    1,   60,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   63,    2, 0x0a /* Public */,
      12,    0,   66,    2, 0x0a /* Public */,
      13,    1,   67,    2, 0x08 /* Private */,
      16,    0,   70,    2, 0x08 /* Private */,
      17,    0,   71,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::QString,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TargetListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TargetListWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->targetSelected((*reinterpret_cast< const TargetDetection(*)>(_a[1]))); break;
        case 1: _t->targetCountChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->statusChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->onNewDetection((*reinterpret_cast< const DetectionData(*)>(_a[1]))); break;
        case 4: _t->refreshDisplay(); break;
        case 5: _t->onTargetItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 6: _t->updateStatus(); break;
        case 7: _t->cleanupOldTargets(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TargetListWidget::*)(const TargetDetection & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TargetListWidget::targetSelected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TargetListWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TargetListWidget::targetCountChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TargetListWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TargetListWidget::statusChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TargetListWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_TargetListWidget.data,
    qt_meta_data_TargetListWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TargetListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TargetListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TargetListWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TargetListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void TargetListWidget::targetSelected(const TargetDetection & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TargetListWidget::targetCountChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TargetListWidget::statusChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
