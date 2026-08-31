#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QPushButton>
#include <QProgressBar>

#include "controller/appcontroller.h"
#include "widgets/navigationsidebar.h"
#include "widgets/itemlistview.h"
#include "widgets/detailspanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    // NavigationSidebar
    void onSectionSelected(NavigationSection section);

    // ItemListVIew
    void onItemSelected(int id);
    void onItemActivated(int id);

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
    ItemListView* m_itemListView;
    DetailsPanel* m_details;

    NavigationSection m_currentSection = NavigationSection::AllTracks;
    
    QPushButton* m_smartScanButton;
    QPushButton* m_fullScanButton;
    QProgressBar* m_scanProgressBar;

};

#endif // MAINWINDOW_H