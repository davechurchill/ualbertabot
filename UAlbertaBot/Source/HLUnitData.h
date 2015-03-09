#pragma once
#include "UnitData.h"

namespace UAlbertaBot{
	class HLUnitData : public UnitData
	{
		std::unordered_map<int, UnitInfo>		_fakeUnitMap;
		int							_highestID;
	public:
		HLUnitData();
		HLUnitData(const UnitData &data);
		virtual ~HLUnitData();

		//virtual methods
		void	getCloakedUnits(std::set<UnitInfo> & v)				const override;
		int		numCloakedUnits()									const override;
		void	getDetectorUnits(std::set<UnitInfo> & v)			const override;
		void	getFlyingUnits(std::set<UnitInfo> & v)				const override;
		bool	hasCloakedUnits()									const override;
		bool	hasDetectorUnits()									const override;
		//int		getNumUnits(BWAPI::UnitType t)						const override;
		//int		getNumCompletedUnits(BWAPI::UnitType t)				const override;
		//int		getNumDeadUnits(BWAPI::UnitType t)					const override;

		//new methods
		void	removeUnit(int id);
		void	addUnit(const UnitInfo &unit);
		std::vector<UnitInfo> getUnitVector()						const;
		int		highestID()												const {return _highestID;};
	};
}
