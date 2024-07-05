#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialog>
#include "varnamcpp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Custom HTML Dialog class definition
class CustomHtmlDialog : public QDialog {
    Q_OBJECT

public:
    CustomHtmlDialog(QWidget *parent = nullptr) : QDialog(parent) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        textBrowser = new QTextBrowser(this);
        closeButton = new QPushButton("Close", this);

        // Set the size of the close button to be smaller
        closeButton->setFixedSize(80, 30);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();
        buttonLayout->addWidget(closeButton);
        buttonLayout->addStretch();

        mainLayout->addWidget(textBrowser);
        mainLayout->addLayout(buttonLayout);

        setLayout(mainLayout);

        // Connect the close button to the dialog's accept slot
        connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // Set minimum and maximum sizes as needed
        setMinimumSize(400, 300); // Minimum size for the dialog
        setMaximumSize(800, 600); // Maximum size for the dialog

        // Set an initial size for the dialog
        resize(400, 275);
    }

    void setHtmlContent(const QString &html) {
        textBrowser->setHtml(html);
    }

private:
    QTextBrowser *textBrowser;
    QPushButton *closeButton;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int varnam_learn_from_file(VarnamHandler* v, char* filePath, LearnStatus** status);
    int varnam_train_from_file(VarnamHandler* v, char* filePath, LearnStatus** status);
    int varnam_export(VarnamHandler* v, char* filePath, int exportWordsPerFile);
    int varnam_import(VarnamHandler* v, char* filePath);
    char* varnam_get_last_error(VarnamHandler* v);
    void resetAppState();
    void showHtmlDialog(); // Declaration of the new method

private slots:
    void on_transliterateButton_clicked();
    void on_reverseTransliterateButton_clicked();
    void on_trainButton_clicked();
    void on_learnButton_clicked();
    void on_unlearnButton_clicked();
    void on_learnFromFileButton_clicked();
    void on_trainFromFileButton_clicked();
    void on_exportButton_clicked();
    void on_importButton_clicked();
    void vstSelectionChanged(int index);
    void populateVSTSelector();
    void printSuggestions(const std::vector<CSuggestion>& suggestions);
    void updateDebugLabel(QString index);
    void on_indicNumberCheckBox_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    VarnamHandler *varnamHandler; // Replace with your actual VarnamHandler class
    QFileDialog* fileDialog;
};

#endif // MAINWINDOW_H
