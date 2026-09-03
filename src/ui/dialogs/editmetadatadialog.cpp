#include "ui/dialogs/editmetadatadialog.h"

#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "dto/constants.h"

EditMetadataDialog::EditMetadataDialog(AbstractEditModel* model,
                                       MetadataController* metadata,
                                       QWidget* parent)
    : QDialog(parent)
    , m_model(model)
    , m_metadata(metadata)
    , m_artworkLabel(new QLabel(this))
    , m_changeArtworkButton(new QPushButton(tr("Cambia artwork"), this))
    , m_removeArtworkButton(new QPushButton(tr("Elimina artwork"), this))
    , m_cleanTagsButton(new QPushButton(tr("Elimina tutti i metadati"), this))
    , m_formLayout(new QFormLayout())
    , m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this))
{
    setupUi();
    showArtwork();
    buildForm();

    setWindowTitle(m_model->windowTitle());
}

void EditMetadataDialog::setupUi() {
    switch(m_model->viewMode()) {
        case ViewMode::Tracks:
            setMinimumSize(QSize(500, 700));
            break;
        case ViewMode::Albums:
            setMinimumSize(QSize(500, 500));
            break;
        case ViewMode::Artists:
            setMinimumSize(QSize(500, 500));
            break;
        case ViewMode::Genres:
            setMinimumSize(QSize(500, 500));
            break;
    }

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    m_artworkLabel->setAlignment(Qt::AlignCenter);
    m_artworkLabel->setFixedHeight(160);
    mainLayout->addWidget(m_artworkLabel, 0, Qt::AlignCenter);
 
    auto* artworkButtonsLayout = new QHBoxLayout();
    artworkButtonsLayout->setSpacing(8);
    artworkButtonsLayout->addWidget(m_changeArtworkButton);
    artworkButtonsLayout->addWidget(m_removeArtworkButton);
    mainLayout->addLayout(artworkButtonsLayout);
 
    connect(m_changeArtworkButton, &QPushButton::clicked, this, &EditMetadataDialog::onChangeArtworkClicked);
    connect(m_removeArtworkButton, &QPushButton::clicked, this, &EditMetadataDialog::onRemoveArtworkClicked);

    m_formLayout->setSpacing(8);
    m_formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    mainLayout->addLayout(m_formLayout);

    auto* bottomButtonsLayout = new QHBoxLayout();
    bottomButtonsLayout->addWidget(m_cleanTagsButton);
    bottomButtonsLayout->addWidget(m_buttonBox);
    m_buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);

    mainLayout->addLayout(bottomButtonsLayout);

    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &EditMetadataDialog::onSaveClicked);
    connect(m_cleanTagsButton, &QPushButton::clicked, this, &EditMetadataDialog::onCleanTagsClicked);
}

void EditMetadataDialog::showArtwork() {
    const QString hash = m_model->artworkHash();

    std::optional<QImage> imagePtr;
    if (!hash.isEmpty() && m_metadata) {
        imagePtr = m_metadata->loadFromCache(hash);
    }

    QImage image;
    switch(m_model->viewMode()) {
        case ViewMode::Albums:
            if(!imagePtr) image.load(Constants::Artwork::Album);
            else image = *imagePtr;
            break;
        case ViewMode::Tracks:
            if(!imagePtr) image.load(Constants::Artwork::Track);
            else image = *imagePtr;
            break;
        case ViewMode::Artists:
            image.load(Constants::Artwork::Artist);
            break;
        case ViewMode::Genres:
            image.load(Constants::Artwork::Genre);
    }

    QPixmap pixmap = QPixmap::fromImage(image).scaled(
        QSize(160, 160),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );
    m_artworkLabel->setPixmap(pixmap);
    m_artworkLabel->setVisible(true);
}

void EditMetadataDialog::buildForm() {
    const QList<EditField> fields = m_model->fields();

    for (const EditField& field : fields) {
        if (!field.editable) {
            auto* valueLabel = new QLabel(field.value.toString(), this);
            valueLabel->setWordWrap(true);
            m_formLayout->addRow(field.label, valueLabel);
            continue;
        }

        switch (field.type) {
            case EditField::Type::Text: {
                auto* editor = new QLineEdit(this);
                editor->setPlaceholderText(field.value.toString());
                m_textEditors.insert(field.key, editor);
                m_formLayout->addRow(field.label, editor);
                connect(editor, &QLineEdit::textChanged, this, &EditMetadataDialog::onAnyFieldChanged);
                break;
            }
            case EditField::Type::Int: {
                auto* editor = new QSpinBox(this);
                editor->setRange(0, 999999);
                editor->setSpecialValueText("-");
                editor->setValue(field.value.toInt());
                editor->setProperty("originalValue", field.value.toInt());
                m_intEditors.insert(field.key, editor);
                m_formLayout->addRow(field.label, editor);
                connect(editor, QOverload<int>::of(&QSpinBox::valueChanged),
                        this, &EditMetadataDialog::onAnyFieldChanged);
                break;
            }
        }
    }
}

