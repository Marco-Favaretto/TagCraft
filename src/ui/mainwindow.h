#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QPushButton>
#include <QProgressBar>

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
    void onItemSelected(int id);

    // AppController
    void onLibraryUpdated();
    void onErrorOccurred(const QString& message);
    void onScanProgress(int percentage);
    void onSmartScanClicked();
    void onFullScanClicked();

private:
    void setupUi();
    void setupConnections();
    void loadCurrentSection();

    AppController* m_appController;

    NavigationSidebar* m_sidebar;
    TrackListView* m_trackList;
    DetailsPanel* m_details;

    NavigationSection m_currentSection = NavigationSection::AllTracks;
    
    QPushButton* m_smartScanButton;
    QPushButton* m_fullScanButton;
    QProgressBar* m_scanProgressBar;

};

#endif // MAINWINDOW_H