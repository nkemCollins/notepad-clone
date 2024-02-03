#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include<QWidget>
#include<QTextEdit>
#include<QFile>
#include<QFileDialog>
#include<QtPrintSupport/QPrinter>
#include<QtPrintSupport/QPrintDialog>
#include<QLabel>
#include<QLineEdit>
#include<QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);

    setCentralWidget(tabWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setFilePath(int currentTabIndex, QString fileName){
    tabWidget->widget(currentTabIndex)->setProperty("filePath", fileName);
}
QString MainWindow::getFilePath(int tabIndex){
    QWidget *currentWidget = tabWidget->widget(tabIndex);
    QString filePath = currentWidget->property("filePath").toString();
    return filePath;
}

void MainWindow::keyBoardShorts(QTextEdit* text){
    QShortcut *shortcutCopy = new QShortcut(QKeySequence::Copy, this);
    connect(shortcutCopy, &QShortcut::activated, text, &QTextEdit::copy);

    QShortcut *shortcutPaste = new QShortcut(QKeySequence::Paste, this);
    connect(shortcutPaste, &QShortcut::activated, text, &QTextEdit::paste);

    QShortcut *shortcutCut = new QShortcut(QKeySequence::Cut, this);
    connect(shortcutCut, &QShortcut::activated, text, &QTextEdit::cut);

    QShortcut *shortcutDelete = new QShortcut(QKeySequence("Ctrl+D"), text);
    connect(shortcutDelete, &QShortcut::activated, [text](){
        text->textCursor().removeSelectedText();
    });

    QShortcut *shortcutUndo = new QShortcut(QKeySequence::Undo, this);
    connect(shortcutUndo, &QShortcut::activated, text, &QTextEdit::undo);

    QShortcut *shortcutRedo = new QShortcut(QKeySequence::Redo, this);
    connect(shortcutRedo, &QShortcut::activated, text, &QTextEdit::redo);

}

// void MainWindow::onOpenDialog() {
//     QDialog *dialog = new QDialog(this); // 'this' ensures the dialog is centered on the main window
//     dialog->setWindowTitle("Dialog Box");

//     QVBoxLayout *layout = new QVBoxLayout(dialog);
//     QLabel *label = new QLabel("Find text", dialog);
//     QLineEdit *line = new QLineEdit();
//     layout->addWidget(label);

//     // You can add more widgets to the dialog here

//     dialog->setLayout(layout);
//     dialog->exec(); // Use exec() for a modal dialog that blocks input to other windows
// }

void MainWindow::on_actionNew_triggered()
{
    QTextEdit *text = new QTextEdit;

    QFont font;
    font.setPointSize(12);
    text->setFont(font);

    tabWidget->addTab(text,tr("Untitled"));

    //keyboard shortcuts
    keyBoardShorts(text);
}

void MainWindow::addNewTab(QString fileName, QString content){
    QTextEdit *text = new QTextEdit;

    QFont font;
    font.setPointSize(12);
    text->setFont(font);

    int index = tabWidget->addTab(text, QFileInfo(fileName).fileName());
    text->setPlainText(content);

    //keyboard shortcuts
    keyBoardShorts(text);

    setFilePath(index, fileName);
}

void MainWindow::onTabCloseRequested(int index)
{
    tabWidget->removeTab(index);
}

//get currentTabContent
QString MainWindow::getCurrentTabContent() {
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1) {
        // Check if a tab is selected
        QTextEdit* text = qobject_cast<QTextEdit*>(tabWidget->widget(currentIndex));
        if (text) {
            return text->toPlainText();  // Return the text content of the current tab
        }
    }
    return QString();  // Return an empty string if no tab is selected or it doesn't contain a QTextEdit
}

//Open file content
void MainWindow::openFileContent(QWidget *parentWidget){
    QString fileName = QFileDialog::getOpenFileName(parentWidget,"Open File");
    if(fileName.isEmpty()){
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Handle error - the file might not be accessible
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    addNewTab(fileName,content);
}

//save file content
void MainWindow::saveFileContent(const QString& content, QWidget* parentWidget) {
    int currentIndex = tabWidget->currentIndex();
        QString fileName = QFileDialog::getSaveFileName(
            parentWidget,
            "Save File",
            "",
            "Text Files (*.txt);;All Files (*)"
            );

    if (!fileName.isEmpty()) {
            setFilePath(currentIndex,fileName);
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << content;
            file.close();

            QFileInfo fileInfo(fileName);
            tabWidget->setTabText(currentIndex, fileInfo.fileName());
        }
    }
}

