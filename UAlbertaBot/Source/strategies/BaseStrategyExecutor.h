#pragma once
#include "StrategyExecutor.h"

namespace AKBot
{
	class BaseStrategyExecutor : public StrategyExecutor
	{
		BuildOrder _buildOrder;
	public:
		void loadOpeningBuildOrder(const BuildOrder& buildOrder) final;
		virtual const BuildOrder& getOpeningBuildOrder() const final;
	};
}