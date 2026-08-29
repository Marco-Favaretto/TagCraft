#ifndef NAVIGATIONSIDEBAR_H
#define NAVIGATIONSIDEBAR_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>

#include "dto/navigationsection.h"

class NavigationSidebar : public QWidget {
    Q_OBJECT
public:
    explicit NavigationSidebar(QWidget* parent = nullptr);
    
signals:
    void sectionSelected(NavigationSection section);
    
private slots:
    void onItemClicked(QListWidgetItem* item);
    
private:
    void populateSections();
    
    QListWidget* m_listWidget;
};

#endif // NAVIGATIONSIDEBAR_H