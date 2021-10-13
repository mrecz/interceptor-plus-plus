#include <QMessageBox>
#include <QGraphicsScene>
#include <QColor>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interceptor.h"
#include "overlay.h"
#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    overlay = new Overlay();
    scene = new QGraphicsScene();

    connect(overlay, SIGNAL(screenshotCreated()), this, SLOT(displayScreenshot()));
}


void MainWindow::notImplemented()
{
    QMessageBox mBox;
    mBox.information(this, WINDOW_TITLE, "This feature will come soon.");
}


void MainWindow::on_actionSave_As_triggered()
{
   notImplemented();
}


void MainWindow::displayScreenshot()
{
    scene->clear();

    if (ui->actionBorder->isChecked())
    {
        QPixmap border = QPixmap(Interceptor::screenshotMap->width() + 4, Interceptor::screenshotMap->height() + 4);
        border.fill(QColor(203, 203, 203));
        scene->addPixmap(border);

    }
    QGraphicsPixmapItem* img = scene->addPixmap(*Interceptor::screenshotMap);
    img->setPos(img->x() + 2, img->y() + 2);
    ui->graphicsView->setScene(scene);

    Interceptor::cleanup();
}


void MainWindow::on_actionExit_triggered()
{
    qApp->exit();
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
    overlay->show();
}


void MainWindow::on_actionSave_triggered()
{
    notImplemented();
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox mBox;
    mBox.aboutQt(this, WINDOW_TITLE);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete overlay;
    delete scene;
}




