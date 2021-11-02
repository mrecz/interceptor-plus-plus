#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsscene.h"
#include "overlay.h"
#include "interceptor.h"

#ifdef _WIN32
#include "hotkey.h"
#include "customeventfilter.h"
#endif // _WIN32

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , interceptor(new Interceptor(this))
#ifdef _WIN32
    , takeScreenshotHotkey(new Hotkey(this, MODIFIERS::NOREPEAT, KEYCODES::KEY_PRINTSCR, winId(), "PrintSCR", WINDOW_TITLE))
#endif // _WIN32
{
    overlay = new Overlay(interceptor);
    scene = new GraphicsScene(interceptor, this);

#ifdef _WIN32
    /** Install Windows Event Filter and register global hotkey */
    nativeEventFilter = new CustomEventFilter(overlay->winId());
    qApp->installNativeEventFilter(nativeEventFilter);
#endif // _WIN32

    ui->setupUi(this);
    ui->graphicsView->setScene(scene);
    /** Rubber Band Properties - It is enabled and with the red background color */
    setStyleSheet("selection-background-color: red;");
    ui->graphicsView->setDragMode(QGraphicsView::DragMode::NoDrag);

    /** Construct the tray icon if possible */
#ifndef QT_NO_SYSTEMTRAYICON
    createTrayIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
#endif // QT_NO_SYSTEMTRAYICON

    connect(overlay, SIGNAL(screenshotCreated()), this, SLOT(displayScreenshot()));
    connect(overlay, SIGNAL(cancelled()), this, SLOT(displayMainApp()));
#ifdef _WIN32
    connect(nativeEventFilter, SIGNAL(captureCurrentScreen()), this, SLOT(saveCurrentScreenAsPixmap()));
    connect(nativeEventFilter, SIGNAL(registredKeyPressed()), this, SLOT(on_actionTake_Shot_triggered()));
#endif // _WIN32
}

void MainWindow::closeEvent(QCloseEvent* event)
{
#ifndef QT_NO_SYSTEMTRAYICON
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::TextFormat::RichText);
    msgBox.information(this, WINDOW_TITLE, "The program will keep running in the "
                                           "system tray. To terminate the program, "
                                           "choose <b>Quit</b> in the context menu "
                                           "of the system tray entry.");
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
                   "</hr>"
                   "<p> </p>"
                   "<p><i>The program is distributed under the GPL-2.0 License. The source code is available on GitHub: <a href='https://github.com/mrecz/interceptor-plus-plus'>https://github.com/mrecz/interceptor-plus-plus</i></p>"
               "</div>"
               );
}

void MainWindow::saveCurrentScreenAsPixmap()
{
    interceptor->saveWholeScreenAsPixmap();
}

void MainWindow::on_actionTake_Shot_triggered()
{
    if (interceptor->getWholeScreenMap().isNull())
    {
        interceptor->saveWholeScreenAsPixmap();
    }
    overlay->show();
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
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
             /** Restore the window */
            setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
            break;
        default:
            return;
    }
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
    delete overlay;
    delete scene;
    delete interceptor;
#ifdef _WIN32
    delete nativeEventFilter;
    delete takeScreenshotHotkey;
#endif // _WIN32
}






