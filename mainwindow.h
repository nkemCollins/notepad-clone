#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void onTabCloseRequested(int index);

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionNew_Window_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_All_triggered();

    void on_actionClose_Tab_triggered();

    void on_actionExit_triggered();

    void on_actionPrint_triggered();

    void on_actionCopy_triggered();

    void on_actionUndo_triggered();

    void on_actionCut_triggered();

    void on_actionPaste_triggered();

    void on_actionDelete_triggered();

    void on_actionFind_triggered();

    void on_actionStatus_Bar_triggered();

    void on_actionBold_Italic_2_triggered();

private:
    Ui::MainWindow *ui;
    QTabWidget *tabWidget;
    QString getCurrentTabContent();
    void saveFileContent(const QString &content, QWidget *parentWidget);
    void saveExistingFile(const QString &filePath, const QString &content);
    //QString currentFilePath;
    void openFileContent(QWidget *parentWidget);
    void addNewTab(QString fileName, QString content);
    void setFilePath(int currentTabIndex, QString fileName);
    QString getFilePath(int tabIndex);
    void onOpenDialog();
    void keyBoardShorts(QTextEdit *text);
};
#endif // MAINWINDOW_H
