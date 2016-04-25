#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "BOSS.h"

namespace PlotTypes
{
    enum { BuildOrderPlot, ArmyPlot, ResourcePlot };
}

namespace Ui
{
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
    void on_saveBuildOrderButton_clicked();
    void on_resourceGraphButton_clicked();
    void on_armyValueGraph_clicked();
    void on_actionAbout_triggered();
    void on_loadStateButton_clicked();
    void on_saveStateButton_clicked();

    QString getImageName(const BOSS::ActionType & type);
    BOSS::GameState getState();
    BOSS::BuildOrder getBuildOrder();
    void generatePlot(int plotType);

    void on_loadBuildOrderButton_clicked();

    void setInitialState(const std::string & raceString);
    void setInitialBuildOrder(const std::string & raceString);
    void addToList(QListWidget * list, const BOSS::ActionType & type);
    void addToList(QListWidget * list, const std::string & typeName);


    void on_viewStateButton_clicked();

    void on_buildOrderDetails_clicked();

    void on_viewFinalStateButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
