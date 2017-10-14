#include "DebuGtools.h"

using namespace UAlbertaBot;
using namespace AKBot;

void DebugTools::DrawUnitHPBar(ScreenCanvas& canvas, const BWAPI::UnitType & type, const BWAPI::Position & pos, const int & hitPoints, const int & shields)
{
    int verticalOffset = -10;
    int left = pos.x - type.dimensionLeft();
    int right = pos.x + type.dimensionRight();
    int top = pos.y - type.dimensionUp();
    int bottom = pos.y + type.dimensionDown();
    
    if (type.maxHitPoints() > 0)
    {
        double hpRatio = (double)hitPoints / (double)type.maxHitPoints();

        BWAPI::Color hpColor = BWAPI::Colors::Green;
		if (hpRatio < 0.66) {
			hpColor = BWAPI::Colors::Orange;
		}

		if (hpRatio < 0.33) {
			hpColor = BWAPI::Colors::Red;
		}

        int ratioRight = left + (int)((right - left) * hpRatio);
        int hpTop = top + verticalOffset;
        int hpBottom = top + 4 + verticalOffset;

        canvas.drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
        canvas.drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), hpColor, true);
        canvas.drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

        int ticWidth = 3;

        for (int i(left); i < right - 1; i += ticWidth)
        {
			canvas.drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
        }
    }

    if (type.maxShields() > 0)
    {
        double shieldRatio = (double)shields / (double)type.maxShields();

        int ratioRight = left + (int)((right - left) * shieldRatio);
        int hpTop = top - 3 + verticalOffset;
        int hpBottom = top + 1 + verticalOffset;

        canvas.drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
        canvas.drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), BWAPI::Colors::Blue, true);
        canvas.drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

        int ticWidth = 3;

        for (int i(left); i < right - 1; i += ticWidth)
        {
			canvas.drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
        }
    }
}