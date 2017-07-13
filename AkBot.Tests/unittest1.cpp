#include "stdafx.h"
#include "CppUnitTest.h"
#include "Micro.h"
#include "UnitImpl.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AkBotTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			auto t = BWAPI::UnitTypes::Hero_Magellan;
			BWAPI::UnitData data;
			AKBot::UnitImpl unit(1, data);
			// TODO: Your test code here
		}

	};
}