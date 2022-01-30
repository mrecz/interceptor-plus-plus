#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsscene.h"
#include "overlay.h"
#include "interceptor.h"
#include "updatebox.h"
#include <vector>

#ifdef _WIN32
#include "hotkey.h"
#include "customeventfilter.h"
#endif // _WIN32

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , interceptor(new Interceptor(this))
    , bShouldBeTrayDialogDisplayed(true)    
#ifdef _WIN32
    , takeScreenshotHotkey(new Hotkey(this, MODIFIERS::NOREPEAT, KEYCODES::KEY_PRINTSCR, winId(), "PrintSCR", WINDOW_TITLE))
#endif // _WIN32    
{
    scene = new GraphicsScene(interceptor, this);

#ifdef _WIN32
    /** Install Windows Event Filter and register global hotkey */
    nativeEventFilter = new CustomEventFilter();
    qApp->installNativeEventFilter(nativeEventFilter);
#endif // _WIN32

    ui->setupUi(this);
    ui->graphicsView->setScene(scene);
    /** Rubber Band Properties - It is enabled and with the red background color */
    setStyleSheet("selection-background-color: red;");
    ui->graphicsView->setDragMode(QGraphicsView::DragMode::NoDrag);
    /** Without underlying image, drawing functionality is disabled*/
    ui->actionAdd_Rect->setDisabled(true);
    ui->actionNumbers->setDisabled(true);

    /** Construct the tray icon if possible */
#ifndef QT_NO_SYSTEMTRAYICON
    createTrayIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
#endif // QT_NO_SYSTEMTRAYICON
#ifdef _WIN32
    connect(nativeEventFilter, SIGNAL(captureCurrentScreen()), this, SLOT(saveCurrentScreenAsPixmap()));
    connect(nativeEventFilter, SIGNAL(registredKeyPressed()), this, SLOT(on_actionTake_Shot_triggered()));
#endif // _WIN32
    /** Wait for a second and check for available updates; useful only if the application is installed by using the online installer */
    QTimer::singleShot(1000, this, [this](void){ updateBox = new UpdateBox(); });
}

void MainWindow::closeEvent(QCloseEvent* event)
{
#ifndef QT_NO_SYSTEMTRAYICON
    if (bShouldBeTrayDialogDisplayed)
    {
        QMessageBox msgBox;
        msgBox.setTextFormat(Qt::TextFormat::RichText);
        msgBox.information(this, WINDOW_TITLE, "The program will keep running in the "
                                               "system tray. To terminate the program, "
                                               "choose <b>Quit</b> in the context menu "
                                               "of the system tray entry."
                                               "<p> </p>"
                                               "<b>This dialog is displayed only once.</b>");
        bShouldBeTrayDialogDisplayed = false;
    }
    hide();
    event->ignore();
#else
    QWindow::closeEvent(event);
#endif // QT_NO_SYSTEMTRAYICON
}

void MainWindow::notImplemented()
{
    QMessageBox mBox;
    mBox.information(this, WINDOW_TITLE, "This feature is not implemented.");
}

void MainWindow::on_actionSave_As_triggered()
{
   notImplemented();
}

