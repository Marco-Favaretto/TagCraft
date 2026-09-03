#ifndef TRACKEDITMODEL_H
#define TRACKEDITMODEL_H

#include <QHash>
#include <QVariant>

#include "model/track.h"
#include "dto/trackdto.h"
#include "controller/librarycontroller.h"
#include "ui/editmodels/abstracteditmodel.h"

class TrackEditModel : public AbstractEditModel {
    Q_OBJECT
public:
    explicit TrackEditModel(const Track& track, LibraryController* library, QObject* parent = nullptr);

    ViewMode viewMode() const override;
    QString windowTitle() const override;
    QString artworkHash() const override;
    QList<EditField> fields() const override;

    TrackDto buildDto(const QHash<QString, QVariant>& changedValues) const;

    static const QString KeyTitle;
    static const QString KeyArtist;
    static const QString KeyAlbum;
    static const QString KeyGenre;
    static const QString KeyTrackNumber;
    static const QString KeyYear;

private:
    Track m_track;
    LibraryController* library;
};

#endif // TRACKEDITMODEL_H
