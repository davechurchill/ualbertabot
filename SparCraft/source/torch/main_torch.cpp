#include "../SparCraft.h"
#include "../torch/TorchTools.h"

using namespace SparCraft;

int main(int argc, char *argv[])
{
    SparCraft::init();
    AIParameters::Instance().parseFile("SparCraft_Config.txt");

    std::string requestType;
    std::cin >> requestType;

    if (requestType == "Move")
    {
        TorchTools::PrintMoveFromFrameStream(std::cin);
    }
    else if (requestType == "Value")
    {
        TorchTools::PrintStateValueFromFrameStream(std::cin);
    }
    return 0;
}
