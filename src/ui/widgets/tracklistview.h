#ifndef TRACKLISTVIEW_H
#define TRACKLISTVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QList>

#include "model/track.h"

class TrackListView : public QWidget {
    Q_OBJECT
public:
    explicit TrackListView(QWidget* parent = nullptr);

    void setTracks(const QList<Track>& tracks);
    void clear();

signals:
    void trackSelected(int trackId);

private slots:
    void onItemClicked(QListWidgetItem* item);

private:
    QListWidget* m_listWidget;
};

#endif // TRACKLISTVIEW_H