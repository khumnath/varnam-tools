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

class CustomHtmlDialog : public QDialog {
    Q_OBJECT

public:
    CustomHtmlDialog(QWidget *parent = nullptr) : QDialog(parent) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        textBrowser = new QTextBrowser(this);
        closeButton = new QPushButton("Close", this);

        closeButton->setFixedSize(80, 30);
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();
        buttonLayout->addWidget(closeButton);
        buttonLayout->addStretch();

        mainLayout->addWidget(textBrowser);
        mainLayout->addLayout(buttonLayout);

        setLayout(mainLayout);
        connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumSize(400, 300);
        setMaximumSize(800, 600);
        resize(400, 275);
    }

    void setHtmlContent(const QString &html) {
        textBrowser->setHtml(html);
    }

private:
    QTextBrowser *textBrowser;
    QPushButton *closeButton;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
    void on_indicNumberCheckBox_toggled(bool checked);
    void on_dictSuggestionsSpinBox_valueChanged(int value);
    void on_patternDictSuggestionsSpinBox_valueChanged(int value);
    void on_tokenizerSuggestionsSpinBox_valueChanged(int value);
    void on_tokenizerSuggestionsAlwaysCheckBox_toggled(bool checked);
    void vstSelectionChanged(int index);
    void populateVSTSelector();
    void printSuggestions(const std::vector<CSuggestion>& suggestions);
    void updateDebugLabel(QString index);
    void showHtmlDialog();

private:
    Ui::MainWindow *ui;
    VarnamHandler *varnamHandler;
    QFileDialog* fileDialog;
    void updateVarnamConfig();
};

#endif // MAINWINDOW_H
