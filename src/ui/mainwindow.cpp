#include "mainwindow.h"

#include <QStatusBar>
#include <QMessageBox>
#include <QKeyEvent>

#include "storage/storagemanager.h"
#include "dto/constants.h"
#include "ui/dialogs/editmetadatadialog.h"
#include "ui/editmodels/trackeditmodel.h"
#include "ui/editmodels/trackeditmodel.h"
#include "ui/editmodels/albumeditmodel.h"
#include "ui/editmodels/artisteditmodel.h"
#include "ui/editmodels/genreeditmodel.h"
#include "ui/dialogs/editmetadatadialog.h"
#include "ui/editmodels/batchtrackeditmodel.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_appController(new AppController(this))
{
    if (!m_appController->initialize()) {
        QMessageBox::critical(this, "Errore avvio",
            "Impossibile inizializzare il database. L'applicazione potrebbe non funzionare correttamente.");
    }

    m_sidebar = new NavigationSidebar(this);
    m_itemTable = new ItemTableView(m_appController->library(), this);
    m_details = new DetailsPanel(m_appController->library(), m_appController->metadata(), this);

    setupUi();
    setupConnections();
    m_itemTable->installEventFilter(this);

    loadCurrentSection();
}

void MainWindow::setupUi() {
    setWindowTitle("TagCraft");
    resize(1200, 800);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_sidebar);
    splitter->addWidget(m_itemTable);
    splitter->addWidget(m_details);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    splitter->setStretchFactor(2, 2);

    setCentralWidget(splitter);

    m_smartScanButton = new QPushButton("Smart Scan", this);
    m_fullScanButton = new QPushButton("Full Rescan", this);
    m_resetAndRebuildDb = new QPushButton("Rebuild DB", this);
    m_scanForDevices = new QPushButton("Scan For Devices", this);

    m_selectModeButton = new QPushButton("Seleziona", this);
    m_selectModeButton->setCheckable(true);
    m_selectModeButton->setVisible(false); // visibile solo in ViewMode::Tracks

    m_selectAllButton = new QPushButton("Seleziona tutto", this);
    m_deselectAllButton = new QPushButton("Deseleziona tutto", this);
    m_batchEditButton = new QPushButton("Modifica selezione", this);
    m_selectAllButton->setVisible(false);
    m_deselectAllButton->setVisible(false);
    m_batchEditButton->setVisible(false);
    m_batchEditButton->setEnabled(false);

    statusBar()->addPermanentWidget(m_selectModeButton);
    statusBar()->addPermanentWidget(m_selectAllButton);
    statusBar()->addPermanentWidget(m_deselectAllButton);
    statusBar()->addPermanentWidget(m_batchEditButton);

    m_scanProgressBar = new QProgressBar(this);
    m_scanProgressBar->setRange(0, 100);
    m_scanProgressBar->setFixedWidth(150);
    m_scanProgressBar->setVisible(false);

    statusBar()->addPermanentWidget(m_scanProgressBar);
    statusBar()->addPermanentWidget(m_smartScanButton);
    statusBar()->addPermanentWidget(m_fullScanButton);
    statusBar()->addPermanentWidget(m_resetAndRebuildDb);
    statusBar()->addPermanentWidget(m_scanForDevices);
}

