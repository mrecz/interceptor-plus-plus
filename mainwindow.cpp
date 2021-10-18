#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsscene.h"
#include "overlay.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , overlay(new Overlay())
    , scene(new GraphicsScene(this))
{
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
    mBox.about(this, WINDOW_TITLE,
               "The program is designed for capturing screenshots. \n\n"
               "See the important shortcuts below: \n"
               "<F1>         HELP \n"
               "<CTRL> + <T> TAKE SCREENSHOT \n"
               "<CTRL> + <S> SAVE \n"
               "<CTRL> + <X> EXIT \n\n"
               "m.reznicek@quadient.com"
               );
}

void MainWindow::on_actionTake_Shot_triggered()
{
    this->hide();
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
    delete ui;
    delete overlay;
    delete scene;
}






