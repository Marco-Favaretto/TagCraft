#pragma once

#include <QHash>
#include <QVariant>

#include "model/artist.h"
#include "ui/editmodels/abstracteditmodel.h"

class ArtistEditModel : public AbstractEditModel {
    Q_OBJECT
public:
    explicit ArtistEditModel(const Artist& artist, QObject* parent = nullptr);

    ViewMode viewMode() const override;
    QString windowTitle() const override;
    QString artworkHash() const override { return QString(); }
    QList<EditField> fields() const override;
    bool supportsArtwork() const override { return false; }
    bool supportsCleanTags() const override { return false; }

    int entityId() const;

    QString buildName(const QHash<QString, QVariant>& changedValues) const;

    static const QString KeyName;

private:
    Artist m_artist;
};
