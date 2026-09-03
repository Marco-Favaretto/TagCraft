#ifndef ABSTRACTEDITMODEL_H
#define ABSTRACTEDITMODEL_H


#include <QList>
#include <QObject>
#include <QString>

#include "dto/editfield.h"
#include "dto/viewmode.h"

class AbstractEditModel : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;
    ~AbstractEditModel() override = default;

    virtual ViewMode viewMode() const = 0;

    virtual QString windowTitle() const = 0;

    virtual QString artworkHash() const = 0;

    virtual QList<EditField> fields() const = 0;
};

#endif // ABSTRACTEDITMODEL_H
