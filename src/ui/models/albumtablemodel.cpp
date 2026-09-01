#include "albumtablemodel.h"

AlbumTableModel::AlbumTableModel(QObject* parent)
    : AbstractLibraryTableModel(parent)
{}

void AlbumTableModel::setRows(const QList<Row>& rows) {
    beginResetModel();
    m_rows = rows;
    endResetModel();
}

int AlbumTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_rows.size();
}

int AlbumTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant AlbumTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_rows.size()) return {};

    const Row& row = m_rows.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case Title:
                return row.title;
            case Artist:
                return row.artistName;
            case Year:
                if (role == Qt::EditRole) return row.year;
                return row.year > 0 ? QString::number(row.year) : QStringLiteral("-");
            default:
                return {};
        }
    }

    return {};
}

QVariant AlbumTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    switch (section) {
        case Title: return "Title";
        case Artist: return "Artist";
        case Year: return "Year";
        default: return {};
    }
}

int AlbumTableModel::idAt(int row) const {
    if (row < 0 || row >= m_rows.size()) return -1;
    return m_rows.at(row).id;
}
