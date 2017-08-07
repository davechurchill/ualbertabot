#pragma once

#include "WorkerManager.h"
#include "ScreenCanvas.h"
#include "DebugInfoProvider.h"

namespace AKBot
{
	using UAlbertaBot::WorkerData;

	class WorkerManagerDebug : public DebugInfoProvider
	{
		shared_ptr<WorkerData> _workerData;

		void drawResourceDebugInfo(AKBot::ScreenCanvas& canvas);
		void drawWorkerInformation(AKBot::ScreenCanvas& canvas, int x, int y);
		void drawDepotDebugInfo(AKBot::ScreenCanvas& canvas) const;
	public:
		WorkerManagerDebug(shared_ptr<WorkerData> workerData);
		void draw(ScreenCanvas& canvas) override;
	};
}
