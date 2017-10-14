#include "stdafx.h"
#include "CppUnitTest.h"
#include "TestLib\UnitImpl.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

template <> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<BWAPI::UnitData>(const BWAPI::UnitData& q)
{
	std::wstringstream _s;
	_s << "Unit#" << q.id;
	return _s.str();
}

template <> static std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<BWAPI::UnitType>(const BWAPI::UnitType& q)
{
	std::wstringstream _s;
	_s << "Unit Type: " << q.getName().c_str();
	return _s.str();
}

namespace AkBotTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			auto t = BWAPI::UnitTypes::Hero_Magellan;
			BWAPI::UnitData data;
			data.type = t;
			data.positionX = 45;
			data.positionY = 123;
			AKBot::UnitImpl unit(1, data);
			auto position = unit.getPosition();
			Assert::AreEqual(45, position.x);
			Assert::AreEqual(123, position.y);
			auto unitType = unit.getType();
			Assert::AreEqual(BWAPI::UnitTypes::Hero_Magellan, unitType);
		}

	};
}