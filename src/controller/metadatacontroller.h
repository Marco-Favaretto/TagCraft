#ifndef METADATACONTROLLER_H
#define METADATACONTROLLER_H

#include "dto/trackdto.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>

class MetadataController : public QObject {
    Q_OBJECT
public:
    explicit MetadataController(QObject* parent = nullptr);
    
    std::optional<TrackDto> readTags(const QString& relativePath) const;
    bool saveMetadata(const QString& relativePath, const TrackDto& newValues);
    
    QByteArray extractCover(const QString& relativePath) const;
    bool setCover(const QString& relativePath, const QString& imagePath);
    
    bool cleanTags(const QString& absolutePath);
    
signals:
    void metadataSaved(const QString& relativePath);
    void metadataSaveFailed(const QString& relativePath, const QString& reason);
    void errorOccurred(const QString& message);
};

#endif // METADATACONTROLLER_H