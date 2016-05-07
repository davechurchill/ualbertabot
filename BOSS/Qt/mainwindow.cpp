#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

#include "BOSS.h"
#include "BuildOrderPlot.h"
#include "BOSSException.h"

std::map<std::string, QIcon> iconMap;
std::map<std::string, QListWidgetItem> itemMap;
std::map<std::string, QImage> imageMap;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    for (size_t r(0); r < BOSS::Races::NUM_RACES; ++r)
    {
        const std::vector<BOSS::ActionType> & actions = BOSS::ActionTypes::GetAllActionTypes((r));

        for (size_t a(0); a < actions.size(); ++a)
        {
            const std::string & name = actions[a].getName();
            QString imageFile = getImageName(actions[a]);

            iconMap[name] = QIcon(imageFile);
            itemMap[name] = QListWidgetItem(iconMap[name], QString::fromStdString(name));
            imageMap[name] = QImage(imageFile);
        }
    }

    ui->setupUi(this);

    ui->raceComboBox->addItem(iconMap["Protoss_Probe"], "Protoss");
    ui->raceComboBox->addItem(iconMap["Terran_SCV"], "Terran");
    ui->raceComboBox->addItem(iconMap["Zerg_Drone"], "Zerg");

    ui->armyValueGraph->setToolTip("Visualizes sum of resouces spent on fighting units over time.\n\nWorkers are not counted.");
    ui->resourceGraphButton->setToolTip("Visualizes resource totals over time.\n\nWill create separate graphs for minerals and gas collected.\n\nAppends _minerals and _gas to file chosen.");
    ui->visualizeButton->setToolTip("Visualizes build order concurrency and timings.");
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
    const QString & actionName = ui->actionTypeComboBox->currentText();
    addToList(ui->buildOrderList, actionName.toStdString());

    //ui->buildOrderList->addItem();
    ui->buildOrderList->scrollToBottom();
}

void MainWindow::on_addToStateButton_clicked()
{
    const QString & actionName = ui->actionTypeComboBox->currentText();
    addToList(ui->initialStateList, actionName.toStdString());

    //ui->buildOrderList->addItem();
    ui->initialStateList->scrollToBottom();
}

void MainWindow::setInitialState(const std::string & raceString)
{
    BOSS::RaceID race = BOSS::Races::GetRaceID(raceString);
    ui->initialStateList->clear();

    // set the initial state items to the default starting state
    addToList(ui->initialStateList, BOSS::ActionTypes::GetWorker(race));
    addToList(ui->initialStateList, BOSS::ActionTypes::GetWorker(race));
    addToList(ui->initialStateList, BOSS::ActionTypes::GetWorker(race));
    addToList(ui->initialStateList, BOSS::ActionTypes::GetWorker(race));
    addToList(ui->initialStateList, BOSS::ActionTypes::GetResourceDepot(race));

    if (race == BOSS::Races::Zerg)
    {
        addToList(ui->initialStateList, BOSS::ActionTypes::GetSupplyProvider(race));
    }
}

void MainWindow::setInitialBuildOrder(const std::string & raceString)
{
    BOSS::RaceID race = BOSS::Races::GetRaceID(raceString);
    ui->buildOrderList->clear();

    // add a sample build order to the build order list
    addToList(ui->buildOrderList, BOSS::ActionTypes::GetWorker(race));
    addToList(ui->buildOrderList, BOSS::ActionTypes::GetWorker(race));
    addToList(ui->buildOrderList, BOSS::ActionTypes::GetWorker(race));
    addToList(ui->buildOrderList, BOSS::ActionTypes::GetWorker(race));
    addToList(ui->buildOrderList, BOSS::ActionTypes::GetSupplyProvider(race));
    addToList(ui->buildOrderList, BOSS::ActionTypes::GetWorker(race));
    addToList(ui->buildOrderList, BOSS::ActionTypes::GetWorker(race));

    if (race == BOSS::Races::Protoss)
    {
        addToList(ui->buildOrderList, BOSS::ActionTypes::GetActionType("Protoss_Gateway"));
    }
    else if (race == BOSS::Races::Terran)
    {
        addToList(ui->buildOrderList, BOSS::ActionTypes::GetActionType("Terran_Barracks"));
    }
    else if (race == BOSS::Races::Zerg)
    {
        addToList(ui->buildOrderList, BOSS::ActionTypes::GetActionType("Zerg_Spawning_Pool"));
    }
}