void MainWindow::setupConnections() {
    connect(m_sidebar, &NavigationSidebar::sectionSelected, this, &MainWindow::onSectionSelected);

    connect(m_itemTable, &ItemTableView::itemSelected, this, &MainWindow::onItemSelected);
    connect(m_itemTable, &ItemTableView::itemActivated, this, &MainWindow::onItemActivated);

    connect(m_appController, &AppController::libraryUpdated, this, &MainWindow::onLibraryUpdated);
    connect(m_appController, &AppController::errorOccurred, this, &MainWindow::onErrorOccurred);
    connect(m_appController, &AppController::scanProgress, this, &MainWindow::onScanProgress);
    connect(m_appController, &AppController::storageMounted, this, &MainWindow::onStorageMounted);
    connect(m_appController, &AppController::batchOperationFinished, this, &MainWindow::onBatchOperationFinished);

    connect(m_smartScanButton, &QPushButton::clicked, this, &MainWindow::onSmartScanClicked);
    connect(m_fullScanButton, &QPushButton::clicked,this, &MainWindow::onFullScanClicked);
    connect(m_resetAndRebuildDb, &QPushButton::clicked, this, &MainWindow::onResetDbClicked);
    connect(m_scanForDevices, &QPushButton::clicked, this, &MainWindow::scanDevices);

    connect(m_details, &DetailsPanel::openFS, this, &MainWindow::openFS);
    connect(m_details, &DetailsPanel::deleteFromFS, this, &MainWindow::deleteFromFS);
    connect(m_details, &DetailsPanel::editRequested, this, &MainWindow::onEditRequested);

    connect(m_selectModeButton, &QPushButton::toggled, this, &MainWindow::onSelectModeToggled);
    connect(m_selectAllButton, &QPushButton::clicked, this, &MainWindow::onSelectAllClicked);
    connect(m_deselectAllButton, &QPushButton::clicked, this, &MainWindow::onDeselectAllClicked);
    connect(m_batchEditButton, &QPushButton::clicked, this, &MainWindow::onBatchEditClicked);
    connect(m_itemTable, &ItemTableView::selectionChanged, this, &MainWindow::onSelectionChanged);
    connect(m_itemTable, &ItemTableView::viewModeChanged, this, &MainWindow::onViewModeChanged);
}

void MainWindow::onSectionSelected(NavigationSection section) {
    m_currentSection = section;
    m_drilldownArtistId.reset();
    m_drilldownAlbumId.reset();
    loadCurrentSection();
}

void MainWindow::loadCurrentSection() {
    switch (m_currentSection) {
        case NavigationSection::AllTracks:
            m_itemTable->setTracks(m_appController->library()->getAllTracksResolved());
            break;
        case NavigationSection::Albums:
            m_itemTable->setAlbums(m_appController->library()->getAllAlbumsResolved());
            break;
        case NavigationSection::Artists:
            m_itemTable->setArtists(m_appController->library()->getAllArtists());
            break;
        case NavigationSection::Genres:
            m_itemTable->setGenres(m_appController->library()->getAllGenres());
            break;
        default:
            m_itemTable->clear();
            statusBar()->showMessage("Sezione non ancora implementata", 3000);
            break;
    }

    m_details->clear();
}

void MainWindow::onItemSelected(int id) {
    switch (m_itemTable->currentViewMode()) {
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
    }
}

void MainWindow::onItemActivated(int id) {
    // Navigazione gerarchica: Artists -> album di quell'artista;
    // Albums -> tracce di quell'album. Genres/Tracks non hanno un livello
    switch (m_itemTable->currentViewMode()) {
        case ViewMode::Artists:
            m_drilldownArtistId = id;
            m_itemTable->setAlbums(m_appController->library()->getAlbumsForArtist(id));
            m_details->clear();
            break;
        case ViewMode::Albums:
            m_drilldownAlbumId = id;
            if (id == Constants::DefaultValues::AlbumId && m_drilldownArtistId) {
                // Album fittizio (tracce sparse di questo artista senza album) - non e' il vero Unknown Album globale, va risolto per artista.
                m_itemTable->setTracks(m_appController->library()->getUnknownAlbumOfArtist(*m_drilldownArtistId));
            } else {
                m_itemTable->setTracks(m_appController->library()->getTracksByAlbum(id));
            }
            m_itemTable->sortByColumn(3, Qt::AscendingOrder);
            m_details->clear();
            break;
        case ViewMode::Genres:
            m_itemTable->setTracks(m_appController->library()->getTracksByGenre(id));
            m_details->clear();
            break;
        case ViewMode::Tracks:
            break; // nessun livello successivo, magari apertura dialog modifica
    }
}

void MainWindow::onBatchOperationFinished(const QString& operationName, int succeeded, int failed) {
    m_suppressNextLibraryUpdatedMessage = true;

    if (failed == 0) {
        statusBar()->showMessage(QString("%1: %2 completate").arg(operationName).arg(succeeded), 4000);
    } else {
        statusBar()->showMessage(QString("%1: %2 completate, %3 fallite").arg(operationName).arg(succeeded).arg(failed), 6000);
    }
}

