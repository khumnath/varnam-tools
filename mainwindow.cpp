#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString version = varnam_get_version();
    ui->version->setText("Varnam Library Version: " + version + " ");

    setWindowFlags(Qt::Window);

    // Connect buttons to slots
    connect(ui->transliterateButton, &QPushButton::clicked, this, &MainWindow::on_transliterateButton_clicked);
    connect(ui->reverseTransliterateButton, &QPushButton::clicked, this, &MainWindow::on_reverseTransliterateButton_clicked);
    connect(ui->trainButton, &QPushButton::clicked, this, &MainWindow::on_trainButton_clicked);
    connect(ui->learnButton, &QPushButton::clicked, this, &MainWindow::on_learnButton_clicked);
    connect(ui->unlearnButton, &QPushButton::clicked, this, &MainWindow::on_unlearnButton_clicked);
    connect(ui->learnFromFileButton, &QPushButton::clicked, this, &MainWindow::on_learnFromFileButton_clicked);
    connect(ui->trainFromFileButton, &QPushButton::clicked, this, &MainWindow::on_trainFromFileButton_clicked);
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::on_exportButton_clicked);
    connect(ui->importButton, &QPushButton::clicked, this, &MainWindow::on_importButton_clicked);
    connect(ui->indicNumberCheckBox, &QCheckBox::toggled, this, &MainWindow::on_indicNumberCheckBox_toggled);
    connect(ui->info, &QPushButton::clicked, this, &MainWindow::showHtmlDialog);


    connect(ui->vstSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::vstSelectionChanged);

    // Initialize varnamHandler with default scheme ID or any initial setup
    varnamHandler = new VarnamHandler("ml"); // Replace with actual default scheme ID
    populateVSTSelector();
    if (varnamHandler == nullptr) {
        throw std::runtime_error("Failed to initialize VarnamHandler");
}
}

    MainWindow::~MainWindow()
    {
        delete ui;
        delete varnamHandler;
    }
    // here need implement other varnam config ui and functions in varnamcpp.h
    void MainWindow::on_indicNumberCheckBox_toggled(bool checked)
    {
        try {
        varnamHandler->enableIndicNumbers(checked);
        QString vstName = ui->vstSelector->currentText();
        if (checked) {
            ui->debugLabel->setText("Enabled indic numbers for: " + vstName);
        } else {
            ui->debugLabel->setText("Disabled indic numbers");
        }
        } catch (const std::runtime_error& e) {
        QMessageBox::critical(this, tr("Error"), tr(e.what()));
        }
    }

void MainWindow::populateVSTSelector()
{
    // Clear any existing items in the combobox
    ui->vstSelector->clear();

    // Predefined list of VSTs
    QStringList vstList = {"ml", "ne", "hi", "ta", "sa", "gu"};

    // Populate the combobox with the predefined VSTs
    ui->vstSelector->addItems(vstList);

    // Connect vstSelector's currentIndexChanged signal to vstSelectionChanged slot
    connect(ui->vstSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::vstSelectionChanged);
}

void MainWindow::vstSelectionChanged(int index)
{
    // Retrieve the selected scheme ID from vstSelector
    QString schemeId = ui->vstSelector->currentText();


    // Update varnamHandler with the selected scheme ID
    delete varnamHandler; // Clean up previous varnamHandler instance
    varnamHandler = new VarnamHandler(schemeId.toStdString());
    updateDebugLabel(schemeId);

}

void MainWindow::on_transliterateButton_clicked()
{
    // Implement transliteration functionality here
    QString inputText = ui->inputtextEdit->toPlainText();
    auto suggestions = varnamHandler->transliterate(inputText.toStdString());
    printSuggestions(suggestions);
    ui->debugLabel->setText("result");
}

void MainWindow::on_reverseTransliterateButton_clicked()
{
    // Implement reverse transliteration functionality here
    QString inputText = ui->inputtextEdit->toPlainText();
    auto suggestions = varnamHandler->reverseTransliterate(inputText.toStdString());
    printSuggestions(suggestions);
     ui->debugLabel->setText("result");
}

void MainWindow::on_trainButton_clicked()
{
    QString pattern = ui->patternLineEdit->text();
    QString word = ui->wordLineEdit->text();
    try {
        varnamHandler->train(pattern.toStdString(), word.toStdString());
        ui->debugLabel->setText("Trained pattern '" + pattern + "' => word '" + word + "'");
    } catch (const std::runtime_error& e) {
        ui->debugLabel->setText(QString::fromStdString(e.what()));
    }
}

void MainWindow::on_learnButton_clicked()
{
    // Implement learn functionality here
    QString word = ui->wordLineEdit->text();
    try {
        varnamHandler->learn(word.toStdString());
        ui->debugLabel->setText("Learned word '" + word + "'");
    } catch (const std::runtime_error& e) {
        ui->debugLabel->setText(QString::fromStdString(e.what()));
    }
}

