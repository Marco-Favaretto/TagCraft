#include "artisttablemodel.h"

ArtistTableModel::ArtistTableModel(QObject* parent)
    : AbstractLibraryTableModel(parent)
{}

void ArtistTableModel::setRows(const QList<Row>& rows) {
    beginResetModel();
    m_rows = rows;
    endResetModel();
}

int ArtistTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_rows.size();
}

int ArtistTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant ArtistTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_rows.size()) return {};

    const Row& row = m_rows.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == Name) return row.name;
    }

    return {};
}

QVariant ArtistTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    if (section == Name) return "Name";
    return {};
}

int ArtistTableModel::idAt(int row) const {
    if (row < 0 || row >= m_rows.size()) return -1;
    return m_rows.at(row).id;
}
