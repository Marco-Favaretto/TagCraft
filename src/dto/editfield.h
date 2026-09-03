#ifndef EDITFIELD_H
#define EDITFIELD_H

#include <QString>
#include <QVariant>

struct EditField {
    enum class Type {
        Text,
        Int
    };
    
    QString key;
    QString label;
    QVariant value;
    bool editable = false;
    Type type = Type::Text;
    
    int emptyIntValue = 0;
};

#endif // EDITFIELD_H