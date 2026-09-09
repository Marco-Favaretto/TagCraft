#include "ui/editmodels/genreeditmodel.h"

const QString GenreEditModel::KeyName = "name";

GenreEditModel::GenreEditModel(const Genre& genre, QObject* parent)
    : AbstractEditModel(parent)
    , m_genre(genre)
{}

ViewMode GenreEditModel::viewMode() const {
    return ViewMode::Genres;
}

QString GenreEditModel::windowTitle() const {
    return QString("Edit Genre");
}

QList<EditField> GenreEditModel::fields() const {
    QList<EditField> result;

    EditField name;
    name.key = KeyName;
    name.label = "Name:";
    name.value = m_genre.name();
    name.editable = true;
    name.type = EditField::Type::Text;
    result.append(name);

    return result;
}

int GenreEditModel::entityId() const {
    return m_genre.id();
}

QString GenreEditModel::buildName(const QHash<QString, QVariant>& changedValues) const {
    return changedValues.contains(KeyName) ? changedValues.value(KeyName).toString() : m_genre.name();
}
