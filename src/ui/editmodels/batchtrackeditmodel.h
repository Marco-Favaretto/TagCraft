#pragma once

#include <QList>
#include <functional>

#include "model/track.h"
#include "dto/trackdto.h"
#include "controller/librarycontroller.h"
#include "ui/editmodels/abstracteditmodel.h"

// Modello di editing per una selezione multipla di Track (batch edit).
//
// Campi editabili: SOLO artistName, albumName, genreName (risolti dagli id
// come per TrackEditModel/AlbumEditModel). Niente Title/TrackNumber/Year:
// non ha senso forzare un valore comune su tracce eterogenee, e niente
// campo Path (non ce n'e' uno comune).
//
// Mixed-value detection: se le tracce della selezione non concordano su un
// campo, quel campo mostra un placeholder ("< valori multipli >") invece
// del valore di una singola traccia, e viene trattato come "nessun valore
// comune" - va sempre riscritto esplicitamente dall'utente per essere
// applicato, mai precompilato con un valore arbitrario della selezione.
// Stessa logica per l'artwork: se gli hash divergono, artworkHash() ritorna
// vuoto e il dialog mostra il placeholder di default (viewMode() = Tracks).
//
// A differenza di TrackEditModel/AlbumEditModel, questo model NON espone
// un buildDto()/buildResult(): il chiamante (MainWindow) passa direttamente
// EditMetadataDialog::changedValues() a
// AppController::applyMetadataToTracks(tracks, changed) - i soli campi
// effettivamente scritti dall'utente vengono applicati, il resto di ogni
// traccia resta intatto (stessa garanzia di TrackDto::mergeWithChanges).
// "Clean Tags" per il batch segue invece il flusso esistente
// (AppController::requestCleanTagsBatch sui relativePath selezionati), non
// passa da questo model.
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

    // Ritorna il valore comune a tutte le tracce per la funzione data, o
    // MixedValuesPlaceholder se almeno una diverge. Lista vuota -> stringa
    // vuota (non dovrebbe accadere: il batch edit richiede >=1 traccia).
    QString commonValue(std::function<QString(const Track&)> resolver) const;

    QList<Track> m_tracks;
    LibraryController* m_library; // non posseduto
};
