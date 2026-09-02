#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QPushButton>
#include <QProgressBar>
#include <QEvent>

#include "widgets/navigationsidebar.h"
#include "widgets/itemtableview.h"
#include "widgets/detailspanel.h"
#include "controller/appcontroller.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onSectionSelected(NavigationSection section);

    // Da ItemTableView
    void onItemSelected(int id);
    void onItemActivated(int id); // doppio clic / invio -> naviga giu' un livello

    void onLibraryUpdated();
    void onErrorOccurred(const QString& message);
    void onScanProgress(int percentage);

    void onSmartScanClicked();
    void onFullScanClicked();
    void onResetDbClicked();
    
    void openFS(const QString& relativePath, bool isAlbum);
    void deleteFromFS(const QString& relativePath, bool isAlbum);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupUi();
    void setupConnections();
    void loadCurrentSection();
    void navigateUp();

    AppController* m_appController;

    NavigationSidebar* m_sidebar;
    ItemTableView* m_itemTable;
    DetailsPanel* m_details;

    QPushButton* m_smartScanButton;
    QPushButton* m_fullScanButton;
    QPushButton* m_resetAndRebuildDb;
    QProgressBar* m_scanProgressBar;

    NavigationSection m_currentSection = NavigationSection::AllTracks;

    std::optional<int> m_drilldownArtistId;
    std::optional<int> m_drilldownAlbumId;
};

#endif // MAINWINDOW_H
