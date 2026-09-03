#ifndef TAGMAPPER_H
#define TAGMAPPER_H

#include <QString>
#include <QByteArray>

#include <taglib/tstring.h>

#include "dto/trackdto.h"

class TagMapper {
public:
    TagMapper() = delete;

    static TrackDto fileToDto(const QString& absolutePath, const QString& relativePath);

    static QByteArray extractEmbeddedCover(const QString& absolutePath);
    
    static bool dtoToFile(const QString& absolutePath, const TrackDto& dto);
    
    static bool embedCover(const QString& absolutePath, const QString& imagePath);

    static bool removeCover(const QString& path);

    static bool cleanTags(const QString& absolutePath);
private:
    static TagLib::String qStringToTagString(const QString& str);
    static QString tagStringToQString(const TagLib::String& str);
};

#endif // TAGMAPPER_H