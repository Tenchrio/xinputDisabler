#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //Here we keep track what the state is off the device
    bool my_isDisabled;
    QStringList GetXinputList();
    QSystemTrayIcon *trayIcon;

    void SetXinputComboBox();
    void CreateTrayIcon();
public slots:
    void SetDevice();
    void CheckChoice();
};

#endif // MAINWINDOW_H
