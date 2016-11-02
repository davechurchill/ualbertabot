#include "../SparCraft.h"
#include "../torch/TorchTools.h"
#include "../cimg/CImg.h"

using namespace SparCraft;
using namespace cimg_library;

#undef min
#undef max

using namespace SparCraft;

const unsigned char White[3] = {255, 255, 255};
const unsigned char GridColor[3] = {25, 25, 25};
const unsigned char PlayerColors[2][3] = {{0, 255, 0}, {255, 0, 0}};

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

    //std::cout << playerID << " "  << unit.id << " " << type.getName();

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

    if (unit.orders.size() > 0)
    {
        const replayer::Order & order = unit.orders.back();
        BWAPI::Order bwapiOrder(order.type);

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

        DrawLine(img, pos, Position(order.targetX * 8, order.targetY * 8), 1, PlayerColors[playerID]);
    }
}

void DrawTorchFrame(CImg<unsigned char> & img, int width, int height, const replayer::Frame & frame)
{
    const auto & allUnits = frame.units;

    for (int x(32); x < width; x += 32)
    {
        DrawLine(img, Position(x, 0), Position(x, height), 1, GridColor);
    }

    for (int y(32); y < height; y += 32)
    {
        DrawLine(img, Position(0, y), Position(width, y), 1, GridColor);
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
    int mapWidth = 2000, mapHeight = 2000;   
    int mapTLX = 0, mapTLY = 0;         
    int mapBRX = mapWidth, mapBRY = mapHeight; 
    int finalScalingFactor = 2;
    int padding = 100;

    // Check to see if the file exists
    std::ifstream fin(file);
    if (!fin.good())
    {
        std::cerr << "Error: Could not read file: " << file << "\n";
        exit(-1);
    }

    // Read in all of the Frame data from the file
    std::vector<replayer::Frame> frames;
    while (fin.good())
    {
        replayer::Frame frame;
	    fin >> frame;
        frames.push_back(frame);
    }

    // Check to see if any valid frames were found
    if (frames.empty())
    {
        std::cerr << "Error: No valid frames found in file\n";
        exit(-1);
    }

    // Set up the image dimensions from args if there are enough
    if (argc == 9)
    {
        mapWidth = atoi(argv[2]);
        mapHeight = atoi(argv[3]);
        mapTLX = atoi(argv[4]);
        mapTLY = atoi(argv[5]);
        mapBRX = atoi(argv[6]);
        mapBRY = atoi(argv[7]);
        finalScalingFactor = atoi(argv[8]);
    }
    // Otherwise, set up default image dimensions
    else
    {
        if (argc == 3)
        {
            finalScalingFactor = atoi(argv[2]);
        }

        int minX = std::numeric_limits<int>::max();
        int minY = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int maxY = std::numeric_limits<int>::min();

        // Determine the dimensions of the image from the frame data
        for (size_t f(0); f < frames.size(); ++f)
        {
            const auto & allUnits = frames[f].units;
            for (const auto & kv : allUnits)
	        {
		        const auto & playerUnits = kv.second;
		        for (const auto & unit : playerUnits)
		        {
                    BWAPI::UnitType type(unit.type);

                    minX = std::min(minX, unit.x*8 - type.dimensionLeft());
                    minY = std::min(minY, unit.y*8 - type.dimensionUp());
                    maxX = std::max(maxX, unit.x*8 + type.dimensionRight());
                    maxY = std::max(maxY, unit.y*8 + type.dimensionDown());
		        }
	        }
        }

        mapWidth = maxX + padding;
        mapHeight = maxY + padding;
        mapTLX = std::max(0, minX - padding);
        mapTLY = std::max(0, minY - padding);
        mapBRX = mapWidth;
        mapBRY = mapHeight;
    }

    // Draw the images from the frame data
    for (size_t f(0); f < frames.size(); ++f)
    {
        CImg<unsigned char> canvas(mapWidth, mapHeight, 1, 4);
        canvas.fill(0);

        DrawTorchFrame(canvas, mapWidth, mapHeight, frames[f]);
        canvas.crop(mapTLX, mapTLY, mapBRX, mapBRY);
        canvas.resize(canvas.width() * finalScalingFactor, canvas.height() * finalScalingFactor, -1);

        std::stringstream name;
        name << file << "_frame_" << f << ".png";
        canvas.save(name.str().c_str());
    }  

    return 0;
}