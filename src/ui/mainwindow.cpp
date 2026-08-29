#include "mainwindow.h"

#include <QStatusBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_appController(new AppController(this))
    , m_sidebar(new NavigationSidebar(this))
    , m_trackList(new TrackListView(this))
    , m_details(new DetailsPanel(this))
{
    setupUi();
    setupConnections();

    if (!m_appController->initialize()) {
        QMessageBox::critical(this, "Errore avvio",
            "Impossibile inizializzare il database. L'applicazione potrebbe non funzionare correttamente.");
    }

    loadCurrentSection();
}

void MainWindow::setupUi() {
    setWindowTitle("TagCraft");
    resize(1100, 700);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_sidebar);
    splitter->addWidget(m_trackList);
    splitter->addWidget(m_details);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    splitter->setStretchFactor(2, 2);

    setCentralWidget(splitter);
    statusBar();
}

void MainWindow::setupConnections() {
    connect(m_sidebar, &NavigationSidebar::sectionSelected,
            this, &MainWindow::onSectionSelected);

    connect(m_trackList, &TrackListView::trackSelected,
            this, &MainWindow::onTrackSelected);

    connect(m_appController, &AppController::libraryUpdated,
            this, &MainWindow::onLibraryUpdated);
    connect(m_appController, &AppController::errorOccurred,
            this, &MainWindow::onErrorOccurred);
    connect(m_appController, &AppController::scanProgress,
            this, &MainWindow::onScanProgress);
}

void MainWindow::onSectionSelected(NavigationSection section) {
    m_currentSection = section;
    loadCurrentSection();
}

void MainWindow::loadCurrentSection() {
    switch (m_currentSection) {
        case NavigationSection::AllTracks:
            m_trackList->setTracks(m_appController->library()->getAllTracks());
            break;
        default:
            m_trackList->clear();
            statusBar()->showMessage("Sezione non ancora implementata", 3000);
            break;
    }

    m_details->clear();
}

void MainWindow::onTrackSelected(int trackId) {
    auto trackOpt = m_appController->library()->getTrackById(trackId);
    if (trackOpt) {
        m_details->showTrack(*trackOpt);
    } else {
        m_details->clear();
    }
}

void MainWindow::onLibraryUpdated() {
    loadCurrentSection();
    statusBar()->showMessage("Libreria aggiornata", 3000);
}

void MainWindow::onErrorOccurred(const QString& message) {
    statusBar()->showMessage(message, 5000);
}

void MainWindow::onScanProgress(int percentage) {
    statusBar()->showMessage(QString("Scansione in corso: %1%").arg(percentage));
}
