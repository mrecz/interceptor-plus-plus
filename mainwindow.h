#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void notImplemented();


private slots:

    void on_actionExit_triggered();

    void on_actionHelp_triggered();

    void on_actionTake_Shot_triggered();

    void on_actionSave_triggered();

    void on_actionAbout_triggered();

    void on_actionSave_As_triggered();

    void displayScreenshot();

    void on_actionAdd_Rect_changed();

private:
    Ui::MainWindow *ui;

    const QString WINDOW_TITLE{"Interceptor++"};
    class Overlay* overlay;
    class GraphicsScene* scene;

signals:
    void rectButtonChanged(bool bIsChecked);

};
#endif // MAINWINDOW_H
