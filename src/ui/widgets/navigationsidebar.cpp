#include "navigationsidebar.h"

NavigationSidebar::NavigationSidebar(QWidget* parent)
    : QWidget(parent)
    , m_listWidget(new QListWidget(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_listWidget);

    populateSections();

    connect(m_listWidget, &QListWidget::itemClicked,
            this, &NavigationSidebar::onItemClicked);
}

void NavigationSidebar::populateSections() {
    struct Entry { QString label; NavigationSection section; };
    const QList<Entry> entries = {
        {"All Tracks", NavigationSection::AllTracks},
        {"Artists",    NavigationSection::Artists},
        {"Albums",     NavigationSection::Albums},
        {"Genres",     NavigationSection::Genres},
        {"Downloads",  NavigationSection::Downloads},
        {"Settings",   NavigationSection::Settings},
    };

    for (const auto& entry : entries) {
        auto* item = new QListWidgetItem(entry.label, m_listWidget);
        item->setData(Qt::UserRole, static_cast<int>(entry.section));
    }
}

void NavigationSidebar::onItemClicked(QListWidgetItem* item) {
    const auto section = static_cast<NavigationSection>(item->data(Qt::UserRole).toInt());
    emit sectionSelected(section);
}
