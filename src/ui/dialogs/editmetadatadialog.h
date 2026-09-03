#ifndef EDITMETADATADIALOG_H
#define EDITMETADATADIALOG_H

#include <QDialog>
#include <QHash>
#include <QVariant>

class QFormLayout;
class QLabel;
class QDialogButtonBox;
class QLineEdit;
class QSpinBox;

class AbstractEditModel;
class MetadataController;

// Dialog generico di editing metadati: costruisce il form a partire da un
// AbstractEditModel (fields())
class EditMetadataDialog : public QDialog {
    Q_OBJECT
public:

    explicit EditMetadataDialog(AbstractEditModel* model,
                                 MetadataController* metadata,
                                 QWidget* parent = nullptr);

    QHash<QString, QVariant> changedValues() const;

private slots:
    void onAnyFieldChanged();
    void onSaveClicked();

private:
    void setupUi();
    void buildForm();
    void showArtwork();
    bool hasChanges() const;
    QHash<QString, QVariant> collectChangedValues() const;

    AbstractEditModel* m_model;
    MetadataController* m_metadata;

    QLabel* m_artworkLabel;
    QFormLayout* m_formLayout;
    QDialogButtonBox* m_buttonBox;

    // Per ogni field editabile: il widget di input associato.
    QHash<QString, QLineEdit*> m_textEditors;
    QHash<QString, QSpinBox*> m_intEditors;

    QHash<QString, QVariant> m_changedValues;
};

#endif // EDITMETADATADIALOG_H
