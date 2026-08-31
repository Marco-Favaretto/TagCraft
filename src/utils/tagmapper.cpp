#include "utils/tagmapper.h"

#include <mpegfile.h>
#include <id3v2tag.h>
#include <attachedpictureframe.h>
#include <fileref.h>
#include <tpropertymap.h>

#include <QFileInfo>

TrackDto TagMapper::fileToDto(const QString& absolutePath, const QString& relativePath) {
    TagLib::FileRef f(absolutePath.toStdString().c_str());
    if (f.isNull() || !f.file()) {
        qDebug() << "Impossibile aprire il file:" << relativePath;
        return TrackDto();
    }

    TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>(f.file());
    if (!mpegFile) {
        qDebug() << "Il file non è un MPEG/MP3 valido";
        return TrackDto();
    }

    TagLib::ID3v2::Tag* tag = mpegFile->ID3v2Tag();
    if (!tag) {
        qDebug() << "Il file non contiene un tag ID3v2";
        return TrackDto();
    }

    TrackDto t;

    t.title      = tagStringToQString(tag->title());
    t.artistName = tagStringToQString(tag->artist());
    t.albumName  = tagStringToQString(tag->album());
    t.genreName  = tagStringToQString(tag->genre());

    t.year = tag->year();
    t.trackNumber = tag->track();
    t.relativePath = relativePath;

    
    QFileInfo fileInfo(absolutePath);
    
    t.durationSeconds = mpegFile->audioProperties() ? mpegFile->audioProperties()->lengthInSeconds() : 0;
    t.fileSize = fileInfo.size();
    t.lastModified = fileInfo.lastModified().toSecsSinceEpoch();
    
    qDebug() << t.toString();

    return t;
}

QByteArray TagMapper::extractEmbeddedCover(const QString& absolutePath) {
    TagLib::FileRef f(absolutePath.toStdString().c_str());
    if (f.isNull() || !f.file()) {
        qDebug() << "Impossibile aprire il file:" << absolutePath;
        return QByteArray();
    }

    TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>(f.file());
    if (!mpegFile) {
        qDebug() << "Il file non è un MPEG/MP3 valido";
        return QByteArray();
    }

    TagLib::ID3v2::Tag* tag = mpegFile->ID3v2Tag();
    if (!tag) {
        qDebug() << "Il file non contiene un tag ID3v2";
        return QByteArray();
    }

    const TagLib::ID3v2::FrameList& frames = tag->frameList("APIC");
    qDebug() << "Numero APIC:" << frames.size();

    for (const auto *frame : frames) {
        const TagLib::ID3v2::AttachedPictureFrame* picture = dynamic_cast<const TagLib::ID3v2::AttachedPictureFrame*>(frame);
        if (!picture) continue;

        qDebug() << "MIME:" << picture->mimeType().toCString();
        qDebug() << "Descrizione:" << picture->description().toCString();
        qDebug() << "Tipo:" << picture->type();

        const TagLib::ByteVector data = picture->picture();
        if (data.isEmpty()) continue;

        return QByteArray(data.data(), static_cast<qsizetype>(data.size()));

    }

    qDebug() << "Nessuna cover incorporata trovata:" << absolutePath;
    return {};

}

bool TagMapper::dtoToFile(const QString& path, const TrackDto& dto) {
    TagLib::MPEG::File file(path.toStdString().c_str());
    TagLib::ID3v2::Tag *tag = file.ID3v2Tag(true);

    // imposta i tag
    tag->setTitle(qStringToTagString(dto.title));
    tag->setArtist(qStringToTagString(dto.artistName));
    tag->setAlbum(qStringToTagString(dto.albumName));
    tag->setGenre(qStringToTagString(dto.genreName));
    tag->setTrack(dto.trackNumber);
    tag->setYear(dto.year);

    return file.save();
}

bool TagMapper::embedCover(const QString& path, const QString& coverPath) {
    TagLib::MPEG::File file(path.toStdString().c_str());
    TagLib::ID3v2::Tag* tag = file.ID3v2Tag(true);

    QFile imageFile(coverPath);
    if (!imageFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Impossibile aprire l'immagine:" << coverPath;
        return false;
    }
    QByteArray imageData = imageFile.readAll();
    imageFile.close();

    if (imageData.isEmpty()) {
        qWarning() << "Immagine vuota:" << coverPath;
        return false;
    }
    
    TagLib::ID3v2::FrameList frames = tag->frameList("APIC");
    for (auto it = frames.begin(); it != frames.end(); ++it) tag->removeFrame(*it, true);

    auto* picture = new TagLib::ID3v2::AttachedPictureFrame();
    picture->setMimeType(
        coverPath.endsWith(".jpg", Qt::CaseInsensitive) ||
        coverPath.endsWith(".jpeg", Qt::CaseInsensitive)
            ? "image/jpeg" : "image/png"
    );
    picture->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
    picture->setPicture(TagLib::ByteVector(imageData.constData(), imageData.size()));

    tag->addFrame(picture);

    return file.save();
}

bool TagMapper::cleanTags(const QString& absolutePath) {
    const QByteArray path = QFile::encodeName(absolutePath);
    TagLib::MPEG::File file(path.constData(), false);
    if (!file.isValid() || file.readOnly()) return false;
    // Rimuove ID3v1, ID3v2 e anche la copertina (APIC)
    file.strip(TagLib::MPEG::File::AllTags);
    return file.save();
}

QString TagMapper::tagStringToQString(const TagLib::String& str) {
    return QString::fromUtf8(str.to8Bit(true).c_str());
}

TagLib::String TagMapper::qStringToTagString(const QString& str) {
    QByteArray utf8 = str.toUtf8();
    return TagLib::String(utf8.constData(), TagLib::String::UTF8);
}