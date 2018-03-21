#include "../SparCraft.h"
#include <experimental/filesystem>
#include "SparCraftExperiment.h"

using namespace SparCraft;
namespace filesystem = std::experimental::filesystem;

int main(int argc, char *argv[])
{
	auto path = filesystem::current_path();
	auto sparcraftConfigPath = path / "SparCraft_Config.txt";
	if (!filesystem::exists(sparcraftConfigPath))
	{
		std::cout << "File SparCraft_Config.txt does not exists in the path " << path << std::endl;
		return 1;
	}

	auto sparcraftConfigFile = sparcraftConfigPath.string();
    SparCraft::init();
	auto& parameters = AIParameters::Instance();
	parameters.parseFile(sparcraftConfigFile);

    SparCraftExperiment exp;
    auto experimentConfigurations = exp.parseConfigFile(sparcraftConfigFile);
#ifndef SPARCRAFT_NOGUI
	auto assetsPath = path / "../../../../SparCraft/asset/images/";
	if (!filesystem::exists(assetsPath))
	{
		assetsPath = path / "SparCraft/asset/images/";
	}

	if (!filesystem::exists(assetsPath))
	{
		assetsPath = path / "asset/images/";
	}
#endif

	for (const auto& experiment : experimentConfigurations)
	{
#ifndef SPARCRAFT_NOGUI
		exp.runExperiementWithUI(experiment, assetsPath.string(), parameters);
#else
		exp.runExperiement(experiment, parameters);
#endif
	}

    return 0;
}
