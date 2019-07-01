#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QSettings>
#include <QAction>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SetXinputComboBox();
    CreateTrayIcon();

    //Connections
    connect(this->ui->my_SaveButton,SIGNAL(clicked()),this,SLOT(SetDevice()));
    connect(this->ui->my_DeviceListCbox,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(CheckChoice()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//This function allows us to get the latest list of Xinput devices
//I do not use GREP as piping does not work with QProcess and it can be worked around without forcing a specific shell
QStringList MainWindow::GetXinputList()
{
    QProcess getproc;
    getproc.start("xinput list");
    getproc.waitForFinished();
    QString output(getproc.readAllStandardOutput());
    return output.split("\n");
}

//A function we call to set the devices in our Combobox
void MainWindow::SetXinputComboBox()
{
    QSettings settings("Tenchrio","XInputDisabler");

    auto devices = GetXinputList();
    int index = 0;
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
            this->ui->my_SaveButton->setEnabled(false);\
            index = var;
            my_isDisabled = false;
        }
    }
    this->ui->my_DeviceListCbox->addItems(devices);
    this->ui->my_DeviceListCbox->setCurrentIndex(index);
}

void MainWindow::SetDevice()
{
    QSettings settings("Tenchrio", "XInputDisabler");
    settings.setValue("choice",this->ui->my_DeviceListCbox->currentText());
    this->ui->my_SaveButton->setEnabled(false);
}

void MainWindow::CheckChoice()
{
    QSettings settings("Tenchrio", "XInputDisabler");
    if (settings.value("choice").toString() == this->ui->my_DeviceListCbox->currentText()){
        this->ui->my_SaveButton->setEnabled(false);
    } else {
        this->ui->my_SaveButton->setEnabled(true);
    }
}

void MainWindow::CreateTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/mouse.png"));
    trayIcon->setToolTip(tr("XInputDisabler"));
    trayIcon->setVisible(true);
    trayIcon->show();

    //Set up the drop menu for the trayicon
    QAction *quit_action = new QAction( "Close app", trayIcon );
    connect( quit_action, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()) );

    QMenu *tray_icon_menu = new QMenu;
    tray_icon_menu->addAction( quit_action );
    trayIcon->setContextMenu( tray_icon_menu );
}
