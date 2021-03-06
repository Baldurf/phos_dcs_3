#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "feesamcli.h"
#include "phosdialog.h"

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    FeeSamCli *_FeeClient;
    PHOSDialog *pDialog;
    QString FeeServerName;
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
