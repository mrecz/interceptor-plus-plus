#ifndef UPDATEBOX_H
#define UPDATEBOX_H

#include <QDialog>

namespace Ui {
class UpdateBox;
}

/** This class is only for checking available updates from remote repository if the application is installed via QT installer framework  */
class UpdateBox : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateBox(QWidget *parent = nullptr);
    ~UpdateBox();

    void showNoUpdatesAvailableBox(QString title);
    bool checkForUpdates();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::UpdateBox *ui;
};

#endif // UPDATEBOX_H
