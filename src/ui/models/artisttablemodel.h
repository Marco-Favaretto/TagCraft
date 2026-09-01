#ifndef ARTISTTABLEMODEL_H
#define ARTISTTABLEMODEL_H

#include <QList>

#include "abstractlibrarytablemodel.h"

// Colonne: Name
class ArtistTableModel : public AbstractLibraryTableModel {
    Q_OBJECT
public:
    struct Row {
        int id;
        QString name;
    };

    explicit ArtistTableModel(QObject* parent = nullptr);

    void setRows(const QList<Row>& rows);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int idAt(int row) const override;

private:
    enum Column { Name = 0, ColumnCount };

    QList<Row> m_rows;
};

#endif // ARTISTTABLEMODEL_H
