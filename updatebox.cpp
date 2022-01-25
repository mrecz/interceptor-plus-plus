#include "updatebox.h"
#include "ui_updatebox.h"
#include <QMessageBox>
#include <QProcess>
#include <QDir>

UpdateBox::UpdateBox(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UpdateBox)
{
    ui->setupUi(this);
    ui->label->setText("A new version of the application is available. Do you want to perform update?");
    /** Call the maintenance tool and read an output */
    bool result = checkForUpdates();
    /** If there is any update, display the box */
    if (result)
    {
        show();
    }
}

UpdateBox::~UpdateBox()
{
    delete ui;
}

void UpdateBox::on_buttonBox_accepted()
{
    /** Ask a user if he want to perform updates. If so, close the application and run the maintenance tool */
    QStringList args("--updater");
    QString toolName = "maintenancetool.exe";
    QString path = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(toolName);
    QProcess::startDetached(path, args);
    exit(EXIT_SUCCESS);
}


void UpdateBox::on_buttonBox_rejected()
{
    hide();
}

bool UpdateBox::checkForUpdates()
{
    /** Call the maintenace tool and check the response about updates */
    QString toolName = "maintenancetool.exe";
    QString path = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(toolName);
    QProcess process;
    QStringList args("ch");
    process.start(path, args);

    process.waitForFinished();

    if (process.error() != QProcess::UnknownError)
    {
        qDebug() << process.errorString();
        return false;
    }

    /** If there are not any updates, maintenance tool returns a message including warning about no updates available */
    QByteArray data = process.readAllStandardOutput();
    if(data.contains("Warning: There are currently no updates available."))
    {
        qDebug() << "No updates available";
        return false;
    }
    /** If the previous message is not present, the updates are available, returns true */
    return true;
}

void UpdateBox::showNoUpdatesAvailableBox(QString title)
{
    QMessageBox mBox;
    mBox.information(this, title, "You are using the most up-to-date version!");
}

