#include "../SparCraft.h"
#include "SparCraftExperiment.h"

using namespace SparCraft;

int main(int argc, char *argv[])
{
    SparCraft::init();
    AIParameters::Instance().parseFile("SparCraft_Config.txt");

    SparCraftExperiment exp;
    exp.parseConfigFile("SparCraft_Config.txt");

    return 0;
}
