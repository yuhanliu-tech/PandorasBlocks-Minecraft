/****************************************************************************
** Meta object code from reading C++ file 'playerinfo.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../assignment_package/src/playerinfo.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playerinfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSPlayerInfoENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSPlayerInfoENDCLASS = QtMocHelpers::stringData(
    "PlayerInfo",
    "slot_setPosText",
    "",
    "slot_setVelText",
    "slot_setAccText",
    "slot_setLookText",
    "slot_setChunkText",
    "slot_setZoneText"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSPlayerInfoENDCLASS_t {
    uint offsetsAndSizes[16];
    char stringdata0[11];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[16];
    char stringdata5[17];
    char stringdata6[18];
    char stringdata7[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSPlayerInfoENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSPlayerInfoENDCLASS_t qt_meta_stringdata_CLASSPlayerInfoENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "PlayerInfo"
        QT_MOC_LITERAL(11, 15),  // "slot_setPosText"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 15),  // "slot_setVelText"
        QT_MOC_LITERAL(44, 15),  // "slot_setAccText"
        QT_MOC_LITERAL(60, 16),  // "slot_setLookText"
        QT_MOC_LITERAL(77, 17),  // "slot_setChunkText"
        QT_MOC_LITERAL(95, 16)   // "slot_setZoneText"
    },
    "PlayerInfo",
    "slot_setPosText",
    "",
    "slot_setVelText",
    "slot_setAccText",
    "slot_setLookText",
    "slot_setChunkText",
    "slot_setZoneText"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSPlayerInfoENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x0a,    1 /* Public */,
       3,    1,   53,    2, 0x0a,    3 /* Public */,
       4,    1,   56,    2, 0x0a,    5 /* Public */,
       5,    1,   59,    2, 0x0a,    7 /* Public */,
       6,    1,   62,    2, 0x0a,    9 /* Public */,
       7,    1,   65,    2, 0x0a,   11 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject PlayerInfo::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSPlayerInfoENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSPlayerInfoENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSPlayerInfoENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PlayerInfo, std::true_type>,
        // method 'slot_setPosText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slot_setVelText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slot_setAccText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slot_setLookText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slot_setChunkText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slot_setZoneText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>
    >,
    nullptr
} };

void PlayerInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlayerInfo *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->slot_setPosText((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->slot_setVelText((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->slot_setAccText((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->slot_setLookText((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->slot_setChunkText((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->slot_setZoneText((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *PlayerInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlayerInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSPlayerInfoENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PlayerInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
