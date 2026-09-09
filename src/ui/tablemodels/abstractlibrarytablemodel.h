#ifndef ABSTRACTLIBRARYTABLEMODEL_H
#define ABSTRACTLIBRARYTABLEMODEL_H

#include <QAbstractTableModel>


class AbstractLibraryTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    using QAbstractTableModel::QAbstractTableModel;

    virtual int idAt(int row) const = 0;
};

#endif // ABSTRACTLIBRARYTABLEMODEL_H