void MainWindow::on_raceComboBox_currentIndexChanged(const QString &arg1)
{
    std::string raceString = arg1.toStdString();;

    BOSS::RaceID race = BOSS::Races::GetRaceID(raceString);

    ui->actionTypeComboBox->clear();
    ui->initialStateList->clear();
    ui->mineralsSpinBox->setValue(50);
    ui->gasSpinBox->setValue(0);

    setInitialState(raceString);
    setInitialBuildOrder(raceString);

    const auto & actionTypes = BOSS::ActionTypes::GetAllActionTypes(race);

    for (const auto & type : actionTypes)
    {
        if (type.getName() == "Zerg_Larva") continue;
        if (type.getName() == "Protoss_Dark_Archon") continue;
        if (type.getName() == "Protoss_Archon") continue;

        ui->actionTypeComboBox->addItem(iconMap[type.getName()], QString::fromStdString(type.getName()));
    }
}

void MainWindow::addToList(QListWidget * list, const BOSS::ActionType & type)
{
    list->insertItem(list->count(), new QListWidgetItem(itemMap[type.getName()]));
}

void MainWindow::addToList(QListWidget * list, const std::string & typeName)
{
    list->insertItem(list->count(), new QListWidgetItem(itemMap[typeName]));
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

    ui->imageLabel->setPixmap(QPixmap::fromImage(imageMap[type.getName()]));
}
BOSS::GameState MainWindow::getState()
{
    BOSS::GameState state(BOSS::Races::GetRaceID(ui->raceComboBox->currentText().toStdString()));
    state.setMinerals(ui->mineralsSpinBox->value());
    state.setGas(ui->gasSpinBox->value());

    for(int row = 0; row < ui->initialStateList->count(); row++)
    {
        state.addCompletedAction(BOSS::ActionTypes::GetActionType(ui->initialStateList->item(row)->text().toStdString()));
    }

    return state;
}

BOSS::BuildOrder MainWindow::getBuildOrder()
{
    BOSS::BuildOrder buildOrder;

    for(int row = 0; row < ui->buildOrderList->count(); row++)
    {
        buildOrder.add(BOSS::ActionTypes::GetActionType(ui->buildOrderList->item(row)->text().toStdString()));
    }

    return buildOrder;
}

QString MainWindow::getImageName(const BOSS::ActionType & type)
{
    std::string filename = "images/";

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
    generatePlot(PlotTypes::BuildOrderPlot);
}

void MainWindow::on_resourceGraphButton_clicked()
{
    generatePlot(PlotTypes::ResourcePlot);
}

void MainWindow::on_armyValueGraph_clicked()
{
    generatePlot(PlotTypes::ArmyPlot);
}


void MainWindow::generatePlot(int plotType)
{
    try
    {
        BOSS::GameState state = getState();
        BOSS::BuildOrder buildOrder = getBuildOrder();

        if (buildOrder.size() == 0)
        {
            QMessageBox::information(this, tr("BOSS Visualization Error"), tr("Cannot visualize an empty build order. Try adding some units.") );
            return;
        }

        if (buildOrder.isLegalFromState(state))
        {
            QString file = QFileDialog::getSaveFileName(this, tr("Save Visualization Gnuplot File"), QDir::currentPath(), tr("gnuplot files (*.gpl);;All files (*.*)"));

            BOSS::BuildOrderPlot plot(state, buildOrder);

            if (plotType == PlotTypes::BuildOrderPlot)
            {
                plot.writeRectanglePlot(file.toStdString());
            }
            else if (plotType == PlotTypes::ArmyPlot)
            {
                plot.writeArmyValuePlot(file.toStdString());
            }
            else if (plotType == PlotTypes::ResourcePlot)
            {
                plot.writeResourcePlot(file.toStdString());
            }
        }
        else
        {
            std::string why = buildOrder.whyIsNotLegalFromState(state);

            QMessageBox::information(this, tr("BOSS Visualization Error"), tr(why.c_str()) );
        }
    }
    catch (BOSS::BOSSException e)
    {
        std::stringstream ss;
        ss << "Build-Order Exception Thrown\n\nPlease contact author with error details\n\n" << e.what();

        QMessageBox::information(this, "BOSS Visualization Error", ss.str().c_str());
    }
}

