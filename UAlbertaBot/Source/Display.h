#ifndef DISPLAY_H
#define DISPLAY_H

struct float3
{
	float x,y,z;

	float3() {}
	float3(float f) : x(f), y(f), z(f) {}
	float3(float x, float y, float z) : x(x), y(y), z(z) {}

	operator const float * () const { return &x; }
	float3 operator + (const float3 & v) const { return float3(x+v.x,y+v.y,z+v.z); }
	float3 operator - (const float3 & v) const { return float3(x-v.x,y-v.y,z-v.z); }
	float3 operator * (const float3 & v) const { return float3(x*v.x,y*v.y,z*v.z); }
	float3 operator / (const float3 & v) const { return float3(x/v.x,y/v.y,z/v.z); }

	operator float * () { return &x; }
	const float3 & operator += (const float3 & v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
	const float3 & operator -= (const float3 & v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
	const float3 & operator *= (const float3 & v) { x*=v.x; y*=v.y; z*=v.z; return *this; }
	const float3 & operator /= (const float3 & v) { x/=v.x; y/=v.y; z/=v.z; return *this; }
};

class Display
{
	int	windowSizeX;
	int windowSizeY;

	int cameraX;
	int cameraY;

	int mapWidth;
	int mapHeight;

	int mapPixelWidth;
	int mapPixelHeight;

	bool bl,br,bd,bu;

	void HandleEvents();
	void RenderMainMap();
	void RenderMinimap();

	void RenderTerrain(int wx0, int wy0, int wx1, int wy1) const;
	void RenderTerrainAnalysis() const;
	void RenderUnits() const;

	float3 GetWalkTileColor(int x, int y) const;
public:
	Display();
	~Display();

	void OnStart();
	void OnFrame();
};

#endif