#include "tracktablemodel.h"

TrackTableModel::TrackTableModel(QObject* parent)
    : AbstractLibraryTableModel(parent)
{}

void TrackTableModel::setRows(const QList<Row>& rows) {
    beginResetModel();
    m_rows = rows;
    endResetModel();
}

int TrackTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_rows.size();
}

int TrackTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant TrackTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_rows.size()) return {};

    const Row& row = m_rows.at(index.row());

    // EditRole separato da DisplayRole per le colonne numeriche: serve al
    // QSortFilterProxyModel per ordinare per valore reale, non per testo
    // (altrimenti "10" finirebbe prima di "9" nell'ordinamento alfabetico).
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case Title:
                return row.title.isEmpty() ? QStringLiteral("-") : row.title;
            case Artist:
                return row.artistName;
            case Album:
                return row.albumName;
            case TrackNumber:
                if (role == Qt::EditRole) return row.trackNumber;
                return row.trackNumber > 0 ? QString::number(row.trackNumber) : QStringLiteral("-");
            case Year:
                if (role == Qt::EditRole) return row.year;
                return row.year > 0 ? QString::number(row.year) : QStringLiteral("-");
            default:
                return {};
        }
    }

    return {};
}

QVariant TrackTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    switch (section) {
        case Title: return "Title";
        case Artist: return "Artist";
        case Album: return "Album";
        case TrackNumber: return "#";
        case Year: return "Year";
        default: return {};
    }
}

int TrackTableModel::idAt(int row) const {
    if (row < 0 || row >= m_rows.size()) return -1;
    return m_rows.at(row).id;
}
