#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>

#include "controller/appcontroller.h"
#include "widgets/navigationsidebar.h"
#include "widgets/tracklistview.h"
#include "widgets/detailspanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    // NavigationSidebar
    void onSectionSelected(NavigationSection section);

    // TrackListView
    void onTrackSelected(int trackId);

    // AppController
    void onLibraryUpdated();
    void onErrorOccurred(const QString& message);
    void onScanProgress(int percentage);

private:
    void setupUi();
    void setupConnections();
    void loadCurrentSection();

    AppController* m_appController;

    NavigationSidebar* m_sidebar;
    TrackListView* m_trackList;
    DetailsPanel* m_details;

    NavigationSection m_currentSection = NavigationSection::AllTracks;
};

#endif // MAINWINDOW_H