#ifndef ALBUMTABLEMODEL_H
#define ALBUMTABLEMODEL_H

#include <QList>

#include "abstractlibrarytablemodel.h"

// Colonne: Title | Artist | Year
class AlbumTableModel : public AbstractLibraryTableModel {
    Q_OBJECT
public:
    struct Row {
        int id;
        QString title;
        QString artistName; // "Various Artists" se compilation (artist_id = Unknown)
        int year;            // 0 se assente/misto (poi corretto in - nella vista)
    };

    explicit AlbumTableModel(QObject* parent = nullptr);

    void setRows(const QList<Row>& rows);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int idAt(int row) const override;

private:
    enum Column { Title = 0, Artist, Year, ColumnCount };

    QList<Row> m_rows;
};

#endif // ALBUMTABLEMODEL_H
