#include "mainwindow.h"

#include <QStatusBar>
#include <QMessageBox>

#include "storage/storagemanager.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_appController(new AppController(this))
    , m_sidebar(new NavigationSidebar(this))
    , m_itemListView(new ItemListView(this))
    , m_details(new DetailsPanel(m_appController->library(), this))
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
    splitter->addWidget(m_itemListView);
    splitter->addWidget(m_details);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    splitter->setStretchFactor(2, 2);


    setCentralWidget(splitter);
    statusBar();

    m_smartScanButton = new QPushButton("Smart Scan", this);
    m_fullScanButton = new QPushButton("Full Rescan", this);
    statusBar()->addPermanentWidget(m_smartScanButton);
    statusBar()->addPermanentWidget(m_fullScanButton);
    m_scanProgressBar = new QProgressBar(this);
    m_scanProgressBar->setRange(0, 100);
    m_scanProgressBar->setFixedWidth(150);
    m_scanProgressBar->setVisible(false); // nascosta finché non parte uno scan

    statusBar()->addPermanentWidget(m_scanProgressBar);
    statusBar()->addPermanentWidget(m_smartScanButton);
    statusBar()->addPermanentWidget(m_fullScanButton);
}

void MainWindow::setupConnections() {
    connect(m_sidebar, &NavigationSidebar::sectionSelected,
            this, &MainWindow::onSectionSelected);

    connect(m_itemListView, &ItemListView::itemSelected,
            this, &MainWindow::onItemSelected);
    connect(m_itemListView, &ItemListView::itemActivated, 
        this, &MainWindow::onItemActivated);

    connect(m_appController, &AppController::libraryUpdated,
            this, &MainWindow::onLibraryUpdated);
    connect(m_appController, &AppController::errorOccurred,
            this, &MainWindow::onErrorOccurred);
    connect(m_appController, &AppController::scanProgress,
            this, &MainWindow::onScanProgress);

    connect(m_smartScanButton, &QPushButton::clicked,
        this, &MainWindow::onSmartScanClicked);
    connect(m_fullScanButton, &QPushButton::clicked,
        this, &MainWindow::onFullScanClicked);
}

void MainWindow::onSectionSelected(NavigationSection section) {
    m_currentSection = section;
    loadCurrentSection();
}

void MainWindow::loadCurrentSection() {
    switch (m_currentSection) {
        case NavigationSection::AllTracks:
            m_itemListView->setTracks(m_appController->library()->getAllTracks());
            break;
        case NavigationSection::Albums:
            m_itemListView->setAlbums(m_appController->library()->getAllAlbums());
            break;
        case NavigationSection::Artists:
            m_itemListView->setArtists(m_appController->library()->getAllArtists());
            break;
        case NavigationSection::Genres:
            m_itemListView->setGenres(m_appController->library()->getAllGenres());
            break;
        default:
            m_itemListView->clear();
            statusBar()->showMessage("Sezione non ancora implementata", 3000);
            break;
    }

    m_details->clear();
}

void MainWindow::onItemSelected(int id) {
    switch (m_itemListView->currentViewMode()) {
        case ViewMode::Tracks: {
            auto opt = m_appController->library()->getTrackById(id);
            if (opt) m_details->showTrack(*opt); else m_details->clear();
            break;
        }
        case ViewMode::Albums: {
            auto opt = m_appController->library()->getAlbumById(id);
            if (opt) m_details->showAlbum(*opt); else m_details->clear();
            break;
        }
        case ViewMode::Artists: {
            auto opt = m_appController->library()->getArtistById(id);
            if (opt) m_details->showArtist(*opt); else m_details->clear();
            break;
        }
        case ViewMode::Genres: {
            auto opt = m_appController->library()->getGenreById(id);
            if (opt) m_details->showGenre(*opt); else m_details->clear();
            break;
        }
        default:
            m_details->clear();
    }
}

void MainWindow::onItemActivated(int id) {
    switch (m_itemListView->currentViewMode()) {
        case ViewMode::Artists: {
            // Da Artista -> Mostra gli Album di quell'artista
            QList<Album> albums = m_appController->library()->getAlbumsByArtist(id);
            m_itemListView->setAlbums(albums);
            m_details->clear();
            break;
        }
        case ViewMode::Albums: {
            // Da Album -> Mostra le Tracce di quell'album
            QList<Track> tracks = m_appController->library()->getTracksByAlbum(id);
            m_itemListView->setTracks(tracks);
            m_details->clear();
            break;
        }
        case ViewMode::Tracks: {
            // Futura apertura modale
            break;
        }
        case ViewMode::Genres: {
            break;
        }
    }
}

void MainWindow::onLibraryUpdated() {
    m_scanProgressBar->setVisible(false);
    loadCurrentSection();
    statusBar()->showMessage("Libreria aggiornata", 3000);
}

void MainWindow::onErrorOccurred(const QString& message) {
    m_scanProgressBar->setVisible(false);
    statusBar()->showMessage(message, 5000);
}

void MainWindow::onScanProgress(int percentage) {
    // statusBar()->showMessage(QString("Scansione in corso: %1%").arg(percentage));
    m_scanProgressBar->setVisible(true);
    m_scanProgressBar->setValue(percentage);
}

void MainWindow::onSmartScanClicked() {
    m_appController->requestScan(StorageManager::instance().musicPoint());
}

void MainWindow::onFullScanClicked() {
    m_appController->requestResetAndRebuildDb();
}