void MainWindow::on_unlearnButton_clicked()
{
    // Get word from UI
    QString word = ui->wordLineEdit->text();

    try {
        // Call unlearn method on varnamHandler
        varnamHandler->unlearn(word.toStdString());
        ui->debugLabel->setText("Unlearned word '" + word + "'");
    } catch (const std::runtime_error& e) {
        // Handle runtime errors
        ui->debugLabel->setText(QString::fromStdString(e.what()));
    }
}

void MainWindow::on_learnFromFileButton_clicked()
{
    static bool fileSelected = false;

    if (!fileSelected) {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));

        if (!filePath.isEmpty()) {
            try {
                varnamHandler->learnFromFile(filePath.toStdString());

                // Process success message (optional)
                QMessageBox::information(this, tr("Success"), tr("File processing completed successfully."));
                fileSelected = true;
                 ui->debugLabel->setText("done! ");
            } catch (const std::runtime_error& e) {
                ui->debugLabel->setText(QString::fromStdString(e.what()));
            }
        }
    } else {
        // Informative message with potential reset option
        QMessageBox::information(this, tr("Information"), tr("A file has already been processed. You can restart the application to select a new file."));
    }
}

void MainWindow::on_trainFromFileButton_clicked()
{
    static bool trainingDone = false;

    if (!trainingDone) {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));

        if (!filePath.isEmpty()) {
            try {
                varnamHandler->trainFromFile(filePath.toStdString());
                trainingDone = true;

                // Process success message (optional)
                QMessageBox::information(this, tr("Success"), tr("File training completed successfully."));
            } catch (const std::runtime_error& e) {
               ui->debugLabel->setText(QString::fromStdString(e.what()));
            }
        }
    } else {
        // Informative message
        QMessageBox::information(this, tr("Information"), tr("A file has already been trained. You can restart the application to train a new file."));
    }
}

void MainWindow::on_exportButton_clicked()
{
    static bool exportDone = false; // Flag to track export state

    if (!exportDone) {
        QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text Files (*.txt);;All Files (*)"));

        if (!filePath.isEmpty()) {
            bool ok;
            int exportWordsPerFile = QInputDialog::getInt(this, tr("Export Words Per File"), tr("Number of words per file:"), 1000, 1, 1000000, 1, &ok);
            if (ok) {
               try {
                   varnamHandler->export_(filePath.toStdString(), exportWordsPerFile);

                   exportDone = true;  // Mark export as done after success

                   // Process success message (optional)
                   QMessageBox::information(this, tr("Success"), tr("Data exported successfully."));
               } catch (const std::runtime_error& e) {
                   ui->debugLabel->setText(QString::fromStdString(e.what())); // Assuming debugLabel for error display
               }
            }
        }
    } else {
        // Export already done, inform user
        QMessageBox::information(this, tr("Information"), tr("Data has already been exported. You can restart the application to export again."));
    }
}


void MainWindow::on_importButton_clicked()
{
    static bool importDone = false; // Flag to track import state

    if (!importDone) {
        try {
            // Open file dialog with custom filter for ".vlf" files
            QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Varnam Learning Files (*.vlf);;All Files (*)"));

            // Check if user cancelled or selected a file
            if (filePath.isEmpty()) {
                return;  // User cancelled or no file selected
            }

            // Call import function with converted std::string path
            varnamHandler->import(filePath.toStdString());

            importDone = true;  // Mark import as done after success

            // Process success message (optional)
            QMessageBox::information(this, tr("Success"), tr("Data imported successfully."));
        } catch (const std::runtime_error& e) {
            // Handle import error
            ui->debugLabel->setText(QString::fromStdString(e.what()));
        }
    } else {
        // Import already done, inform user
        QMessageBox::information(this, tr("Information"), tr("Data has already been imported. You can restart the application to import from a new file."));
    }
}



void MainWindow::printSuggestions(const std::vector<CSuggestion>& suggestions)
{
    QString result;
    for (const auto& sug : suggestions) {
        result += QString::fromStdString(sug.word) + " (Weight: " + QString::number(sug.weight) + ", Learned On: " + QString::number(sug.learnedOn) + ")\n";
    }
    ui->resultTextEdit->setText(result);
}

void MainWindow::updateDebugLabel(QString index) {
    QString vstName = ui->vstSelector->currentText();
    ui->debugLabel->setText("Selected VST: " + vstName);
}
void MainWindow::showHtmlDialog() {
    CustomHtmlDialog dialog(this);
    dialog.setHtmlContent(R"(
              <div class="text-center">
        <p style="margin-top:20px">this application is built for testing and training varnam. app uses govarnam c library and qt technology to create widgets. numbers can be enabled with checkbox.
other varnam configs are generaly not important here. but i will try to add those.</p>
<br><p> special thanks to @anoopms and @subins2000 in telegram </p>
<p style="color:indigo">https://t.me/khumnathx</p>
        </div>
    )");
    dialog.exec();
}
