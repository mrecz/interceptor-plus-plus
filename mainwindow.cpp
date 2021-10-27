#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsscene.h"
#include "overlay.h"
#include "interceptor.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , interceptor(new Interceptor(this))
{
    overlay = new Overlay(interceptor);
    scene = new GraphicsScene(interceptor, this);
    ui->setupUi(this);
    ui->graphicsView->setScene(scene);
    setMouseTracking(true);

    connect(overlay, SIGNAL(screenshotCreated()), this, SLOT(displayScreenshot()));
    connect(overlay, SIGNAL(cancelled()), this, SLOT(displayMainApp()));
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
    this->show();
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
   this->show();
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
                        "<dt><b>CTRL + T</b></dt>"
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

void MainWindow::on_actionTake_Shot_triggered()
{
    interceptor->saveWholeScreenAsPixmap();
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
}

void MainWindow::on_actionBorder_changed()
{
    emit borderButtonChanged(ui->actionBorder->isChecked());
}

void MainWindow::on_actionCopy_to_Clipboard_triggered()
{
    scene->render(MODE::CLIPBOARD);
}

MainWindow::~MainWindow()
{
    /** When the application window is destroyed, all objects are deleted */
    delete ui;
    delete overlay;
    delete scene;
    delete interceptor;
}






