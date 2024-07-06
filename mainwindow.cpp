#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

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
    connect(ui->dictSuggestionsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_dictSuggestionsSpinBox_valueChanged);
    connect(ui->patternDictSuggestionsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_patternDictSuggestionsSpinBox_valueChanged);
    connect(ui->tokenizerSuggestionsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_tokenizerSuggestionsSpinBox_valueChanged);
    connect(ui->tokenizerSuggestionsAlwaysCheckBox, &QCheckBox::toggled, this, &MainWindow::on_tokenizerSuggestionsAlwaysCheckBox_toggled);
    connect(ui->vstSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::vstSelectionChanged);

    // Initialize varnamHandler with default scheme ID or any initial setup
    varnamHandler = new VarnamHandler("ml"); // Replace with actual default scheme ID
    populateVSTSelector();
    if (varnamHandler == nullptr) {
        throw std::runtime_error("Failed to initialize VarnamHandler");
    }

    // Initialize default config values
    ui->indicNumberCheckBox->setChecked(true); // Indic numbers enable
    ui->dictSuggestionsSpinBox->setValue(10);  // Dictionary Suggestions Limit
    ui->patternDictSuggestionsSpinBox->setValue(5); // Pattern Dictionary Suggestions Limit
    ui->tokenizerSuggestionsSpinBox->setValue(3);  // Tokenizer Suggestions Limit
    ui->tokenizerSuggestionsAlwaysCheckBox->setChecked(false); // Tokenizer Suggestions Always
    ui->ignoreDuplicateTokenCheckBox->setChecked(false); // Ignore Duplicate Token

    updateVarnamConfig(); // Apply initial configuration
}

MainWindow::~MainWindow()
{
    delete ui;
    delete varnamHandler;
}

void MainWindow::updateVarnamConfig()
{
    try {
        varnamHandler->updateVarnamConfig(
            ui->indicNumberCheckBox->isChecked(),
            ui->dictSuggestionsSpinBox->value(),
            ui->patternDictSuggestionsSpinBox->value(),
            ui->tokenizerSuggestionsSpinBox->value(),
            ui->tokenizerSuggestionsAlwaysCheckBox->isChecked(),
            ui->ignoreDuplicateTokenCheckBox->isChecked()
            );
    } catch (const std::runtime_error &e) {
        QMessageBox::critical(this, tr("Error"), tr(e.what()));
    }
}

void MainWindow::on_dictSuggestionsSpinBox_valueChanged(int value)
{
    updateVarnamConfig();
}

void MainWindow::on_patternDictSuggestionsSpinBox_valueChanged(int value)
{
    updateVarnamConfig();
}

void MainWindow::on_tokenizerSuggestionsSpinBox_valueChanged(int value)
{
    updateVarnamConfig();
}

void MainWindow::on_tokenizerSuggestionsAlwaysCheckBox_toggled(bool checked)
{
    updateVarnamConfig();
}

void MainWindow::on_indicNumberCheckBox_toggled(bool checked)
{
    updateVarnamConfig();
}

void MainWindow::populateVSTSelector()
{
    ui->vstSelector->clear();

    // Predefined list of VSTs
    QStringList vstList = {"ml", "ne", "hi", "ta", "sa", "gu"};
        if (vstList.isEmpty()) {
               ui->debugLabel->setText("Error: No VST schemes available.");
        return;
    }
    ui->vstSelector->addItems(vstList);
    connect(ui->vstSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::vstSelectionChanged);
}

void MainWindow::vstSelectionChanged(int index)
{
    QString schemeId = ui->vstSelector->currentText();

    // Clean up previous varnamHandler instance
    delete varnamHandler;
    varnamHandler = nullptr;

    // Try to create a new VarnamHandler instance
    try {
        varnamHandler = new VarnamHandler(schemeId.toStdString());
        // If successful, update the UI and varnamHandler settings
        updateDebugLabel(schemeId);
        updateVarnamConfig();
        ui->debugLabel->setText("Initialized scheme: " + schemeId);
    } catch (const std::exception& e) {
        // If there is an error, show an error message in the debug label
        ui->debugLabel->setText("Error initializing scheme: " + schemeId + ". " + e.what());
    }
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
    } catch (const std::runtime_error &e) {
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
    } catch (const std::runtime_error &e) {
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
    } catch (const std::runtime_error &e) {
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
            } catch (const std::runtime_error &e) {
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
            } catch (const std::runtime_error &e) {
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

                    exportDone = true; // Mark export as done after success

                    // Process success message (optional)
                    QMessageBox::information(this, tr("Success"), tr("Data exported successfully."));
                } catch (const std::runtime_error &e) {
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
                return; // User cancelled or no file selected
            }

            // Call import function with converted std::string path
            varnamHandler->import(filePath.toStdString());

            importDone = true; // Mark import as done after success

            // Process success message (optional)
            QMessageBox::information(this, tr("Success"), tr("Data imported successfully."));
        } catch (const std::runtime_error &e) {
            // Handle import error
            ui->debugLabel->setText(QString::fromStdString(e.what()));
        }
    } else {
        // Import already done, inform user
        QMessageBox::information(this, tr("Information"), tr("Data has already been imported. You can restart the application to import from a new file."));
    }
}

void MainWindow::printSuggestions(const std::vector<CSuggestion> &suggestions)
{
    QString result;
    for (const auto &sug : suggestions) {
        result += QString::fromStdString(sug.word) + " (Weight: " + QString::number(sug.weight) + ", Learned On: " + QString::number(sug.learnedOn) + ")\n";
    }
    ui->resultTextEdit->setText(result);
}

void MainWindow::updateDebugLabel(QString index)
{
    QString vstName = ui->vstSelector->currentText();
    ui->debugLabel->setText("Selected VST: " + vstName);
}

void MainWindow::showHtmlDialog()
{
    CustomHtmlDialog dialog(this);
    dialog.setHtmlContent(R"(
              <div class="text-center">
        <p style="margin-top:20px">this application is built for testing and training varnam. app uses govarnam c library and qt technology to create widgets. VARNAM_CONFIG available in system are implemented
other than enable suggestion and enable dead consonants.</p>
<br><p> special thanks to @anoopms and @subins2000 in telegram </p>
<p style="color:indigo">https://t.me/khumnathx</p>
        </div>
    )");
    dialog.exec();
}
