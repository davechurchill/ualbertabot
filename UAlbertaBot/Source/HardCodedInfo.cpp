/*#include "Common.h"
#include "BWAPI.h"
#include "HardCodedInfo.h"

// gotta keep c++ static happy
HardCodedInfo * HardCodedInfo::Instance() = NULL;

// constructor
HardCodedInfo::HardCodedInfo() 
{
	setChokeDefendLocations();
}

// get an instance of this
HardCodedInfo * HardCodedInfo::Instance() 
{
	// if the instance doesn't exist, create it
	if (!HardCodedInfo::Instance()) 
	{
		HardCodedInfo::Instance() = new HardCodedInfo();
	}

	return HardCodedInfo::Instance();
}

void HardCodedInfo::setChokeDefendLocations()
{
	// (2)Benzene.scx ---- af618ea3ed8a8926ca7b17619eebcb9126f0d8b1
	mainChokes["af618ea3ed8a8926ca7b17619eebcb9126f0d8b1"] = std::vector<BWAPI::Position>();
	mainChokes["af618ea3ed8a8926ca7b17619eebcb9126f0d8b1"].push_back(BWAPI::Position(3334, 953));
	mainChokes["af618ea3ed8a8926ca7b17619eebcb9126f0d8b1"].push_back(BWAPI::Position(741, 2614));

	naturalChokes["af618ea3ed8a8926ca7b17619eebcb9126f0d8b1"] = std::vector<BWAPI::Position>();
	naturalChokes["af618ea3ed8a8926ca7b17619eebcb9126f0d8b1"].push_back(BWAPI::Position(3683, 1530));
	naturalChokes["af618ea3ed8a8926ca7b17619eebcb9126f0d8b1"].push_back(BWAPI::Position(368, 1994));

	// (2)Destination.scx ---- 4e24f217d2fe4dbfa6799bc57f74d8dc939d425b
	mainChokes["4e24f217d2fe4dbfa6799bc57f74d8dc939d425b"] = std::vector<BWAPI::Position>();
	mainChokes["4e24f217d2fe4dbfa6799bc57f74d8dc939d425b"].push_back(BWAPI::Position(1417, 3752));
	mainChokes["4e24f217d2fe4dbfa6799bc57f74d8dc939d425b"].push_back(BWAPI::Position(1610, 311));

	naturalChokes["4e24f217d2fe4dbfa6799bc57f74d8dc939d425b"] = std::vector<BWAPI::Position>();
	naturalChokes["4e24f217d2fe4dbfa6799bc57f74d8dc939d425b"].push_back(BWAPI::Position(1049, 3274));
	naturalChokes["4e24f217d2fe4dbfa6799bc57f74d8dc939d425b"].push_back(BWAPI::Position(2015, 805));

	//mainChokes["4e24f217d2fe4dbfa6799bc57f74d8dc939d425b"].push_back(BWAPI::Position(, ));
	// (2)Heartbreak Ridge.scx ---- 6f8da3c3cc8d08d9cf882700efa049280aedca8c
	mainChokes["6f8da3c3cc8d08d9cf882700efa049280aedca8c"] = std::vector<BWAPI::Position>();
	mainChokes["6f8da3c3cc8d08d9cf882700efa049280aedca8c"].push_back(BWAPI::Position(3635, 1387));
	mainChokes["6f8da3c3cc8d08d9cf882700efa049280aedca8c"].push_back(BWAPI::Position(439, 1752));

	naturalChokes["6f8da3c3cc8d08d9cf882700efa049280aedca8c"] = std::vector<BWAPI::Position>();
	naturalChokes["6f8da3c3cc8d08d9cf882700efa049280aedca8c"].push_back(BWAPI::Position(3336, 859));
	naturalChokes["6f8da3c3cc8d08d9cf882700efa049280aedca8c"].push_back(BWAPI::Position(694, 2234));

	// (3)Aztec.scx ---- ba2fc0ed637e4ec91cc70424335b3c13e131b75a
	mainChokes["ba2fc0ed637e4ec91cc70424335b3c13e131b75a"] = std::vector<BWAPI::Position>();
	mainChokes["ba2fc0ed637e4ec91cc70424335b3c13e131b75a"].push_back(BWAPI::Position(3158, 481));
	mainChokes["ba2fc0ed637e4ec91cc70424335b3c13e131b75a"].push_back(BWAPI::Position(3049, 3589));
	mainChokes["ba2fc0ed637e4ec91cc70424335b3c13e131b75a"].push_back(BWAPI::Position(522, 1865));

	naturalChokes["ba2fc0ed637e4ec91cc70424335b3c13e131b75a"] = std::vector<BWAPI::Position>();
	naturalChokes["ba2fc0ed637e4ec91cc70424335b3c13e131b75a"].push_back(BWAPI::Position(3158, 481));
	naturalChokes["ba2fc0ed637e4ec91cc70424335b3c13e131b75a"].push_back(BWAPI::Position(3049, 3589));
	naturalChokes["ba2fc0ed637e4ec91cc70424335b3c13e131b75a"].push_back(BWAPI::Position(522, 1865));

	// (3)Tau Cross.scx ---- 9bfc271360fa5bab3707a29e1326b84d0ff58911
	mainChokes["9bfc271360fa5bab3707a29e1326b84d0ff58911"] = std::vector<BWAPI::Position>();
	mainChokes["9bfc271360fa5bab3707a29e1326b84d0ff58911"].push_back(BWAPI::Position(274, 894));
	mainChokes["9bfc271360fa5bab3707a29e1326b84d0ff58911"].push_back(BWAPI::Position(3825, 1134));
	mainChokes["9bfc271360fa5bab3707a29e1326b84d0ff58911"].push_back(BWAPI::Position(2219, 3677));

	naturalChokes["9bfc271360fa5bab3707a29e1326b84d0ff58911"] = std::vector<BWAPI::Position>();
	naturalChokes["9bfc271360fa5bab3707a29e1326b84d0ff58911"].push_back(BWAPI::Position(891, 591));
	naturalChokes["9bfc271360fa5bab3707a29e1326b84d0ff58911"].push_back(BWAPI::Position(3406, 1392));
	naturalChokes["9bfc271360fa5bab3707a29e1326b84d0ff58911"].push_back(BWAPI::Position(1675, 3419));

	// (4)Andromeda.scx ---- 1e983eb6bcfa02ef7d75bd572cb59ad3aab49285
	mainChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"] = std::vector<BWAPI::Position>();
	mainChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"].push_back(BWAPI::Position(3716, 1023));
	mainChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"].push_back(BWAPI::Position(346, 1031));
	mainChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"].push_back(BWAPI::Position(338, 3026));
	mainChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"].push_back(BWAPI::Position(3690, 3041));

	naturalChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"] = std::vector<BWAPI::Position>();
	naturalChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"].push_back(BWAPI::Position(3028, 1052));
	naturalChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"].push_back(BWAPI::Position(1004, 1034));
	naturalChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"].push_back(BWAPI::Position(914, 3101));
	naturalChokes["1e983eb6bcfa02ef7d75bd572cb59ad3aab49285"].push_back(BWAPI::Position(3075, 3064));

	// (4)Circuit Breaker.scx ---- 450a792de0e544b51af5de578061cb8a2f020f32
	mainChokes["450a792de0e544b51af5de578061cb8a2f020f32"] = std::vector<BWAPI::Position>();
	mainChokes["450a792de0e544b51af5de578061cb8a2f020f32"].push_back(BWAPI::Position(3948, 797));
	mainChokes["450a792de0e544b51af5de578061cb8a2f020f32"].push_back(BWAPI::Position(143, 800));
	mainChokes["450a792de0e544b51af5de578061cb8a2f020f32"].push_back(BWAPI::Position(171, 3285));
	mainChokes["450a792de0e544b51af5de578061cb8a2f020f32"].push_back(BWAPI::Position(3906, 3289));

	naturalChokes["450a792de0e544b51af5de578061cb8a2f020f32"] = std::vector<BWAPI::Position>();
	naturalChokes["450a792de0e544b51af5de578061cb8a2f020f32"].push_back(BWAPI::Position(3489, 1115));
	naturalChokes["450a792de0e544b51af5de578061cb8a2f020f32"].push_back(BWAPI::Position(589, 1113));
	naturalChokes["450a792de0e544b51af5de578061cb8a2f020f32"].push_back(BWAPI::Position(602, 3005));
	naturalChokes["450a792de0e544b51af5de578061cb8a2f020f32"].push_back(BWAPI::Position(3487, 3008));

	// (4)Empire of the Sun.scm ---- a220d93efdf05a439b83546a579953c63c863ca7
	mainChokes["a220d93efdf05a439b83546a579953c63c863ca7"] = std::vector<BWAPI::Position>();
	mainChokes["a220d93efdf05a439b83546a579953c63c863ca7"].push_back(BWAPI::Position(3325, 3831));
	mainChokes["a220d93efdf05a439b83546a579953c63c863ca7"].push_back(BWAPI::Position(3344, 304));
	mainChokes["a220d93efdf05a439b83546a579953c63c863ca7"].push_back(BWAPI::Position(738, 287));
	mainChokes["a220d93efdf05a439b83546a579953c63c863ca7"].push_back(BWAPI::Position(753, 3815));

	naturalChokes["a220d93efdf05a439b83546a579953c63c863ca7"] = std::vector<BWAPI::Position>();
	naturalChokes["a220d93efdf05a439b83546a579953c63c863ca7"].push_back(BWAPI::Position(2945, 3547));
	naturalChokes["a220d93efdf05a439b83546a579953c63c863ca7"].push_back(BWAPI::Position(2927, 487));
	naturalChokes["a220d93efdf05a439b83546a579953c63c863ca7"].push_back(BWAPI::Position(1128, 522));
	naturalChokes["a220d93efdf05a439b83546a579953c63c863ca7"].push_back(BWAPI::Position(1143, 3507));

	// (4)Fortress.scx ---- 83320e505f35c65324e93510ce2eafbaa71c9aa1
	mainChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"] = std::vector<BWAPI::Position>();
	mainChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"].push_back(BWAPI::Position(1810, 3863));
	mainChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"].push_back(BWAPI::Position(3667, 2181));
	mainChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"].push_back(BWAPI::Position(397, 1904));
	mainChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"].push_back(BWAPI::Position(2188, 249));

	naturalChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"] = std::vector<BWAPI::Position>();
	naturalChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"].push_back(BWAPI::Position(1559, 3612));
	naturalChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"].push_back(BWAPI::Position(3471, 2477));
	naturalChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"].push_back(BWAPI::Position(583, 1628));
	naturalChokes["83320e505f35c65324e93510ce2eafbaa71c9aa1"].push_back(BWAPI::Position(2492, 506));

	// (4)Python.scx ---- de2ada75fbc741cfa261ee467bf6416b10f9e301
	mainChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"] = std::vector<BWAPI::Position>();
	mainChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"].push_back(BWAPI::Position(2064, 114));
	mainChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"].push_back(BWAPI::Position(2001, 3957));
	mainChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"].push_back(BWAPI::Position(3874, 1812));
	mainChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"].push_back(BWAPI::Position(156, 2262));

	naturalChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"] = std::vector<BWAPI::Position>();
	naturalChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"].push_back(BWAPI::Position(1803, 607));
	naturalChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"].push_back(BWAPI::Position(2218, 3553));
	naturalChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"].push_back(BWAPI::Position(3391, 2047));
	naturalChokes["de2ada75fbc741cfa261ee467bf6416b10f9e301"].push_back(BWAPI::Position(618, 2058));
}

BWAPI::Position HardCodedInfo::getChokepoint(ChokeType type, BWAPI::Player * player)
{
	// get and assert we know where this player is
	BWTA::BaseLocation * mainBaseLocation = BWTA::getStartLocation(player);
	assert(mainBaseLocation);

	// get the position of the main base
	BWAPI::Position mainBasePosition = mainBaseLocation->getPosition();

	// find the closest main choke point to this location
	double closestDist = 10000;
	BWAPI::Position closestPos(10000,10000);

	std::vector<BWAPI::Position> & chokes = (type == MAIN_CHOKE) 
		? mainChokes[BWAPI::Broodwar->mapHash()] : naturalChokes[BWAPI::Broodwar->mapHash()];

	BOOST_FOREACH (BWAPI::Position p, chokes)
	{
		double dist = p.getDistance(mainBasePosition);
		if (dist < closestDist)
		{
			closestDist = dist;
			closestPos = p;
		}
	}

	assert(closestPos.isValid());

	return closestPos;
}

void HardCodedInfo::drawChokePoints()
{
	BOOST_FOREACH(BWAPI::Position p, mainChokes[BWAPI::Broodwar->mapHash()])
	{
		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawCircleMap(p.x(), p.y(), 10, BWAPI::Colors::Purple, true);
	}

	BOOST_FOREACH(BWAPI::Position p, naturalChokes[BWAPI::Broodwar->mapHash()])
	{
		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawCircleMap(p.x(), p.y(), 10, BWAPI::Colors::Purple, true);
	}
}*/