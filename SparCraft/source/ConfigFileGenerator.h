#pragma once

#include "Common.h"
#include "SearchExperiment.h"
#include "UCTSearch.h"
#include "PlayerProperties.h"
#include "UnitProperties.h"

// you shouldn't run this, I used it for my paper
void generateExperimentFile(int searchTimer, int maxChildren, int numStates, int numUnits, bool sep)
{
    std::string display("Display true C:\\Users\\Dave\\Desktop\\sc2010\\StarCraft_Bot\\Visual_Studio_Projects\\SparCraft\\starcraft_images\\");
        
    std::string stat = (sep ? "sep" : "sym");
    std::stringstream folder; folder << "exp\\exp_" << numUnits << "_" << stat;
    std::stringstream command; command << "mkdir " << folder.str();

    // Creates the directory
    system("mkdir exp");
    system(command.str().c_str());

    std::stringstream states;
    if (sep)
    {
        states << "State SeparatedState 100 128 128 400 360 840 360 Protoss_Dragoon "   << numUnits << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Zerg_Zergling "     << numUnits << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Protoss_Dragoon "   << (numUnits/2) << " Protoss_Zealot " << (numUnits/2) << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Protoss_Dragoon "   << (numUnits/2) << " Terran_Marine " << (numUnits/2) << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Terran_Marine "     << (numUnits/2) << " Zerg_Zergling " << (numUnits/2) << "\n";
    }
    else
    {
        states << "State StateSymmetric 100 128 128 Protoss_Dragoon "   << numUnits << "\n";
        states << "State StateSymmetric 100 128 128 Zerg_Zergling "     << numUnits << "\n";
        states << "State StateSymmetric 100 128 128 Protoss_Dragoon "   << (numUnits/2) << " Protoss_Zealot " << (numUnits/2) << "\n";
        states << "State StateSymmetric 100 128 128 Protoss_Dragoon "   << (numUnits/2) << " Terran_Marine " << (numUnits/2) << "\n";
        states << "State StateSymmetric 100 128 128 Terran_Marine "     << (numUnits/2) << " Zerg_Zergling " << (numUnits/2) << "\n";
    }

    std::stringstream exp1;
    exp1 << "Player 0 AlphaBeta " << searchTimer << " " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp1 << "Player 0 AlphaBeta " << searchTimer << " " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp1 << "Player 1 PortfolioGreedySearch 40 NOKDPS 1 0\n";
    exp1 << "ResultsFile c:\\users\\dave\\desktop\\results\\exp\\exp_" << numUnits << "_" << stat << "\\results_" << numUnits << "_ab_vs_portfolio_" << stat << " true\n";

    std::stringstream exp2;
    exp2 << "Player 0 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp2 << "Player 0 UCT " << searchTimer << " 1.5 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp2 << "Player 1 PortfolioGreedySearch 40 NOKDPS 1 0\n";
    exp2 << "ResultsFile c:\\users\\dave\\desktop\\results\\exp\\exp_" << numUnits << "_" << stat << "\\results_" << numUnits << "_uct_vs_portfolio_" << stat << " true\n";

    std::stringstream exp3;
    exp3 << "Player 0 AlphaBeta " << searchTimer << " " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp3 << "Player 1 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp3 << "Player 1 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp3 << "ResultsFile c:\\users\\dave\\desktop\\results\\exp\\exp_" << numUnits << "_" << stat << "\\results_" << numUnits << "_ab_vs_uct_nok_" << stat << " true\n";

    std::stringstream exp4;
    exp4 << "Player 0 AlphaBeta " << searchTimer << " " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp4 << "Player 1 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp4 << "Player 1 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp4 << "ResultsFile c:\\users\\dave\\desktop\\results\\exp\\exp_" << numUnits << "_" << stat << "\\results_" << numUnits << "_ab_vs_uct_none_" << stat << " true\n";

    std::stringstream exp5;
    exp5 << "Player 0 PortfolioGreedySearch 0 NOKDPS 1 0\n";
    exp5 << "Player 1 PortfolioGreedySearch 0 NOKDPS 2 0\n";
    exp5 << "Player 1 PortfolioGreedySearch 0 NOKDPS 3 0\n";
    exp5 << "ResultsFile c:\\users\\dave\\desktop\\results\\exp\\exp_" << numUnits << "_" << stat << "\\results_" << numUnits << "_p_vs_p_iter_" << stat << " true\n";
    
    std::stringstream exp6;
    exp6 << "Player 0 PortfolioGreedySearch 0 NOKDPS 1 0\n";
    exp6 << "Player 1 PortfolioGreedySearch 0 NOKDPS 1 1\n";
    exp6 << "Player 1 PortfolioGreedySearch 0 NOKDPS 1 2\n";
    exp6 << "ResultsFile c:\\users\\dave\\desktop\\results\\exp\\exp_" << numUnits << "_" << stat << "\\results_" << numUnits << "_p_vs_p_resp_" << stat << " true\n";
     
    std::stringstream exp7;
    exp7 << "Player 0 PortfolioGreedySearch 0 NOKDPS 1 0\n";
    exp7 << "Player 1 PortfolioGreedySearch 0 NOKDPS 1 1\n";
    exp7 << "Player 1 PortfolioGreedySearch 0 NOKDPS 1 2\n";
    exp7 << "ResultsFile c:\\users\\dave\\desktop\\results\\exp\\exp_" << numUnits << "_" << stat << "\\results_" << numUnits << "_p_vs_p_resp_" << stat << " true\n";

    std::stringstream f1; f1 << folder.str() << "\\exp_" << numUnits << "_ab_vs_portfolio_"     << stat << ".txt";
    std::stringstream f2; f2 << folder.str() << "\\exp_" << numUnits << "_uct_vs_portfolio_"    << stat << ".txt";
    std::stringstream f3; f3 << folder.str() << "\\exp_" << numUnits << "_ab_vs_uct_nok_"       << stat << ".txt";
    std::stringstream f4; f4 << folder.str() << "\\exp_" << numUnits << "_ab_vs_uct_none_"      << stat << ".txt";
    std::stringstream f5; f5 << folder.str() << "\\exp_" << numUnits << "_p_vs_p_iter_"         << stat << ".txt";
    std::stringstream f6; f6 << folder.str() << "\\exp_" << numUnits << "_p_vs_p_resp_"         << stat << ".txt";

    std::ofstream fout1(f1.str().c_str()); fout1 << exp1.str(); fout1 << states.str(); fout1 << display; fout1.close();
    std::ofstream fout2(f2.str().c_str()); fout2 << exp2.str(); fout2 << states.str(); fout2 << display; fout2.close();
    std::ofstream fout3(f3.str().c_str()); fout3 << exp3.str(); fout3 << states.str(); fout3 << display; fout3.close();
    std::ofstream fout4(f4.str().c_str()); fout4 << exp4.str(); fout4 << states.str(); fout4 << display; fout4.close();
    std::ofstream fout5(f5.str().c_str()); fout5 << exp5.str(); fout5 << states.str(); fout5 << display; fout5.close();
    std::ofstream fout6(f6.str().c_str()); fout6 << exp6.str(); fout6 << states.str(); fout6 << display; fout6.close();

    std::stringstream batName; batName << folder.str() << "\\runall_" << numUnits << "_" << stat << ".bat";
    std::ofstream bat(batName.str().c_str());
    bat << "@echo off\n";
    bat << "cd C:\\Users\\Dave\\Desktop\\sc2010\\StarCraft_Bot\\Visual_Studio_Projects\\SparCraft\\VisualStudio\\Release\n";
    bat << "copy SparCraft.exe exp_" << numUnits << "_" << stat << ".exe\n";
    bat << "TIMEOUT 3\n";
    bat << "start exp_" << numUnits << "_" << stat << ".exe c:\\users\\dave\\desktop\\results\\" << f1.str() << "\n";
    bat << "start exp_" << numUnits << "_" << stat << ".exe c:\\users\\dave\\desktop\\results\\" << f2.str() << "\n";
    bat << "start exp_" << numUnits << "_" << stat << ".exe c:\\users\\dave\\desktop\\results\\" << f3.str() << "\n";
    bat << "start exp_" << numUnits << "_" << stat << ".exe c:\\users\\dave\\desktop\\results\\" << f4.str() << "\n";
    bat.close();

    std::stringstream batName2; batName2 << folder.str() << "\\runall_p_" << numUnits << "_" << stat << ".bat";
    std::ofstream bat2(batName2.str().c_str());
    bat2 << "@echo off\n";
    bat2 << "cd C:\\Users\\Dave\\Desktop\\sc2010\\StarCraft_Bot\\Visual_Studio_Projects\\SparCraft\\VisualStudio\\Release\n";
    bat2 << "copy SparCraft.exe exp_p_" << numUnits << "_" << stat << ".exe\n";
    bat2 << "TIMEOUT 3\n";
    bat2 << "start exp_p_" << numUnits << "_" << stat << ".exe c:\\users\\dave\\desktop\\results\\" << f5.str() << "\n";
    bat2 << "start exp_p_" << numUnits << "_" << stat << ".exe c:\\users\\dave\\desktop\\results\\" << f6.str() << "\n";
    bat.close();
}

