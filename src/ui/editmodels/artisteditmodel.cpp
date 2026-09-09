#include "ui/editmodels/artisteditmodel.h"

const QString ArtistEditModel::KeyName = "name";

ArtistEditModel::ArtistEditModel(const Artist& artist, QObject* parent)
    : AbstractEditModel(parent)
    , m_artist(artist)
{}

ViewMode ArtistEditModel::viewMode() const {
    return ViewMode::Artists;
}

QString ArtistEditModel::windowTitle() const {
    return QString("Edit Artist");
}

QList<EditField> ArtistEditModel::fields() const {
    QList<EditField> result;

    EditField name;
    name.key = KeyName;
    name.label = "Name:";
    name.value = m_artist.name();
    name.editable = true;
    name.type = EditField::Type::Text;
    result.append(name);

    return result;
}

int ArtistEditModel::entityId() const {
    return m_artist.id();
}

QString ArtistEditModel::buildName(const QHash<QString, QVariant>& changedValues) const {
    return changedValues.contains(KeyName) ? changedValues.value(KeyName).toString() : m_artist.name();
}
