#ifndef EDITMETADATADIALOG_H
#define EDITMETADATADIALOG_H

#include <QDialog>
#include <QHash>
#include <QVariant>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QSpinBox>
#include <QPushButton>

#include "controller/metadatacontroller.h"
#include "ui/editmodels/abstracteditmodel.h"

// Dialog generico di editing metadati: costruisce il form a partire da un
// AbstractEditModel (fields())
class EditMetadataDialog : public QDialog {
    Q_OBJECT
public:

    explicit EditMetadataDialog(AbstractEditModel* model,
                                 MetadataController* metadata,
                                 QWidget* parent = nullptr);

    QHash<QString, QVariant> changedValues() const;
    QString stagedArtworkPath() const;
    bool artworkRemoved() const;
    bool cleanTags() const;

private slots:
    void onAnyFieldChanged();
    void onSaveClicked();
    void onChangeArtworkClicked();
    void onRemoveArtworkClicked();
    void onCleanTagsClicked();

private:
    void setupUi();
    void buildForm();
    void showArtwork();
    bool hasChanges() const;
    QHash<QString, QVariant> collectChangedValues() const;
    QHash<QString, QVariant> collectCleanTags() const;

    AbstractEditModel* m_model;
    MetadataController* m_metadata;

    QLabel* m_artworkLabel;
    QFormLayout* m_formLayout;
    QDialogButtonBox* m_buttonBox;

    // Per ogni field editabile: il widget di input associato.
    QHash<QString, QLineEdit*> m_textEditors;
    QHash<QString, QSpinBox*> m_intEditors;

    QHash<QString, QVariant> m_changedValues;

    QPushButton* m_changeArtworkButton;
    QPushButton* m_removeArtworkButton;
    QPushButton* m_cleanTagsButton;

    QString m_stagedArtworkPath;
    bool m_artworkRemoved = false;
    bool m_cleanTags = false;
};

#endif // EDITMETADATADIALOG_H