bool EditMetadataDialog::hasChanges() const {
    if(cleanTags()) return true;

    if (!m_stagedArtworkPath.isEmpty() || m_artworkRemoved) return true;

    for (auto it = m_textEditors.constBegin(); it != m_textEditors.constEnd(); ++it) {
        if (!it.value()->text().isEmpty()) return true;
    }
    for (auto it = m_intEditors.constBegin(); it != m_intEditors.constEnd(); ++it) {
        const int original = it.value()->property("originalValue").toInt();
        if (it.value()->value() != original) return true;
    }
    return false;
}

QHash<QString, QVariant> EditMetadataDialog::collectChangedValues() const {
    QHash<QString, QVariant> changed;

    for (auto it = m_textEditors.constBegin(); it != m_textEditors.constEnd(); ++it) {
        if (!it.value()->text().isEmpty()) {
            changed.insert(it.key(), it.value()->text());
        }
    }

    for (auto it = m_intEditors.constBegin(); it != m_intEditors.constEnd(); ++it) {
        const int original = it.value()->property("originalValue").toInt();
        if (it.value()->value() != original) {
            changed.insert(it.key(), it.value()->value());
        }
    }

    return changed;
}

void EditMetadataDialog::onAnyFieldChanged() {
    m_buttonBox->button(QDialogButtonBox::Save)->setEnabled(hasChanges());
}

void EditMetadataDialog::onSaveClicked() {
    QHash<QString, QVariant> changed;
    if (cleanTags()) changed = collectCleanTags();
    else changed = collectChangedValues();
    const bool artworkChanged = !m_stagedArtworkPath.isEmpty() || m_artworkRemoved;
 
    if (changed.isEmpty() && !artworkChanged) return; // Non dovrebbe succedere (Save e' disabilitato), ma per sicurezza
 
    // Riepilogo "vecchio -> nuovo"
    QStringList lines;
    const QList<EditField> fields = m_model->fields();
    for (const EditField& field : fields) {
        if (!changed.contains(field.key)) continue;
 
        QString oldDisplay = field.value.toString();
        QString newDisplay = changed.value(field.key).toString();
 
        if (field.type == EditField::Type::Int) {
            const int oldValue = field.value.toInt();
            const int newValue = changed.value(field.key).toInt();
            oldDisplay = oldValue == field.emptyIntValue ? "-" : QString::number(oldValue);
            newDisplay = newValue == field.emptyIntValue ? "-" : QString::number(newValue);
        } else if (oldDisplay.isEmpty()) {
            oldDisplay = "-";
        }
 
        lines << QString("%1 %2 -> %3").arg(field.label, oldDisplay, newDisplay);
    }
 
    if (!m_stagedArtworkPath.isEmpty()) {
        lines << tr("Artwork: modificata");
    } else if (m_artworkRemoved) {
        lines << tr("Artwork: rimossa");
    }
 
    const QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Conferma modifiche"),
        tr("Stai per salvare le seguenti modifiche:\n\n%1").arg(lines.join("\n")),
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
    );
 
    if (reply != QMessageBox::Ok) return;
 
    m_changedValues = changed;
    accept();
}

QHash<QString, QVariant> EditMetadataDialog::changedValues() const {
    return m_changedValues;
}

void EditMetadataDialog::onChangeArtworkClicked() {
    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Seleziona artwork"),
        QString(),
        tr("Immagini (*.jpg *.jpeg *.png)")
    );
    if (path.isEmpty()) return;
 
    QImage test;
    if (!test.load(path)) {
        QMessageBox::warning(this, tr("Errore"), tr("Impossibile caricare l'immagine selezionata."));
        return;
    }
 
    m_stagedArtworkPath = path;
    m_artworkRemoved = false;
    showArtwork();
    onAnyFieldChanged();
}
 
void EditMetadataDialog::onRemoveArtworkClicked() {
    m_stagedArtworkPath.clear();
    m_artworkRemoved = true;
    showArtwork();
    onAnyFieldChanged();
}

QString EditMetadataDialog::stagedArtworkPath() const {
    return m_stagedArtworkPath;
}
 
bool EditMetadataDialog::artworkRemoved() const {
    return m_artworkRemoved;
}

void EditMetadataDialog::onCleanTagsClicked() {
    m_cleanTags = true;
    onAnyFieldChanged();
}

bool EditMetadataDialog::cleanTags() const {
    return m_cleanTags;
}

QHash<QString, QVariant> EditMetadataDialog::collectCleanTags() const {
    QHash<QString, QVariant> cleaned;

    for (auto it = m_textEditors.constBegin(); it != m_textEditors.constEnd(); ++it) {
        cleaned.insert(it.key(), QString());
    }

    for (auto it = m_intEditors.constBegin(); it != m_intEditors.constEnd(); ++it) {
        cleaned.insert(it.key(), 0);
    }

    return cleaned;
}