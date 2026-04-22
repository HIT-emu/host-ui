/****************************************************************************
** Meta object code from reading C++ file 'emdevicecommunicator.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "src/emdevicecommunicator.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'emdevicecommunicator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_EmDeviceCommunicator_t {
    uint offsetsAndSizes[26];
    char stringdata0[21];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[6];
    char stringdata5[11];
    char stringdata6[4];
    char stringdata7[18];
    char stringdata8[9];
    char stringdata9[11];
    char stringdata10[15];
    char stringdata11[3];
    char stringdata12[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_EmDeviceCommunicator_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_EmDeviceCommunicator_t qt_meta_stringdata_EmDeviceCommunicator = {
    {
        QT_MOC_LITERAL(0, 20),  // "EmDeviceCommunicator"
        QT_MOC_LITERAL(21, 17),  // "telemetryReceived"
        QT_MOC_LITERAL(39, 0),  // ""
        QT_MOC_LITERAL(40, 14),  // "TelemetryFrame"
        QT_MOC_LITERAL(55, 5),  // "frame"
        QT_MOC_LITERAL(61, 10),  // "logMessage"
        QT_MOC_LITERAL(72, 3),  // "msg"
        QT_MOC_LITERAL(76, 17),  // "sendTargetVoltage"
        QT_MOC_LITERAL(94, 8),  // "uint32_t"
        QT_MOC_LITERAL(103, 10),  // "voltage_mv"
        QT_MOC_LITERAL(114, 14),  // "sendPowerState"
        QT_MOC_LITERAL(129, 2),  // "on"
        QT_MOC_LITERAL(132, 11)   // "onReadyRead"
    },
    "EmDeviceCommunicator",
    "telemetryReceived",
    "",
    "TelemetryFrame",
    "frame",
    "logMessage",
    "msg",
    "sendTargetVoltage",
    "uint32_t",
    "voltage_mv",
    "sendPowerState",
    "on",
    "onReadyRead"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_EmDeviceCommunicator[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,
       5,    1,   47,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    1,   50,    2, 0x0a,    5 /* Public */,
      10,    1,   53,    2, 0x0a,    7 /* Public */,
      12,    0,   56,    2, 0x08,    9 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject EmDeviceCommunicator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_EmDeviceCommunicator.offsetsAndSizes,
    qt_meta_data_EmDeviceCommunicator,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_EmDeviceCommunicator_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<EmDeviceCommunicator, std::true_type>,
        // method 'telemetryReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const TelemetryFrame &, std::false_type>,
        // method 'logMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'sendTargetVoltage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint32_t, std::false_type>,
        // method 'sendPowerState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void EmDeviceCommunicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<EmDeviceCommunicator *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->telemetryReceived((*reinterpret_cast< std::add_pointer_t<TelemetryFrame>>(_a[1]))); break;
        case 1: _t->logMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->sendTargetVoltage((*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[1]))); break;
        case 3: _t->sendPowerState((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->onReadyRead(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (EmDeviceCommunicator::*)(const TelemetryFrame & );
            if (_t _q_method = &EmDeviceCommunicator::telemetryReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (EmDeviceCommunicator::*)(const QString & );
            if (_t _q_method = &EmDeviceCommunicator::logMessage; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *EmDeviceCommunicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EmDeviceCommunicator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EmDeviceCommunicator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EmDeviceCommunicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void EmDeviceCommunicator::telemetryReceived(const TelemetryFrame & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EmDeviceCommunicator::logMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