void MainWindow::saveExistingFile(const QString& filePath, const QString& content) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;
        file.close();
    }
}

void MainWindow::on_actionSave_triggered(){
    QString content = getCurrentTabContent();
    int tabIndex = tabWidget->currentIndex();
    QString filePath = getFilePath(tabIndex);
    if(filePath.isEmpty()){
        saveFileContent(content,this);
    }else{
        saveExistingFile(filePath, content);
    }
}

void MainWindow::on_actionSave_As_triggered(){
    QString content = getCurrentTabContent();
    saveFileContent(content,this);
}

void MainWindow::on_actionNew_Window_triggered()
{

        MainWindow w;

        w.setWindowTitle("MyNotes");
        w.show();
}

void MainWindow::on_actionOpen_triggered(){
    openFileContent(this);
}

void MainWindow::on_actionSave_All_triggered(){
    int count = tabWidget->count();

    if(count>0){
        for(int i=0; i<count; ++i){
            QTextEdit* text = qobject_cast<QTextEdit*>(tabWidget->widget(i));
            if(text){
                QString content =text->toPlainText();
                if(!content.isEmpty()){
                    QString filePath = getFilePath(i);
                    if(filePath.isEmpty()){
                        QString fileName = QFileDialog::getSaveFileName(
                            this,
                            "Save File",
                            "",
                            "Text Files (*.txt);;All Files (*)"
                            );

                        if(!fileName.isEmpty()) {
                            setFilePath(i,fileName);
                            QFile file(fileName);
                            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                                QTextStream out(&file);
                                out << content;
                                file.close();

                                QFileInfo fileInfo(fileName);
                                tabWidget->setTabText(i, fileInfo.fileName());
                            }
                        }
                    }else{
                        saveExistingFile(filePath, content);
                    }
                }
            }
    }
 }
}

void MainWindow::on_actionClose_Tab_triggered(){
    tabWidget->removeTab(tabWidget->currentIndex());
}

void MainWindow::on_actionExit_triggered(){
    QApplication::quit();
}

void MainWindow::on_actionPrint_triggered(){
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    printer.setPrintRange(QPrinter::AllPages);

    QTextEdit *text = findChild<QTextEdit*>();
    if (text && printDialog.exec() == QDialog::Accepted) {
        text->print(&printer);
    }
}

void MainWindow::on_actionCopy_triggered(){
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1) {
        // Check if a tab is selected
        QTextEdit* text = qobject_cast<QTextEdit*>(tabWidget->widget(currentIndex));
        text->copy();
    }
}

void MainWindow::on_actionUndo_triggered(){
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1) {
        // Check if a tab is selected
        QTextEdit* text = qobject_cast<QTextEdit*>(tabWidget->widget(currentIndex));
        text->undo();
    }
}

void MainWindow::on_actionCut_triggered(){
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1) {
        // Check if a tab is selected
        QTextEdit* text = qobject_cast<QTextEdit*>(tabWidget->widget(currentIndex));
        text->cut();
    }
}

void MainWindow::on_actionPaste_triggered(){
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1) {
        // Check if a tab is selected
        QTextEdit* text = qobject_cast<QTextEdit*>(tabWidget->widget(currentIndex));
        text->paste();
    }
}

void MainWindow::on_actionDelete_triggered(){
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1) {
        // Check if a tab is selected
        QTextEdit* text = qobject_cast<QTextEdit*>(tabWidget->widget(currentIndex));
        QTextCursor cursor = text->textCursor();
        cursor.removeSelectedText();
    }
}


void MainWindow::on_actionFind_triggered(){
    //  QString content = getCurrentTabContent();
    // // bool result = content->;
    // // if(result){
    // //     qDebug() << "Found match";
    // // }
    //  onOpenDialog();
}

void MainWindow::on_actionStatus_Bar_triggered()
{
    statusBar()->showMessage("UTF-8");
}


void MainWindow::on_actionBold_Italic_2_triggered()
{
    QTextEdit* text = qobject_cast<QTextEdit*>(tabWidget->widget(tabWidget->currentIndex()));
    QFont font;
    font.setBold(true);
    font.setItalic(true);
    text->setFont(font);
}