void MainWindow::onLibraryUpdated() {
    m_scanProgressBar->setVisible(false);
    loadCurrentSection();

    if (m_suppressNextLibraryUpdatedMessage) {
        m_suppressNextLibraryUpdatedMessage = false;
    } else {
        statusBar()->showMessage("Libreria aggiornata", 3000);
    }
}

void MainWindow::onErrorOccurred(const QString& message) {
    m_scanProgressBar->setVisible(false);
    statusBar()->showMessage(message, 5000);
}

void MainWindow::onScanProgress(int percentage) {
    m_scanProgressBar->setVisible(true);
    m_scanProgressBar->setValue(percentage);
}

void MainWindow::onStorageMounted() {
    m_fullScanButton->setEnabled(true);
    m_smartScanButton->setEnabled(true);
    m_resetAndRebuildDb->setEnabled(true);
    loadCurrentSection();
    statusBar()->showMessage("Storage collegato, libreria caricata", 3000);
}

void MainWindow::onSmartScanClicked() {
    m_scanForDevices->setEnabled(false);
    m_appController->requestScan(StorageManager::instance().musicPoint());
}

void MainWindow::onFullScanClicked() {
    m_scanForDevices->setEnabled(false);
    m_appController->requestResetAndRebuildDb();
}

void MainWindow::onResetDbClicked() {
    m_scanForDevices->setEnabled(false);
    m_appController->requestResetAndRebuildDb();
}

