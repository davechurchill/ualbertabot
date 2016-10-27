#include "../SparCraft.h"
#include "../torch/TorchTools.h"
#include "../cimg/CImg.h"

using namespace SparCraft;
using namespace cimg_library;

#undef min
#undef max

using namespace SparCraft;
unsigned char White[3] = {255, 255, 255};
unsigned char Grid[3] = {25, 25, 25};
unsigned char PlayerColors[2][3] = {{0, 255, 0}, {255, 0, 0}};

int scale = 2;

void DrawLine(CImg<unsigned char> & img, const Position & p1, const Position & p2, const float thickness, const unsigned char *const color)
{
    img.draw_line(p1.x(), p1.y(), p2.x(), p2.y(), color);
}

void DrawRectangle(CImg<unsigned char> & img, const Position & p1, const Position & p2, const unsigned char *const color)
{
    DrawLine(img, p1, Position(p2.x(), p1.y()), 1, color);
    DrawLine(img, p2, Position(p2.x(), p1.y()), 1, color);
    DrawLine(img, p2, Position(p1.x(), p2.y()), 1, color);
    DrawLine(img, p1, Position(p1.x(), p2.y()), 1, color);
}

void DrawTorchUnit(CImg<unsigned char> & img, const replayer::Frame & frame, const replayer::Unit & unit, const int playerID)
{
    const BWAPI::UnitType type(unit.type);
    Position pos(unit.x * 8, unit.y * 8);

    std::cout << playerID << " "  << unit.id << " " << type.getName();

    Position tl(pos.x() - type.dimensionLeft(),  pos.y() - type.dimensionUp());
    Position br(pos.x() + type.dimensionRight(), pos.y() + type.dimensionDown());

    std::stringstream ss;
    for (size_t i(0); i < type.getName().size(); ++i)
    {
        if (type.getName()[i] == '_')
        {
            ss << type.getName()[i+1];
            break;
        }
    }

    ss << unit.id;

    DrawRectangle(img, tl, br, PlayerColors[playerID]);
    img.draw_text(tl.x(), tl.y()-12, ss.str().c_str(), White);
    img.draw_text(tl.x()+2, tl.y(), "%d", White, 0, 1, 8, unit.health);

    std::cout << " [";
    for (const replayer::Order & order : unit.orders)
    {
        std::cout << order.targetId << " ";

        BWAPI::Order bwapiOrder(order.type);
        std::cout << bwapiOrder.getName() << " ";

        if (bwapiOrder == BWAPI::Orders::AttackUnit)
        {
            const auto & allUnits = frame.units;
            for (const auto & kv : allUnits)
	        {
		        const auto & playerID = kv.first;
		        const auto & playerUnits = kv.second;

		        for (const auto & target : playerUnits)
		        {
                    if (target.id == order.targetId)
                    {
                        DrawLine(img, pos, Position(target.x * 8, target.y * 8), 1, PlayerColors[playerID]);
                    }
		        }
	        }
        }

        DrawLine(img, pos, Position(order.targetX * 8, order.targetY * 8), 1, White);
    }
    std::cout << "]\n";
}

void DrawTorchFrame(CImg<unsigned char> & img, int width, int height, const replayer::Frame & frame)
{
    const auto & allUnits = frame.units;

    for (int x(32); x < width; x += 32)
    {
        DrawLine(img, Position(x, 0), Position(x, height), 1, Grid);
    }

    for (int y(32); y < height; y += 32)
    {
        DrawLine(img, Position(0, y), Position(width, y), 1, Grid);
    }

	for (const auto & kv : allUnits)
	{
		const auto & playerID = kv.first;
		const auto & playerUnits = kv.second;

		for (const auto & unit : playerUnits)
		{
            DrawTorchUnit(img, frame, unit, playerID);
		}
	}
}

int main(int argc, char ** argv)
{
    std::string file = "frames.txt";  file = argv[1];
    int mapWidth = 2000;    mapWidth = atoi(argv[2]);
    int mapHeight = 2000;   mapHeight = atoi(argv[3]);
    int mapTLX = 0;         mapTLX = atoi(argv[4]);
    int mapTLY = 0;         mapTLY = atoi(argv[5]);
    int mapBRX = mapWidth;  mapBRX = atoi(argv[6]);
    int mapBRY = mapHeight; mapBRY = atoi(argv[7]);
    int scale = 1;          scale = atoi(argv[8]);

    std::ifstream fin(file);

    int lineNum = 0;
    while (fin.good())
    {
        replayer::Frame frame;
	    fin >> frame;

        CImg<unsigned char> canvas(mapWidth, mapHeight, 1, 4);
        canvas.fill(0);

        DrawTorchFrame(canvas, mapWidth, mapHeight, frame);
        canvas.crop(mapTLX, mapTLY, mapBRX, mapBRY);
        canvas.resize(canvas.width() * scale, canvas.height() * scale, -1);

        std::stringstream name;
        name << "test_" << lineNum << ".png";
        canvas.save(name.str().c_str());

        lineNum++;
    }

    return 0;
}