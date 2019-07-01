#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSettings settings("Tenchrio", "XInputDisabler");
    setWindowTitle("XInputDisabler");
    SetXinputComboBox();
    SetSavedSettings();
    CreateTrayIcon();
    my_isDisabled = false;

    if (settings.value("disableOnStart").toBool()){
        ToggleDevice();
    }

    //Connections
    connect(this->ui->my_SaveButton,SIGNAL(clicked()),this,SLOT(SetDevice()));
    connect(this->ui->my_DeviceListCbox,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(CheckChoice()));
    connect(this->ui->my_OnlyPointerCheck,SIGNAL(clicked()),this,SLOT(ToggleOnlyKPChoice()));
    connect(this->ui->my_OnlyKeyboardCheck,SIGNAL(clicked()),this,SLOT(ToggleOnlyKPChoice()));
    connect(this->ui->my_DisableOnStartup,SIGNAL(clicked()),this,SLOT(CheckStartUp()));
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(TrayActivation(QSystemTrayIcon::ActivationReason)));
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

    QAction *show_action = new QAction( "Open settings", trayIcon );
    connect( show_action, SIGNAL(triggered()), this, SLOT(ShowWindow()) );

    QMenu *tray_icon_menu = new QMenu;
    tray_icon_menu->addAction( show_action );
    tray_icon_menu->addAction( quit_action );
    trayIcon->setContextMenu( tray_icon_menu );
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
     if (trayIcon->isVisible()) {
         hide();
         event->ignore();
     }
 }

void MainWindow::ShowWindow()
 {
    //Nice way to refresh the input
    SetXinputComboBox();
    show();
 }

void MainWindow::SetSavedSettings()
{
    QSettings settings("Tenchrio", "XInputDisabler");
    this->ui->my_OnlyPointerCheck->setChecked(settings.value("onlyPointer").toBool());
    this->ui->my_OnlyKeyboardCheck->setChecked(settings.value("onlyKeyboard").toBool());
    this->ui->my_DisableOnStartup->setChecked(settings.value("disableOnStart").toBool());
}

//It is pointless for both Keyboard and Pointer to be enable
//But it also has to be possible to have both off so we can't have a radio button (without a third option which seems counter intuitive)
void MainWindow::ToggleOnlyKPChoice()
{
    QSettings settings("Tenchrio","XInputDisabler");
    if (sender() == this->ui->my_OnlyPointerCheck) {
        this->ui->my_OnlyKeyboardCheck->setChecked(false);
        settings.setValue("onlyKeyboard",false);
        if (this->ui->my_OnlyPointerCheck->isChecked() == true){
            settings.setValue("onlyPointer",true);
        } else {
            settings.setValue("onlyPointer",false);
        }
    } else {
        this->ui->my_OnlyPointerCheck->setChecked(false);
        settings.setValue("onlyPointer",false);
        if (this->ui->my_OnlyKeyboardCheck->isChecked() == true){
            settings.setValue("onlyKeyboard",true);
        } else {
            settings.setValue("onlyKeyboard",false);
        }
    }
}

void MainWindow::CheckStartUp()
{
    QSettings settings("Tenchrio", "XInputDisabler");
    settings.setValue("disableOnStart",this->ui->my_DisableOnStartup->isChecked());
}

void MainWindow::TrayActivation(QSystemTrayIcon::ActivationReason reason)
{
    QSettings settings("Tenchrio", "XInputDisabler");
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        ToggleDevice();
        break;
    case QSystemTrayIcon::MiddleClick:
        ToggleDevice();
        break;
    default:
        ;
    }
}



void MainWindow::ToggleDevice()
{
    trayIcon->showMessage("What?","Enabling device");
    QSettings settings("Tenchrio", "XInputDisabler");

    QStringList allchoices = GetXinputList();
    QStringList choices;

    //Check for the choice's ID in the list, sometimes a device has both a pointer and a keyboard (e.g. Razer Naga, Razer Nostromo etc etc)
    for (int var = 0; var < allchoices.size(); ++var) {
        if (allchoices[var].contains(settings.value("choice").toString())) {
            if (allchoices[var].contains("slave  keyboard") && settings.value("onlyKeyboard")==false) {
                choices << allchoices[var];
            } else if (allchoices[var].contains("slave  pointer") && settings.value("onlyKeyboard")==false){
                choices << allchoices[var];
            }

        }
    }

    //Next we extract the id from the extracted choices and disable/enable them
    for (int var = 0; var < choices.size(); ++var) {
        choices[var].truncate(choices[var].indexOf("["));
        choices[var].remove(0,choices[var].indexOf("id="));
        choices[var].replace("id=","");
        QProcess action;
        if (my_isDisabled){
            QString secondCommand = "xinput enable " + choices[var];
            action.start(secondCommand);
            action.waitForFinished();
            trayIcon->showMessage("What?","Enabled device '" + choices[var] + "'");
            trayIcon->setIcon(QIcon(":/mouse.png"));
        } else {
            QString secondCommand = "xinput disable " + choices[var];
            action.start(secondCommand);
            action.waitForFinished();
            trayIcon->showMessage("What?","Disabled device '" + choices[var] + "'");
            trayIcon->setIcon(QIcon(":/mousedisabled.png"));
        }
    }
    //We toggle the isDisabled variable
    my_isDisabled = !my_isDisabled;
}