void MainWindow::scanDevices() {
    m_fullScanButton->setEnabled(false);
    m_smartScanButton->setEnabled(false);
    m_resetAndRebuildDb->setEnabled(false);
    m_appController->requestScanForDevices();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Backspace) {
            navigateUp();
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::navigateUp() {
    switch (m_itemTable->currentViewMode()) {
        case ViewMode::Tracks:
            if (m_drilldownAlbumId) {
                m_drilldownAlbumId.reset();
                if (m_drilldownArtistId) {
                    m_itemTable->setAlbums(m_appController->library()->getAlbumsByArtist(*m_drilldownArtistId));
                } else {
                    m_itemTable->setAlbums(m_appController->library()->getAllAlbums());
                }
                m_details->clear();
            }
            break;
        case ViewMode::Albums:
            if (m_drilldownArtistId) {
                m_drilldownArtistId.reset();
                m_itemTable->setArtists(m_appController->library()->getAllArtists());
                m_details->clear();
            }
            break;
        default:
            break;
    }
}

void MainWindow::openFS(const QString& relativePath, bool isAlbum) {
    m_appController->openFS(relativePath, isAlbum);
}

void MainWindow::deleteFromFS(const QString& relativePath, bool isAlbum) {
if (relativePath.isEmpty()) {
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        tr("Conferma eliminazione"),
        tr("Stai per cancellare %1. Continuare?").arg(relativePath),
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel // Pulsante predefinito
    );

    if (reply == QMessageBox::Ok) {
        m_appController->deleteFromFS(relativePath, isAlbum);
    }
}

void MainWindow::onEditRequested(ViewMode mode, int id) {
    switch (mode) {
        case ViewMode::Tracks:{
            auto opt = m_appController->library()->getTrackById(id);
            if (!opt) return;
            TrackEditModel model(*opt, m_appController->library());
            EditMetadataDialog dialog(&model, m_appController->metadata(), this);
            if (dialog.exec() == QDialog::Accepted) {
                const TrackDto dto = model.buildDto(dialog.changedValues());
                if(dialog.cleanTags()) m_appController->requestCleanTags(dto.relativePath);
                else {
                    m_appController->requestSaveMetadata(dto.relativePath, dto);
                    if (!dialog.stagedArtworkPath().isEmpty()) {
                        m_appController->requestSetCover(dto.relativePath, dialog.stagedArtworkPath());
                    } else if (dialog.artworkRemoved()) {
                        m_appController->requestRemoveCover(dto.relativePath);
                    }
                }
            }
            break;
        }
        case ViewMode::Albums: {
            auto opt = m_appController->library()->getAlbumById(id);
            if (!opt) return;

            AlbumEditModel model(*opt, m_appController->library());
            EditMetadataDialog dialog(&model, m_appController->metadata(), this);

            if (dialog.exec() == QDialog::Accepted) {
                const QHash<QString, QVariant> result = model.buildResult(dialog.changedValues());
                // result["id"], result["title"], result["artistName"], result["genreName"]
                m_appController->requestSaveAlbumMetadata(result);

                if (!dialog.stagedArtworkPath().isEmpty()) {
                    m_appController->requestSetAlbumCover(opt->id(), dialog.stagedArtworkPath());
                } else if (dialog.artworkRemoved()) {
                    m_appController->requestRemoveAlbumCover(opt->id());
                }
            }
            break;
        }
        case ViewMode::Artists: {
            auto opt = m_appController->library()->getArtistById(id);
            if (!opt) return;

            ArtistEditModel model(*opt);
            EditMetadataDialog dialog(&model, m_appController->metadata(), this);

            if (dialog.exec() == QDialog::Accepted) {
                const QString newName = model.buildName(dialog.changedValues());
                m_appController->requestRenameArtist(model.entityId(), newName);
            }
            break;
        }
        case ViewMode::Genres: {
            auto opt = m_appController->library()->getGenreById(id);
            if (!opt) return;

            GenreEditModel model(*opt);
            EditMetadataDialog dialog(&model, m_appController->metadata(), this);

            if (dialog.exec() == QDialog::Accepted) {
                const QString newName = model.buildName(dialog.changedValues());
                m_appController->requestRenameGenre(model.entityId(), newName);
            }
            break;
        }
    }
}

void MainWindow::onViewModeChanged(ViewMode mode) {
    const bool isTracks = (mode == ViewMode::Tracks);
    m_selectModeButton->setVisible(isTracks);
    if (!isTracks) {
        m_selectModeButton->setChecked(false); // forza anche onSelectModeToggled(false)
    }
}

void MainWindow::onSelectModeToggled() {
    const bool enabled = m_selectModeButton->isChecked();
    m_itemTable->setSelectionModeEnabled(enabled);
    m_selectModeButton->setText(enabled ? "Termina selezione" : "Seleziona");
    m_selectAllButton->setVisible(enabled);
    m_deselectAllButton->setVisible(enabled);
    m_batchEditButton->setVisible(enabled);
    if (!enabled) m_batchEditButton->setEnabled(false);
}

void MainWindow::onSelectAllClicked() {
    m_itemTable->selectAllVisible();
}

void MainWindow::onDeselectAllClicked() {
    m_itemTable->deselectAllVisible();
}

void MainWindow::onSelectionChanged(int count) {
    m_batchEditButton->setText(QString("Modifica selezione (%1)").arg(count));
    m_batchEditButton->setEnabled(count > 0);
}

void MainWindow::onBatchEditClicked() {
    const QList<int> ids = m_itemTable->selectedTrackIds();
    if (ids.isEmpty()) return;

    QList<Track> tracks;
    QList<QString> relativePaths;
    for (int id : ids) {
        auto opt = m_appController->library()->getTrackById(id);
        if (!opt) continue;
        tracks.append(*opt);
        relativePaths.append(opt->relativePath());
    }
    if (tracks.isEmpty()) return;

    BatchTrackEditModel model(tracks, m_appController->library());
    EditMetadataDialog dialog(&model, m_appController->metadata(), this);

    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.cleanTags()) {
            m_appController->requestCleanTagsBatch(relativePaths);
        } else {
            m_appController->requestSaveMetadataBatch(tracks, dialog.changedValues());

            if (!dialog.stagedArtworkPath().isEmpty()) {
                m_appController->requestSetCoverBatch(relativePaths, dialog.stagedArtworkPath());
            } else if (dialog.artworkRemoved()) {
                m_appController->requestRemoveCoverBatch(relativePaths);
            }
        }
    }

    // Uscita automatica dalla modalita' selezione dopo l'edit
    m_selectModeButton->setChecked(false);
}