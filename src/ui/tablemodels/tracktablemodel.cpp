#include "tracktablemodel.h"

TrackTableModel::TrackTableModel(QObject* parent)
    : AbstractLibraryTableModel(parent)
{}

void TrackTableModel::setRows(const QList<Row>& rows) {
    beginResetModel();
    m_rows = rows;
    m_selectionMode = false;
    m_selectedIds.clear();
    endResetModel();
}

int TrackTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_rows.size();
}

int TrackTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_selectionMode ? ColumnCount + 1 : ColumnCount;
}

QVariant TrackTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_rows.size()) return {};

    const Row& row = m_rows.at(index.row());

    if (role == Qt::CheckStateRole && m_selectionMode && index.column() == ColumnCount) {
        return m_selectedIds.contains(row.id) ? Qt::Checked : Qt::Unchecked;
    }

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
        default: return QString();
    }
}

int TrackTableModel::idAt(int row) const {
    if (row < 0 || row >= m_rows.size()) return -1;
    return m_rows.at(row).id;
}

Qt::ItemFlags TrackTableModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (m_selectionMode && index.column() == ColumnCount) {
        f |= Qt::ItemIsUserCheckable;
    }
    return f;
}

bool TrackTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role == Qt::CheckStateRole && m_selectionMode && index.column() == ColumnCount
        && index.row() >= 0 && index.row() < m_rows.size()) {
            
        const int id = m_rows.at(index.row()).id;
        if (value.toInt() == Qt::Checked) m_selectedIds.insert(id);
        else m_selectedIds.remove(id);
        emit dataChanged(index, index, {Qt::CheckStateRole});
        return true;
    }
    return false;
}

void TrackTableModel::setSelectionModeEnabled(bool enabled) {
    if (m_selectionMode == enabled) return;
    beginResetModel(); // cambia columnCount()
    m_selectionMode = enabled;
    m_selectedIds.clear();
    endResetModel();
}

bool TrackTableModel::selectionModeEnabled() const {
    return m_selectionMode;
}

QList<int> TrackTableModel::selectedIds() const {
    return m_selectedIds.values();
}

void TrackTableModel::setIdChecked(int id, bool checked) {
    if (checked) m_selectedIds.insert(id);
    else m_selectedIds.remove(id);

    for (int r = 0; r < m_rows.size(); ++r) {
        if (m_rows.at(r).id == id) {
            const QModelIndex idx = index(r, ColumnCount);
            emit dataChanged(idx, idx, {Qt::CheckStateRole});
            break;
        }
    }
}

bool TrackTableModel::isIdChecked(int id) const {
    return m_selectedIds.contains(id);
}

void TrackTableModel::selectAllVisible() {
    if (!m_selectionMode) return;
    m_selectedIds.clear();
    for (const Row& r : m_rows) m_selectedIds.insert(r.id);
    if (!m_rows.isEmpty()) {
        emit dataChanged(index(0, ColumnCount), index(m_rows.size() - 1, ColumnCount), {Qt::CheckStateRole});
    }
}

void TrackTableModel::deselectAll() {
    if (m_selectedIds.isEmpty()) return;
    m_selectedIds.clear();
    if (!m_rows.isEmpty()) {
        emit dataChanged(index(0, ColumnCount), index(m_rows.size() - 1, ColumnCount), {Qt::CheckStateRole});
    }
}

int TrackTableModel::checkboxColumn() const { 
    return ColumnCount; 
} 
