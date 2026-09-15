#pragma once

#include <QList>
#include <functional>

#include "model/track.h"
#include "dto/trackdto.h"
#include "controller/librarycontroller.h"
#include "ui/editmodels/abstracteditmodel.h"

class BatchTrackEditModel : public AbstractEditModel {
    Q_OBJECT
public:
    explicit BatchTrackEditModel(const QList<Track>& tracks, LibraryController* library, QObject* parent = nullptr);

    ViewMode viewMode() const override { return ViewMode::Tracks; }
    QString windowTitle() const override;
    QString artworkHash() const override;
    QList<EditField> fields() const override;
    bool supportsArtwork() const override { return true; }
    bool supportsCleanTags() const override { return true; }

    static const QString MixedValuesPlaceholder;

private:
    QString resolvedArtistName(const Track& t) const;
    QString resolvedAlbumName(const Track& t) const;
    QString resolvedGenreName(const Track& t) const;
    QString resolvedYear(const Track& t) const;

    // Ritorna il valore comune a tutte le tracce per la funzione data, o
    // MixedValuesPlaceholder se almeno una diverge. Lista vuota -> stringa
    // vuota (non dovrebbe accadere: il batch edit richiede >=1 traccia).
    QString commonValue(std::function<QString(const Track&)> resolver) const;

    QList<Track> m_tracks;
    LibraryController* m_library; // non posseduto
};