void MainWindow::on_loadBuildOrderButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Load Build-Order File"), QDir::currentPath(), tr("Text Files (*.txt);;All files (*.*)"));
    if (file.size() == 0) return;

    std::ifstream fin(file.toStdString());

    std::string line;
    std::string raceString;

    // read the first line of the file which should be the race
    std::getline(fin, raceString);
    BOSS::RaceID race = BOSS::Races::GetRaceID(raceString);
    if (race == BOSS::Races::None)
    {
        QMessageBox::information(this, tr("BOSS Visualization Error"), tr("First line of the build-order file was not a valid race.\n\nIt must be Protoss, Terran, or Zerg.") );
        return;
    }

    size_t lineNum = 1;
    std::vector<BOSS::ActionType> types;

    // read the remaining lines which each should be one build-order item
    while (std::getline(fin, line))
    {
        // break when we reach a blank line
        if (line.size() == 0)
        {
            break;
        }

        ++lineNum;
        if (!BOSS::ActionTypes::TypeExists(line))
        {
            std::stringstream ss;
            ss << "Line " << lineNum << " of the build order file was not a valid Build-Order ActionType.";

            QMessageBox::information(this, tr("BOSS Visualization Error"), tr(ss.str().c_str()) );
            return;
        }

        BOSS::ActionType type = BOSS::ActionTypes::GetActionType(line);

        if (type.getRace() != race)
        {
            std::stringstream ss;
            ss << "Build order file specifies " << raceString << " race.\n\nLine " << lineNum << ": " << type.getName() << " is not a " << raceString << " unit.";
            ss << "\n\nPlease ensure all units are of the same race and try again.";

            QMessageBox::information(this, tr("BOSS Visualization Error"), tr(ss.str().c_str()) );
            return;
        }

        types.push_back(type);
    }

    // we must change the selected race to the race loaded
    if (raceString != ui->raceComboBox->currentText().toStdString())
    {
        for (int i=0; i < ui->raceComboBox->count(); ++i)
        {
            std::string itemRace = ui->raceComboBox->itemText(i).toStdString();

            if (itemRace == raceString)
            {
                ui->raceComboBox->setCurrentIndex(i);
                break;
            }
        }
    }

    ui->buildOrderList->clear();

    for (const auto & type : types)
    {
        addToList(ui->buildOrderList, type.getName());
    }

    fin.close();

    ui->buildOrderList->scrollToBottom();
}

void MainWindow::on_saveBuildOrderButton_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save Build-Order"), QDir::currentPath(), tr("Text Files (*.txt);;All files (*.*)"));
    if (file.size() == 0) return;

    std::ofstream fout(file.toStdString());

    fout << ui->raceComboBox->currentText().toStdString() << "\n";
    for(int row = 0; row < ui->buildOrderList->count(); row++)
    {
        fout << ui->buildOrderList->item(row)->text().toStdString() << "\n";
    }

    ui->buildOrderList->scrollToBottom();
    fout.close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("About");
    msgBox.setIconPixmap(QPixmap::fromImage(imageMap["Protoss_Probe"]));
    msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
    msgBox.setText("StarCraft Build-Order Visualizer<br><br>Written by <a href=\"http://webdocs.cs.ualberta.ca/~cdavid/\">David Churchill</a><br><br>Source and details available on <a href=\"https://github.com/davechurchill/ualbertabot\">GitHub</a><br><br>To view output graphs, use <a href=\"http://www.gnuplot.info/\">gnuplot</a>");
    msgBox.exec();
}

