#pragma once

#include <QHash>
#include <QVariant>

#include "model/album.h"
#include "controller/librarycontroller.h"
#include "ui/editmodels/abstracteditmodel.h"

class AlbumEditModel : public AbstractEditModel {
    Q_OBJECT
public:
    explicit AlbumEditModel(const Album& album, LibraryController* library, QObject* parent = nullptr);

    ViewMode viewMode() const override;
    QString windowTitle() const override;
    QString artworkHash() const override;
    QList<EditField> fields() const override;
    bool supportsArtwork() const override { return true; }
    bool supportsCleanTags() const override { return false; }

    QHash<QString, QVariant> buildResult(const QHash<QString, QVariant>& changedValues) const;

    QList<Track> tracks() const;

    static const QString KeyTitle;
    static const QString KeyArtist;
    static const QString KeyGenre;
    static const QString KeyTrackNumbers;

private:
    Album m_album;
    LibraryController* m_library; // non posseduto
};
