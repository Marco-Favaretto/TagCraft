#ifndef TRACKTABLEMODEL_H
#define TRACKTABLEMODEL_H

#include <QList>

#include "abstractlibrarytablemodel.h"
#include "model/track.h"

// Colonne: Title | Artist | Album | Track # | Year
class TrackTableModel : public AbstractLibraryTableModel {
    Q_OBJECT
public:
    struct Row {
        int id;
        QString title;
        QString artistName;
        QString albumName;
        int trackNumber; // 0 se assente (poi in vista -)
        int year;        // 0 se assente (poi in vista -)
    };

    explicit TrackTableModel(QObject* parent = nullptr);

    void setRows(const QList<Row>& rows);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int idAt(int row) const override;

private:
    enum Column { Title = 0, Artist, Album, TrackNumber, Year, ColumnCount };

    QList<Row> m_rows;
};

#endif // TRACKTABLEMODEL_H
