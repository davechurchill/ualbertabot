#pragma once

#include "WorkerManager.h"
#include "ScreenCanvas.h"

namespace AKBot
{
	using UAlbertaBot::WorkerData;

	class WorkerManagerDebug
	{
		const WorkerData& _workerData;

		void drawResourceDebugInfo(AKBot::ScreenCanvas& canvas);
		void drawWorkerInformation(AKBot::ScreenCanvas& canvas, int x, int y);
		void drawDepotDebugInfo(AKBot::ScreenCanvas& canvas) const;
	public:
		WorkerManagerDebug(const WorkerData& workerData);
		void draw(ScreenCanvas& canvas);
	};
}
