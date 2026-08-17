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
    TagLib::ID3v2::Tag* tag = mpegFile->ID3v2Tag();

    if (!tag) {
        qDebug() << "Il file non contiene un tag ID3v2";
        return TrackDto();
    }

    TrackDto t;

    t.title = tag->title().toCString();
    t.artistName = tag->artist().toCString();
    t.albumName = tag->album().toCString();
    t.genreName = tag->genre().toCString();
    t.year = tag->year();
    t.trackNumber = tag->track();
    t.relativePath = relativePath;

    
    QFileInfo fileInfo(absolutePath);
    
    t.durationSeconds = mpegFile->audioProperties() ? mpegFile->audioProperties()->lengthInSeconds() : 0;
    t.fileSize = fileInfo.size();
    t.lastModified = fileInfo.lastModified().toMSecsSinceEpoch();
    
    qDebug() << t.toString();

    return t;
}

bool TagMapper::dtoToFile(const QString& absolutePath, const TrackDto& dto) {}

bool TagMapper::embedCover(const QString& absolutePath, const QString& imagePath) {}

QByteArray TagMapper::extractEmbeddedCover(const QString& absolutePath) {
    TagLib::FileRef f(absolutePath.toStdString().c_str());
    
    if (f.isNull() || !f.file()) {
        qDebug() << "Impossibile aprire il file:" << absolutePath;
        return QByteArray();
    }

    TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>(f.file());
    TagLib::ID3v2::Tag* tag = mpegFile->ID3v2Tag();
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