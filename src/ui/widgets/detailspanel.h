#ifndef DETAILSPANEL_H
#define DETAILSPANEL_H

#include <QWidget>
#include <QLabel>
#include <QFormLayout>

#include "model/track.h"

class DetailsPanel : public QWidget {
    Q_OBJECT
public:
    explicit DetailsPanel(QWidget* parent = nullptr);

public slots:
    void showTrack(const Track& track);
    void clear();

private:
    QLabel* m_titleValue;
    QLabel* m_artistValue;
    QLabel* m_albumValue;
    QLabel* m_genreValue;
    QLabel* m_yearValue;
    QLabel* m_pathValue;
};

#endif // DETAILSPANEL_H