void generateStateConfig(int numUnits, bool sep)
{
    std::stringstream states;

    if (sep)
    {
        states << "State SeparatedState 100 128 128 400 360 840 360 Protoss_Dragoon "   << numUnits << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Zerg_Zergling "     << numUnits << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Protoss_Zealot "     << numUnits << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Terran_Marine "     << numUnits << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Terran_Vulture "     << numUnits << "\n";
   }
    else
    {
        states << "State StateSymmetric 100 128 128 Protoss_Dragoon "   << numUnits << "\n";
        states << "State StateSymmetric 100 128 128 Zerg_Zergling "     << numUnits << "\n";
        states << "State StateSymmetric 100 128 128 Protoss_Zealot "     << numUnits << "\n";
        states << "State StateSymmetric 100 128 128 Terran_Marine "     << numUnits << "\n";
        states << "State StateSymmetric 100 128 128 Terran_Vulture "     << numUnits << "\n";
    }

    std::cout << states.str();
}

// you shouldn't run this, I used it for my paper
void generateExperimentFileLinux(int searchTimer, int maxChildren, int numStates, int numUnits, bool sep)
{
    std::string display("Display false .");
        
    std::string stat = (sep ? "sep" : "sym");
    std::stringstream folder; folder << "exp_linux_" << numUnits << "_" << stat;
    std::stringstream command; command << "mkdir exp\\" << folder.str();

    // Creates the directory
    system("mkdir exp");
    system(command.str().c_str());

    std::stringstream states;
    if (sep)
    {
        states << "State SeparatedState 100 128 128 400 360 840 360 Protoss_Dragoon "   << numUnits << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Zerg_Zergling "     << numUnits << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Protoss_Dragoon "   << (numUnits/2) << " Protoss_Zealot " << (numUnits/2) << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Protoss_Dragoon "   << (numUnits/2) << " Terran_Marine " << (numUnits/2) << "\n";
        states << "State SeparatedState 100 128 128 400 360 840 360 Terran_Marine "     << (numUnits/2) << " Zerg_Zergling " << (numUnits/2) << "\n";
    }
    else
    {
        states << "State StateSymmetric 100 128 128 Protoss_Dragoon "   << numUnits << "\n";
        states << "State StateSymmetric 100 128 128 Zerg_Zergling "     << numUnits << "\n";
        states << "State StateSymmetric 100 128 128 Protoss_Dragoon "   << (numUnits/2) << " Protoss_Zealot " << (numUnits/2) << "\n";
        states << "State StateSymmetric 100 128 128 Protoss_Dragoon "   << (numUnits/2) << " Terran_Marine " << (numUnits/2) << "\n";
        states << "State StateSymmetric 100 128 128 Terran_Marine "     << (numUnits/2) << " Zerg_Zergling " << (numUnits/2) << "\n";
    }

    std::stringstream exp1;
    exp1 << "Player 0 AlphaBeta " << searchTimer << " " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp1 << "Player 0 AlphaBeta " << searchTimer << " " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp1 << "Player 1 PortfolioGreedySearch NOKDPS 1 0\n";
    exp1 << "ResultsFile /home/cdavid/SparCraft/linux/exp/" << folder.str() << "/results_" << numUnits << "_ab_vs_portfolio_" << stat << " true\n";

    std::stringstream exp2;
    exp2 << "Player 0 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp2 << "Player 0 UCT " << searchTimer << " 1.5 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp2 << "Player 1 PortfolioGreedySearch NOKDPS 1 0\n";
    exp2 << "ResultsFile /home/cdavid/SparCraft/linux/exp/" << folder.str() << "/results_" << numUnits << "_uct_vs_portfolio_" << stat << " true\n";

    std::stringstream exp3;
    exp3 << "Player 0 AlphaBeta " << searchTimer << " " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp3 << "Player 1 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp3 << "Player 1 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp3 << "ResultsFile /home/cdavid/SparCraft/linux/exp/" << folder.str() << "/results_" << numUnits << "_ab_vs_uct_nok_" << stat << " true\n";

    std::stringstream exp4;
    exp4 << "Player 0 AlphaBeta " << searchTimer << " " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp4 << "Player 1 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate None\n";
    exp4 << "Player 1 UCT " << searchTimer << " 1.6 5000 " << maxChildren << " ScriptFirst Playout NOKDPS NOKDPS Alternate NOKDPS\n";
    exp4 << "ResultsFile /home/cdavid/SparCraft/linux/exp/" << folder.str() << "/results_" << numUnits << "_ab_vs_uct_none_" << stat << " true\n";

    std::stringstream f1; f1 << "exp/" << folder.str() << "/exp_linux_" << numUnits << "_ab_vs_portfolio_"     << stat << ".txt";
    std::stringstream f2; f2 << "exp/" << folder.str() << "/exp_linux_" << numUnits << "_uct_vs_portfolio_"    << stat << ".txt";
    std::stringstream f3; f3 << "exp/" << folder.str() << "/exp_linux_" << numUnits << "_ab_vs_uct_nok_"       << stat << ".txt";
    std::stringstream f4; f4 << "exp/" << folder.str() << "/exp_linux_" << numUnits << "_ab_vs_uct_none_"      << stat << ".txt";

    std::ofstream fout1(f1.str().c_str()); fout1 << exp1.str(); fout1 << states.str(); fout1 << display; fout1.close();
    std::ofstream fout2(f2.str().c_str()); fout2 << exp2.str(); fout2 << states.str(); fout2 << display; fout2.close();
    std::ofstream fout3(f3.str().c_str()); fout3 << exp3.str(); fout3 << states.str(); fout3 << display; fout3.close();
    std::ofstream fout4(f4.str().c_str()); fout4 << exp4.str(); fout4 << states.str(); fout4 << display; fout4.close();

    std::stringstream batName; batName << "exp\\" << folder.str() << "\\ssh_linuxbox_runall_" << numUnits << "_" << stat << ".bat";
    std::ofstream bat(batName.str().c_str());
    bat << "start cmd /K plink cdavid@linuxbox /home/cdavid/SparCraft/linux/SparCraft /home/cdavid/SparCraft/linux/" << f1.str() << "\n";
    bat << "start cmd /K plink cdavid@linuxbox /home/cdavid/SparCraft/linux/SparCraft /home/cdavid/SparCraft/linux/" << f2.str() << "\n";
    bat << "start cmd /K plink cdavid@linuxbox /home/cdavid/SparCraft/linux/SparCraft /home/cdavid/SparCraft/linux/" << f3.str() << "\n";
    bat << "start cmd /K plink cdavid@linuxbox /home/cdavid/SparCraft/linux/SparCraft /home/cdavid/SparCraft/linux/" << f4.str() << "\n";
    bat.close();
}

void generate()
{
    generateExperimentFileLinux(40, 20, 100, 8, true);
    generateExperimentFileLinux(40, 20, 100, 16, true);
    generateExperimentFileLinux(40, 20, 100, 32, true);
    generateExperimentFileLinux(40, 20, 100, 50, true);
    generateExperimentFileLinux(40, 20, 100, 8, false);
    generateExperimentFileLinux(40, 20, 100, 16, false);
    generateExperimentFileLinux(40, 20, 100, 32, false);
    generateExperimentFileLinux(40, 20, 100, 50, false);
    generateExperimentFile(40, 20, 100, 8, true);
    generateExperimentFile(40, 20, 100, 16, true);
    generateExperimentFile(40, 20, 100, 32, true);
    generateExperimentFile(40, 20, 100, 50, true);
    generateExperimentFile(40, 20, 100, 8, false);
    generateExperimentFile(40, 20, 100, 16, false);
    generateExperimentFile(40, 20, 100, 32, false);
    generateExperimentFile(40, 20, 100, 50, false);

    //for (int i=1; i<51; ++i)
    //{
    //    generateStateConfig(i, false);
    //}
}