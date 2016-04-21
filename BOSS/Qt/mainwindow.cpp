#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

#include "BOSS.h"
#include "BuildOrderPlot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    ui->raceComboBox->addItem("Protoss");
    ui->raceComboBox->addItem("Terran");
    ui->raceComboBox->addItem("Zerg");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_deleteItemsButton_clicked()
{
    qDeleteAll(ui->buildOrderList->selectedItems());
}

void MainWindow::on_addToBuildOrderButton_clicked()
{
    ui->buildOrderList->addItem(ui->actionTypeComboBox->currentText());
    ui->buildOrderList->scrollToBottom();
}

void MainWindow::on_raceComboBox_currentIndexChanged(const QString &arg1)
{
    std::string raceString = arg1.toStdString();;

    BOSS::RaceID race = BOSS::Races::GetRaceID(raceString);

    ui->actionTypeComboBox->clear();
    ui->buildOrderList->clear();
    ui->initialStateList->clear();
    ui->mineralsSpinBox->setValue(50);
    ui->gasSpinBox->setValue(0);

    // set the initial state items to the default starting state
    ui->initialStateList->addItem(QString::fromStdString(BOSS::ActionTypes::GetWorker(race).getName()));
    ui->initialStateList->addItem(QString::fromStdString(BOSS::ActionTypes::GetWorker(race).getName()));
    ui->initialStateList->addItem(QString::fromStdString(BOSS::ActionTypes::GetWorker(race).getName()));
    ui->initialStateList->addItem(QString::fromStdString(BOSS::ActionTypes::GetWorker(race).getName()));
    ui->initialStateList->addItem(QString::fromStdString(BOSS::ActionTypes::GetResourceDepot(race).getName()));

    if (race == BOSS::Races::Zerg)
    {
        ui->initialStateList->addItem(QString::fromStdString(BOSS::ActionTypes::GetSupplyProvider(race).getName()));
    }

    const auto & actionTypes = BOSS::ActionTypes::GetAllActionTypes(race);

    for (const auto & type : actionTypes)
    {
        if (type.getName() == "Zerg_Larva") continue;
        if (type.getName() == "Protoss_Dark_Archon") continue;
        if (type.getName() == "Protoss_Archon") continue;

        ui->actionTypeComboBox->addItem(QString::fromStdString(type.getName()));
    }
}

void MainWindow::on_addToStateButton_clicked()
{
    ui->initialStateList->addItem(ui->actionTypeComboBox->currentText());
    ui->initialStateList->scrollToBottom();
}

void MainWindow::on_clearAllItems_clicked()
{
    ui->buildOrderList->clear();
}

void MainWindow::on_deleteItemsButtonState_clicked()
{
    qDeleteAll(ui->initialStateList->selectedItems());
}

void MainWindow::on_clearAllItemsState_clicked()
{
    ui->initialStateList->clear();
}

void MainWindow::on_actionTypeComboBox_currentIndexChanged(const QString &arg1)
{
    if (arg1.length() == 0)
    {
        return;
    }

    BOSS::ActionType type = BOSS::ActionTypes::GetActionType(arg1.toStdString());

    std::stringstream minerals, gas, buildTime, builder, hpShield;
    minerals << type.mineralPrice()/1000;
    gas << type.gasPrice()/1000;
    buildTime << type.buildTime();
    builder << type.whatBuildsActionType().getName();

    if (type.isUnit())
    {
        hpShield << type.getUnitType().maxHitPoints() << " / " << type.getUnitType().maxShields();
    }
    else
    {
        hpShield << "N/A";
    }

    ui->itemInformationLabel->setText(arg1);
    ui->mineralCost->setText(QString::fromStdString(minerals.str()));
    ui->gasCost->setText(QString::fromStdString(gas.str()));
    ui->buildTime->setText(QString::fromStdString(buildTime.str()));
    ui->builder->setText(QString::fromStdString(builder.str()));
    ui->hpShield->setText(QString::fromStdString(hpShield.str()));

    QImage image(getImageName(type));
    ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}

QString MainWindow::getImageName(const BOSS::ActionType & type)
{
    std::string filename = "../asset/images/";

    if (type.isUnit())
    {
        filename += "units/" + type.getName() + ".png";
    }
    else if (type.isUpgrade() || type.isTech())
    {
        filename += "command_icons/" + type.getName() + ".png";
    }

    return QString::fromStdString(filename);
}

void MainWindow::on_visualizeButton_clicked()
{
    BOSS::GameState state(BOSS::Races::GetRaceID(ui->raceComboBox->currentText().toStdString()));

    for(int row = 0; row < ui->initialStateList->count(); row++)
    {
        state.addCompletedAction(BOSS::ActionTypes::GetActionType(ui->initialStateList->item(row)->text().toStdString()));
    }

    BOSS::BuildOrder buildOrder;

    for(int row = 0; row < ui->buildOrderList->count(); row++)
    {
        buildOrder.add(BOSS::ActionTypes::GetActionType(ui->buildOrderList->item(row)->text().toStdString()));
    }

    if (buildOrder.size() == 0)
    {
        QMessageBox::information(this, tr("BOSS Visualization Error"), tr("Cannot visualize an empty build order. Try adding some units.") );
        return;
    }


    if (buildOrder.isLegalFromState(state))
    {
        QString file = QFileDialog::getSaveFileName(this, tr("Save gnuplot file as"), QDir::currentPath(), tr("gnuplot files (*.gpl);;All files (*.*)"));

        BOSS::BuildOrderPlot plot(state, buildOrder);
        plot.writeRectanglePlot(file.toStdString());
    }
    else
    {
        QMessageBox::information(this, tr("BOSS Visualization Error"), tr("The build order you entered is not legal from the initial state. Please try another.") );
    }


}
