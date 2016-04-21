#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "BOSS.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_deleteItemsButton_clicked();
    void on_addToBuildOrderButton_clicked();
    void on_visualizeButton_clicked();
    void on_raceComboBox_currentIndexChanged(const QString &arg1);
    void on_addToStateButton_clicked();
    void on_clearAllItems_clicked();
    void on_deleteItemsButtonState_clicked();
    void on_clearAllItemsState_clicked();
    void on_actionTypeComboBox_currentIndexChanged(const QString &arg1);

    QString getImageName(const BOSS::ActionType & type);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