void MainWindow::on_loadStateButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Load State File"), QDir::currentPath(), tr("Text Files (*.txt);;All files (*.*)"));
    if (file.size() == 0) return;

    std::ifstream fin(file.toStdString());

    std::string line;
    std::string raceString;

    // read the first line of the file which should be the race
    std::getline(fin, raceString);
    BOSS::RaceID race = BOSS::Races::GetRaceID(raceString);
    if (race == BOSS::Races::None)
    {
        QMessageBox::information(this, tr("BOSS Visualization Error"), tr("First line of the state file was not a valid race.\n\nIt must be Protoss, Terran, or Zerg.") );
        return;
    }

    size_t lineNum = 1;
    std::vector<BOSS::ActionType> types;

    // read the remaining lines which each should be one build-order item
    while (std::getline(fin, line))
    {
        // break when we reach a blank line
        if (line.size() == 0)
        {
            break;
        }

        ++lineNum;
        if (!BOSS::ActionTypes::TypeExists(line))
        {
            std::stringstream ss;
            ss << "Line " << lineNum << " of the state file was not a valid ActionType.";

            QMessageBox::information(this, tr("BOSS Visualization Error"), tr(ss.str().c_str()) );
            return;
        }

        BOSS::ActionType type = BOSS::ActionTypes::GetActionType(line);

        if (type.getRace() != race)
        {
            std::stringstream ss;
            ss << "State file specifies " << raceString << " race.\n\nLine " << lineNum << ": " << type.getName() << " is not a " << raceString << " unit.";
            ss << "\n\nPlease ensure all units are of the same race and try again.";

            QMessageBox::information(this, tr("BOSS Visualization Error"), tr(ss.str().c_str()) );
            return;
        }

        types.push_back(type);
    }

    // we must change the selected race to the race loaded
    if (raceString != ui->raceComboBox->currentText().toStdString())
    {
        for (int i=0; i < ui->raceComboBox->count(); ++i)
        {
            std::string itemRace = ui->raceComboBox->itemText(i).toStdString();

            if (itemRace == raceString)
            {
                ui->raceComboBox->setCurrentIndex(i);
                break;
            }
        }
    }

    ui->initialStateList->clear();

    for (const auto & type : types)
    {
        addToList(ui->initialStateList, type.getName());
    }

    fin.close();

    ui->initialStateList->scrollToBottom();
}

void MainWindow::on_saveStateButton_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save State"), QDir::currentPath(), tr("Text Files (*.txt);;All files (*.*)"));
    if (file.size() == 0) return;

    std::ofstream fout(file.toStdString());

    fout << ui->raceComboBox->currentText().toStdString() << "\n";
    for(int row = 0; row < ui->initialStateList->count(); row++)
    {
        fout << ui->initialStateList->item(row)->text().toStdString() << "\n";
    }

    fout.close();
}


void MainWindow::on_viewStateButton_clicked()
{
    BOSS::GameState state = getState();
    QMessageBox::about(this, "State Details", state.toString().c_str());
}

void MainWindow::on_buildOrderDetails_clicked()
{
    std::stringstream ss;

    BOSS::GameState state = getState();
    BOSS::BuildOrder buildOrder = getBuildOrder();

    if (buildOrder.isLegalFromState(state))
    {
        ss << "Build-Order can be built from the chosen initial state.\n\n";

        buildOrder.doActions(state);
        int totalFrames = state.getLastActionFinishTime();

        ss << "Total time to completion: " << totalFrames << " frames " << " (" << (totalFrames / (60 * 24)) << "m " << ((totalFrames / 24) % 60) << "s)\n\n";

        int totalMinerals = 0;
        int totalGas = 0;
        for (size_t a(0); a < buildOrder.size(); ++a)
        {
            totalMinerals += buildOrder[a].mineralPrice();
            totalGas+= buildOrder[a].gasPrice();
        }

        ss << "Total Minerals Gathered: " << (state.getMinerals() + totalMinerals)/BOSS::Constants::RESOURCE_SCALE << "\n";
        ss << "Total Gas Gathered: " << (state.getGas() + totalGas)/BOSS::Constants::RESOURCE_SCALE << "\n\n";
    }
    else
    {
        ss << buildOrder.whyIsNotLegalFromState(state);
    }

    QMessageBox::about(this, "Build-Order Details", ss.str().c_str() );
}

void MainWindow::on_viewFinalStateButton_clicked()
{
    std::stringstream ss;
    BOSS::GameState state = getState();
    BOSS::BuildOrder buildOrder = getBuildOrder();

    if (buildOrder.isLegalFromState(state))
    {
        ss << "Final state after build-order has completed:\n";

        buildOrder.doActions(state);

        state.fastForward(state.getLastActionFinishTime());

        ss << state.toString() << "\n";

        int totalMinerals = 0;
        int totalGas = 0;
        for (size_t a(0); a < buildOrder.size(); ++a)
        {
            totalMinerals += buildOrder[a].mineralPrice();
            totalGas+= buildOrder[a].gasPrice();
        }

        ss << "Total Minerals Gathered: " << (state.getMinerals() + totalMinerals)/BOSS::Constants::RESOURCE_SCALE << "\n";
        ss << "Total Gas Gathered: " << (state.getGas() + totalGas)/BOSS::Constants::RESOURCE_SCALE << "\n\n";
    }
    else
    {
        ss << buildOrder.whyIsNotLegalFromState(state);
    }

    QMessageBox::about(this, "Final State Details", ss.str().c_str() );
}
