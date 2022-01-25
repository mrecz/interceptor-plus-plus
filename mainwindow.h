#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QSharedPointer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
const QString WINDOW_TITLE{"Interceptor++"};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();    

    void notImplemented();

protected:
    void closeEvent(class QCloseEvent* event) override;

private slots:
    void on_actionExit_triggered();

    void on_actionHelp_triggered();

    void on_actionSave_triggered();

    void on_actionAbout_triggered();

    void on_actionSave_As_triggered();

    void on_actionTake_Shot_triggered();

    void displayScreenshot();

    void on_actionAdd_Rect_changed();

    void displayMainApp();

    void deleteOverlays();

    void on_actionBorder_changed();

    void on_actionCopy_to_Clipboard_triggered();

    void saveCurrentScreenAsPixmap();

#ifndef QT_NO_SYSTEMTRAYICON
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
#endif // QT_NO_SYSTEMTRAYICON

    void on_actionNumbers_changed();

    void on_actionCheck_for_Updates_triggered();

private:
    Ui::MainWindow *ui;

    class std::vector<class Overlay*> overlays;
    class GraphicsScene* scene;
    class Interceptor* interceptor;
    class UpdateBox* updateBox;
    bool bShouldBeTrayDialogDisplayed;

#ifndef QT_NO_SYSTEMTRAYICON
    QSystemTrayIcon* trayIcon;
    class QMenu* trayMenu;
    class QAction* minimizeAction;
    QAction* maximizeAction;
    QAction* restoreAction;
    QAction* quitAction;
    void createTrayIcon();
#endif // QT_NO_SYSTEMTRAYICON

#ifdef _WIN32
    class CustomEventFilter* nativeEventFilter;
    class Hotkey* takeScreenshotHotkey;
#endif // _WIN32

signals:
    void rectButtonChanged(bool bIsChecked);
    void numbersButtonChanged(bool bIsChecked);
    void borderButtonChanged(bool bIsChecked);

};
#endif // MAINWINDOW_H
