#ifndef TRACKTABLEMODEL_H
#define TRACKTABLEMODEL_H

#include <QList>
#include <QSet>

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

    void setSelectionModeEnabled(bool enabled);
    bool selectionModeEnabled() const;
    QList<int> selectedIds() const;
    void setIdChecked(int id, bool checked);
    bool isIdChecked(int id) const;
    void selectAllVisible();
    void deselectAll();
    int checkboxColumn() const;
    
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

private:
    enum Column { Title = 0, Artist, Album, TrackNumber, Year, ColumnCount };

    QList<Row> m_rows;
    bool m_selectionMode = false;
    QSet<int> m_selectedIds;
};

#endif // TRACKTABLEMODEL_H
