#pragma once

#include "Common.h"

namespace SparCraft
{

template <class T>
class EnumData
{
protected:

    static std::string name;
    static std::vector<std::string> names;
    static std::map<std::string, int> nameMap;

public:

    static const int size()
    {
        return T::Size;
    }

    static void setType(const std::string & s)
    {
        name = s;
    }

    static const std::string & getType()
    {
        return name;
    }

    static void setData(const int & ID, const std::string & s)
    {
        if (ID > size())
        {
            std::stringstream ss;
            ss << ID;
            System::FatalError("Unknown " + getType() + " ID: " + ss.str());
        }

        names[ID] = s;
        nameMap[s] = ID;
    }

    static const IDType getID(const std::string & s)
    {
        const std::map<std::string, int>::const_iterator it(nameMap.find(s));

        if (it == nameMap.end())
        {
            System::FatalError("Unknown " + getType() + " String: " + s);
        }
        
        return it->second;
    }

    static const std::string & getName(const IDType & ID)
    {
        if (ID > size())
        {
            std::stringstream ss;
            ss << ID;
            System::FatalError("Unknown " + getType() + " ID: " + ss.str());
        }

        return names[ID];
    }
};

template <class T> std::string EnumData<T>::name;
template <class T> std::vector<std::string> EnumData<T>::names;
template <class T> std::map<std::string, int> EnumData<T>::nameMap;

class Players : public EnumData<Players>
{
public:
    enum { Player_One = 0, Player_Two = 1, Player_None = 2, Player_Both = 3, Size};
    static void init()
    {
        setType("Players");
        names.resize(Size);
        setData(Player_One,     "Player One");
		setData(Player_Two,     "Player Two");
		setData(Player_None,    "Player None");
		setData(Player_Both,    "Player Both");
    }
};

class SearchMethods : public EnumData<SearchMethods>
{
public:
    enum { AlphaBeta, IDAlphaBeta, MiniMax, Size };
    static void init()
    {
        setType("SearchMethod");
        names.resize(Size);
        setData(AlphaBeta,      "AlphaBeta");
        setData(IDAlphaBeta,    "IDAlphaBeta");
        setData(MiniMax,        "MiniMax");
    }
};

class PlayerModels : public EnumData<PlayerModels>
{
public:
    enum { AlphaBeta, AttackClosest, Kiter, Random, AttackWeakest, AttackDPS, KiterDPS, NOKDPS, Kiter_NOKDPS, Cluster, PortfolioGreedySearch, UCT, None, Size };
    static void init()
    {
        setType("PlayerModels");
        names.resize(Size);
        setData(AlphaBeta,              "AlphaBeta");
		setData(AttackClosest,          "AttackClosest");
		setData(Kiter,                  "Kiter");
		setData(Random,                 "Random");
		setData(AttackWeakest,          "AttackWeakest");
		setData(AttackDPS,              "AttackDPS");
		setData(KiterDPS,               "KiterDPS");
		setData(NOKDPS,                 "NOKDPS");
        setData(Kiter_NOKDPS,           "Kiter_NOKDPS");
        setData(Cluster,                "Cluster");
        setData(PortfolioGreedySearch,  "PortfolioGreedySearch");
        setData(UCT,                    "UCT");
		setData(None,                   "None");
    }
};

class EvaluationMethods : public EnumData<EvaluationMethods>
{
public:
    enum { LTD, LTD2, Playout, Size };
    static void init()
    {
        setType("EvaluationMethods");
        names.resize(Size);
        setData(LTD,        "LTD");
        setData(LTD2,       "LTD2");
        setData(Playout,    "Playout");
    }
};

class SearchNodeType : public EnumData<SearchNodeType>
{
public:
    enum { Default, RootNode, SoloNode, FirstSimNode, SecondSimNode, Size };
    static void init()
    {
        setType("SearchNodeType");
        names.resize(Size);
        setData(Default,        "Default");
        setData(RootNode,       "RootNode");
        setData(SoloNode,       "SoloNode");
        setData(FirstSimNode,   "FirstSimNode");
        setData(SecondSimNode,  "SecondSimNode");
    }
};

class MoveOrderMethod : public EnumData<MoveOrderMethod>
{
public:
    enum { ScriptFirst, None, Size };
    static void init()
    {
        setType("MoveOrderMethod");
        names.resize(Size);
        setData(ScriptFirst,    "ScriptFirst");
        setData(None,           "None");
    }
};

class PlayerToMove : public EnumData<PlayerToMove>
{
public:
    enum { Random, Alternate, Not_Alternate, Size };
    static void init()
    {
        setType("PlayerToMove");
        names.resize(Size);
        setData(Random,         "Random");
        setData(Alternate,      "Alternate");
        setData(Not_Alternate,  "Not_Alternate");
    }
};

extern void EnumDataInit();

}