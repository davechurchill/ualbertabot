#pragma once

#include "..\WorkerManager.h"
#include "..\ScreenCanvas.h"
#include "DebugInfoProvider.h"
#include "..\BotConfiguration.h"

namespace AKBot
{
	using UAlbertaBot::WorkerData;

	class WorkerManagerDebug : public DebugInfoProvider
	{
		shared_ptr<WorkerData> _workerData;
		const BotDebugConfiguration& _debugConfiguration;

		void drawResourceDebugInfo(AKBot::ScreenCanvas& canvas);
		void drawWorkerInformation(AKBot::ScreenCanvas& canvas, int x, int y);
		void drawDepotDebugInfo(AKBot::ScreenCanvas& canvas) const;
	public:
		WorkerManagerDebug(
			shared_ptr<WorkerData> workerData,
			const BotDebugConfiguration& debugConfiguration);
		WorkerManagerDebug(const WorkerManagerDebug&) = delete;
		void draw(ScreenCanvas& canvas) override;
	};
}
