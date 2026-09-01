#include "genretablemodel.h"

GenreTableModel::GenreTableModel(QObject* parent)
    : AbstractLibraryTableModel(parent)
{}

void GenreTableModel::setRows(const QList<Row>& rows) {
    beginResetModel();
    m_rows = rows;
    endResetModel();
}

int GenreTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_rows.size();
}

int GenreTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant GenreTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_rows.size()) return {};

    const Row& row = m_rows.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == Name) return row.name;
    }

    return {};
}

QVariant GenreTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    if (section == Name) return "Name";
    return {};
}

int GenreTableModel::idAt(int row) const {
    if (row < 0 || row >= m_rows.size()) return -1;
    return m_rows.at(row).id;
}