void MainWindow::displayScreenshot()
{
    ui->actionAdd_Rect->setDisabled(false);
    ui->actionNumbers->setDisabled(false);
    displayMainApp();
    /** Restore the window */
    setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    /** Before adding a new sreenshot to the scene, clear and delete all old objects */
    scene->clearReferencedObjects();
    scene->clear();

    scene->addImageToScene();

    if (ui->actionBorder->isChecked())
    {
        scene->addBorderToScene();
    }    
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

void MainWindow::displayMainApp()
{
   show();
   deleteOverlays();
}

void MainWindow::deleteOverlays()
{
    /** All Overlay wdigets must be destroyed and disconnected; clear the vector do delete smart pointers which also deletes the referenced objects */
    for(const auto& overlay : overlays)
    {
        overlay->deleteLater();
    }
    overlays.clear();
}

void MainWindow::on_actionHelp_triggered()
{
    QMessageBox mBox;
    mBox.setTextFormat(Qt::TextFormat::RichText);
    mBox.about(this, WINDOW_TITLE,
               "<div>"
                   "<b>The program is designed for capturing screenshots.</b>"
                   "<p> </p>"
                   "<p>See the important shorcuts below:</p>"
                   "<p> </p>"
                   "<dl>"
                        "<dt><b>F1</b></dt>"
                        "<dt>HELP</dt>"
                        "<dt><b>PrintSCR</b></dt>"
                        "<dt>TAKE SCREENSHOT</dt>"
                        "<dt><b>ESC</b></dt>"
                        "<dt>CANCEL SCREENSHOT WINDOW</dt>"
                        "<dt><b>CTRL + S</b></dt>"
                        "<dt>SAVE SCREENSHOT</dt>"
                        "<dt><b>CTRL + X</b></dt>"
                        "<dt>EXIT</dt>"
                   "</dl>"
                   "<p> </p>"
                   "<p><b><u>Drawing Mode</u></b> - <i>Can be enabled by clicking on the Drawing Mode button. Available only if the screenshot exists.</i></p>"
                   "<p>If Drawing Mode is enabled, you can draw new objects by holding the <b>right mouse button</b> and dragging the mouse.</p>"
                   "<p>Existing rectangles can be moved by holding the <b>left mouse button</b> and dragging the mouse.</p>"
                   "<p>Existing rectangles can be resized by placing the cursor over the object and using the <b>mouse wheel</b>.</p>"
                   "<p>Existing rectangles can be deleted by placing the cursor over the object and double clicking with the <b>left mouse button</b>.</p>"
                   "<p> </p>"
                   "<p><b><u>Numbering Mode</u></b> - <i>Can be enabled by clicking on the Numbers button. Available only if the screenshot exists.</i></p>"
                   "<p>If Numbering mode is enabled, you can add new numbers by right clicking with the <b>right mouse button</b>. The maximum number of numbers is 10 and each number can exist only once.</p>"
                   "<p>If Numbering mode is enabled, existing numbers can be resized by using the <b>mouse wheel</b>. ALL NUMBERS ARE RESIZED AT ONCE!</p>"
                   "<p>Existing numbers can be moved by holding the <b>left mouse button</b> and dragging the mouse.</p>"
                   "<p>Existing numbers can be deleted by placing the cursor over the number and double clicking with the <b>left mouse button</b>.</p>"
                   "<p> </p>"
                   "</hr>"
                   "<p> </p>"
                   "<p><i>The program is distributed under the GPL-2.0 License. The source code is available on GitHub: <a href='https://github.com/mrecz/interceptor-plus-plus'>https://github.com/mrecz/interceptor-plus-plus</i></p>"
               "</div>"
               );
}

void MainWindow::saveCurrentScreenAsPixmap()
{
    interceptor->saveScreensBackgroundAsPixmap();
}

void MainWindow::on_actionTake_Shot_triggered()
{
    /** Recheck available screens */
    interceptor->getScreenPointers();
    /** Create overlay widget for each screen and store their WinIDs */
    const auto screensMap = interceptor->getScreens();
    for (const auto& screen : screensMap)
    {
        /** Each overlay holds the name of the display for which is created; it is used then for identifying from which screen the screenshot is taken */
        overlays.push_back(new Overlay(interceptor, screen.second));
    }
    /** Connect signals from all created Overlay widgets */
    std::vector<int> overlayWinIDs;
    for (const auto& overlay : overlays)
    {
        connect(overlay, SIGNAL(screenshotCreated()), this, SLOT(displayScreenshot()));
        connect(overlay, SIGNAL(cancelledDisplayMainApp()), this, SLOT(displayMainApp()));
        connect(overlay, SIGNAL(cancelled()), this, SLOT(deleteOverlays()));
        overlayWinIDs.push_back(overlay->winId());

        /** If the screenshot action is triggered directly from the main window, the main window should be displayed even if the screenshot is cancelled */
        overlay->setWasMainWindowVisible(isVisible());
    }
    /** WINIDs of all created overlay widgets must be known by event filter */
    nativeEventFilter->setWinIDs(overlayWinIDs);

    if (isVisible())
    {
        hide();
    }

    if (!interceptor->isBackgroundSaved())
    {
        saveCurrentScreenAsPixmap();
    }    

    for (const auto& overlay : overlays)
    {
        overlay->showFullScreen();
        overlay->activateWindow();
    }

}

void MainWindow::on_actionSave_triggered()
{
    scene->render(MODE::FILE);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox mBox;
    mBox.aboutQt(this, WINDOW_TITLE);
}

void MainWindow::on_actionAdd_Rect_changed()
{
    if (ui->actionNumbers->isChecked() && ui->actionAdd_Rect->isChecked())
    {
        ui->actionNumbers->setChecked(false);
    }
    emit rectButtonChanged(ui->actionAdd_Rect->isChecked());
    if (ui->actionAdd_Rect->isChecked())
    {
        ui->graphicsView->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
    }
    else
    {
       ui->graphicsView->setDragMode(QGraphicsView::DragMode::NoDrag);
    }
}

void MainWindow::on_actionBorder_changed()
{
    emit borderButtonChanged(ui->actionBorder->isChecked());
}

void MainWindow::on_actionCopy_to_Clipboard_triggered()
{
    scene->render(MODE::CLIPBOARD);
}

#ifndef QT_NO_SYSTEMTRAYICON
void MainWindow::createTrayIcon()
{
    minimizeAction = new QAction("Minimize", this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction("Maximize", this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction("Restore", this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayMenu = new QMenu(this);
    trayMenu->addAction(restoreAction);
    trayMenu->addAction(minimizeAction);
    trayMenu->addAction(maximizeAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayMenu);

    QIcon icon = QIcon(":/img/IconTray");

    trayIcon->setIcon(icon);
    trayIcon->setToolTip(WINDOW_TITLE);
    trayIcon->show();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::DoubleClick:
             /** Restore the window */
            setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
            displayMainApp();
            break;
        default:
            return;
    }
}

void MainWindow::on_actionNumbers_changed()
{
   if (ui->actionAdd_Rect->isChecked() && ui->actionNumbers->isChecked())
   {
       ui->actionAdd_Rect->setChecked(false);
   }
   emit numbersButtonChanged(ui->actionNumbers->isChecked());
}

#endif // QT_NO_SYSTEMTRAYICON

MainWindow::~MainWindow()
{
    /** When the application window is destroyed, all objects are deleted */
#ifndef QT_NO_SYSTEMTRAYICON
    delete trayIcon;
    delete trayMenu;
    delete minimizeAction;
    delete maximizeAction;
    delete restoreAction;
    delete quitAction;
#endif // QT_NO_SYSTEMTRAYICON
    delete ui;
    delete scene;
    delete interceptor;
    delete updateBox;
#ifdef _WIN32
    delete nativeEventFilter;
    delete takeScreenshotHotkey;
#endif // _WIN32
}


void MainWindow::on_actionCheck_for_Updates_triggered()
{
    bool result = updateBox->checkForUpdates();
    if (result)
    {
        updateBox->show();
    }
    else
    {
        updateBox->showNoUpdatesAvailableBox(WINDOW_TITLE);
    }
}

