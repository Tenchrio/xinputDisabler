#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SetXinputComboBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//This function allows us to get the latest list of Xinput devices
//I do not use GREP as piping does not work with QProcess and it can be worked around without forcing a specific shell
QStringList MainWindow::GetXinputList(){
    QProcess getproc;
    getproc.start("xinput list");
    getproc.waitForFinished();
    QString output(getproc.readAllStandardOutput());
    return output.split("\n");
}

//A function we call to set the devices in our Combobox
void MainWindow::SetXinputComboBox(){
    QSettings settings;

    auto devices = GetXinputList();
    for (int var = 0; var<devices.size(); var++) {
        devices[var].truncate(devices[var].indexOf("id="));
        devices[var].replace("↳","");
        devices[var].replace("⎜","");
        devices[var].replace("⎡","");
        devices[var].replace("⎣","");
        //Remove whitespace at the end, for some reason a do while loop won't work
        devices[var].remove(devices[var].size()-6,6);
        //Not even sure if this do while works considering the above statement.
        do {
            devices[var].remove(0,1);
        } while(devices[var].startsWith(" "));

        if (devices[var] == settings.value("choice").toString() && devices[var]!=""){
            this->ui->my_SaveButton->setEnabled(false);
            my_isDisabled = false;
        }
    }
    this->ui->my_DeviceListCbox->addItems(devices);
}
