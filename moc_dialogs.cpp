/****************************************************************************
** Meta object code from reading C++ file 'dialogs.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "dialogs.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialogs.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UdpConfigDialog_t {
    QByteArrayData data[18];
    char stringdata0[239];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UdpConfigDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UdpConfigDialog_t qt_meta_stringdata_UdpConfigDialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "UdpConfigDialog"
QT_MOC_LITERAL(1, 16, 23), // "connectionStatusChanged"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 9), // "connected"
QT_MOC_LITERAL(4, 51, 12), // "dataReceived"
QT_MOC_LITERAL(5, 64, 13), // "DetectionData"
QT_MOC_LITERAL(6, 78, 4), // "data"
QT_MOC_LITERAL(7, 83, 13), // "connectToHost"
QT_MOC_LITERAL(8, 97, 18), // "disconnectFromHost"
QT_MOC_LITERAL(9, 116, 25), // "onConnectionStatusChanged"
QT_MOC_LITERAL(10, 142, 22), // "onNewDetectionReceived"
QT_MOC_LITERAL(11, 165, 9), // "detection"
QT_MOC_LITERAL(12, 175, 15), // "onErrorOccurred"
QT_MOC_LITERAL(13, 191, 5), // "error"
QT_MOC_LITERAL(14, 197, 19), // "onStatisticsUpdated"
QT_MOC_LITERAL(15, 217, 8), // "received"
QT_MOC_LITERAL(16, 226, 7), // "dropped"
QT_MOC_LITERAL(17, 234, 4) // "rate"

    },
    "UdpConfigDialog\0connectionStatusChanged\0"
    "\0connected\0dataReceived\0DetectionData\0"
    "data\0connectToHost\0disconnectFromHost\0"
    "onConnectionStatusChanged\0"
    "onNewDetectionReceived\0detection\0"
    "onErrorOccurred\0error\0onStatisticsUpdated\0"
    "received\0dropped\0rate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UdpConfigDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   60,    2, 0x08 /* Private */,
       8,    0,   61,    2, 0x08 /* Private */,
       9,    1,   62,    2, 0x08 /* Private */,
      10,    1,   65,    2, 0x08 /* Private */,
      12,    1,   68,    2, 0x08 /* Private */,
      14,    3,   71,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5,   11,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Double,   15,   16,   17,

       0        // eod
};

void UdpConfigDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UdpConfigDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connectionStatusChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->dataReceived((*reinterpret_cast< const DetectionData(*)>(_a[1]))); break;
        case 2: _t->connectToHost(); break;
        case 3: _t->disconnectFromHost(); break;
        case 4: _t->onConnectionStatusChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->onNewDetectionReceived((*reinterpret_cast< const DetectionData(*)>(_a[1]))); break;
        case 6: _t->onErrorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onStatisticsUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UdpConfigDialog::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UdpConfigDialog::connectionStatusChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UdpConfigDialog::*)(const DetectionData & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UdpConfigDialog::dataReceived)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UdpConfigDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_UdpConfigDialog.data,
    qt_meta_data_UdpConfigDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UdpConfigDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UdpConfigDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UdpConfigDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int UdpConfigDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
void UdpConfigDialog::connectionStatusChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UdpConfigDialog::dataReceived(const DetectionData & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_OutputConfigDialog_t {
    QByteArrayData data[1];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OutputConfigDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OutputConfigDialog_t qt_meta_stringdata_OutputConfigDialog = {
    {
QT_MOC_LITERAL(0, 0, 18) // "OutputConfigDialog"

    },
    "OutputConfigDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OutputConfigDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void OutputConfigDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject OutputConfigDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_OutputConfigDialog.data,
    qt_meta_data_OutputConfigDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *OutputConfigDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OutputConfigDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OutputConfigDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int OutputConfigDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_AngleCorrectionDialog_t {
    QByteArrayData data[1];
    char stringdata0[22];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AngleCorrectionDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AngleCorrectionDialog_t qt_meta_stringdata_AngleCorrectionDialog = {
    {
QT_MOC_LITERAL(0, 0, 21) // "AngleCorrectionDialog"

    },
    "AngleCorrectionDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AngleCorrectionDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void AngleCorrectionDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject AngleCorrectionDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_AngleCorrectionDialog.data,
    qt_meta_data_AngleCorrectionDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AngleCorrectionDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AngleCorrectionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AngleCorrectionDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int AngleCorrectionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_AmplificationDialog_t {
    QByteArrayData data[10];
    char stringdata0[143];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AmplificationDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AmplificationDialog_t qt_meta_stringdata_AmplificationDialog = {
    {
QT_MOC_LITERAL(0, 0, 19), // "AmplificationDialog"
QT_MOC_LITERAL(1, 20, 20), // "amplificationChanged"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 13), // "amplification"
QT_MOC_LITERAL(4, 56, 14), // "settingsStored"
QT_MOC_LITERAL(5, 71, 22), // "onAmplificationChanged"
QT_MOC_LITERAL(6, 94, 5), // "value"
QT_MOC_LITERAL(7, 100, 20), // "onAutoEnabledChanged"
QT_MOC_LITERAL(8, 121, 7), // "enabled"
QT_MOC_LITERAL(9, 129, 13) // "storeSettings"

    },
    "AmplificationDialog\0amplificationChanged\0"
    "\0amplification\0settingsStored\0"
    "onAmplificationChanged\0value\0"
    "onAutoEnabledChanged\0enabled\0storeSettings"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AmplificationDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   43,    2, 0x08 /* Private */,
       7,    1,   46,    2, 0x08 /* Private */,
       9,    0,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,

       0        // eod
};

void AmplificationDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AmplificationDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->amplificationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->settingsStored(); break;
        case 2: _t->onAmplificationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onAutoEnabledChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->storeSettings(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AmplificationDialog::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AmplificationDialog::amplificationChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (AmplificationDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AmplificationDialog::settingsStored)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AmplificationDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_AmplificationDialog.data,
    qt_meta_data_AmplificationDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AmplificationDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AmplificationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AmplificationDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int AmplificationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void AmplificationDialog::amplificationChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AmplificationDialog::settingsStored()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_FilterConfigDialog_t {
    QByteArrayData data[1];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FilterConfigDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FilterConfigDialog_t qt_meta_stringdata_FilterConfigDialog = {
    {
QT_MOC_LITERAL(0, 0, 18) // "FilterConfigDialog"

    },
    "FilterConfigDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FilterConfigDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void FilterConfigDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject FilterConfigDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_FilterConfigDialog.data,
    qt_meta_data_FilterConfigDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FilterConfigDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FilterConfigDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FilterConfigDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int FilterConfigDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
