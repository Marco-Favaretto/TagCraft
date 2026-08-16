#ifndef TAGMAPPER_H
#define TAGMAPPER_H

#include <QString>
#include <QByteArray>
#include "dto/trackdto.h"

class TagMapper {
public:
    TagMapper() = delete;

    static TrackDto fileToDto(const QString& absolutePath, const QString& relativePath);

    static bool dtoToFile(const QString& absolutePath, const TrackDto& dto);

    static bool embedCover(const QString& absolutePath, const QString& imagePath);

    static QByteArray extractEmbeddedCover(const QString& absolutePath);
};

#endif // TAGMAPPER_H