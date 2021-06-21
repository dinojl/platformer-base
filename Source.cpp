#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

private:

	std::string sLevel;
	int nLevelWidth;
	int nLevelHeight;

	bool bPlayerOnGround = false;
	bool bPlayerDir = true;

	float fCameraPosX = 0.0f;
	float fCameraPosY = 0.0f;

	float fPlayerPosX = 0.0f;
	float fPlayerPosY = 0.0f;

	float fPlayerVelX = 0.0f;
	float fPlayerVelY = 0.0f;

	int coins = 0;

	olc::vi2d TileSize = { 16, 16 };
	int nTileWidth = TileSize.x;
	int nTileHeight = TileSize.y;

	olc::Sprite* TileSheet = nullptr;
	olc::Sprite* PlayerSprite = nullptr;

public:

	bool OnUserCreate() override
	{
		nLevelWidth = 64;
		nLevelHeight = 16;

		sLevel += "................................................................";
		sLevel += "................................................................";
		sLevel += "................................................................";
		sLevel += "................................................................";
		sLevel += "................................................................";
		sLevel += "................................................................";
		sLevel += "....................................B?B.........................";
		sLevel += "................................................................";
		sLevel += ".....................o..........................................";
		sLevel += "............#######.........##............BB....................";
		sLevel += "...........###..............##...........BBBB...................";
		sLevel += "..........#####.........................BBBBBB..................";
		sLevel += "######################..###########.############################";
		sLevel += "......................#.###########.#...........................";
		sLevel += "......................#.............#...........................";
		sLevel += "......................#######..######...........................";

		TileSheet = new olc::Sprite("gfx/SpriteSheet.png");
		PlayerSprite = new olc::Sprite("gfx/PlayerSprite.png");

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Utility lambdas
		auto GetTile = [&](int x, int y) {
			if (x >= 0 && x < nLevelWidth && y >= 0 && y < nLevelHeight)
				return sLevel[y * nLevelWidth + x];
			else
				return ' ';
		};
		auto SetTile = [&](int x, int y, char c) {
			if (x >= 0 && x < nLevelWidth && y >= 0 && y < nLevelHeight)
				sLevel[y * nLevelWidth + x] = c;
		};

		// Input handling

		bool bPlayerMoving = false;
		if (IsFocused()) {
			if (GetKey(olc::LEFT).bHeld) {
				fPlayerVelX += (bPlayerOnGround ? -10.0f : -4.0f) * fElapsedTime;
				bPlayerMoving = true;
			}
			if (GetKey(olc::RIGHT).bHeld) {
				fPlayerVelX += (bPlayerOnGround ? 10.0f : 4.0f) * fElapsedTime;
				bPlayerMoving = true;
			}
			if (GetKey(olc::UP).bPressed) {
				if (fPlayerVelY == 0)
					fPlayerVelY = -15;
			}
			if (GetKey(olc::F3).bPressed && GetKey(olc::CTRL).bHeld) { // dumbass cheat code TODO: remove before release
				coins = 69420;
			}
		}

		fPlayerVelY += 20.0f * fElapsedTime;

		if (bPlayerOnGround) {
			fPlayerVelX += -3.0f * fPlayerVelX * fElapsedTime;
			if (!bPlayerMoving && (fabs(fPlayerVelX) < 0.1f)) // stops if no input and too slow
				fPlayerVelX = 0;
		}


		if (fPlayerVelX > 10.0f)
			fPlayerVelX = 10.0f;
		if (fPlayerVelX < -10.0f)
			fPlayerVelX = -10.0f;
		if (fPlayerVelY > 100.0f)
			fPlayerVelY = 100.0f;
		if (fPlayerVelY < -100.0f)
			fPlayerVelY = -100.0f;

		float fNewPlayerPosX = fPlayerPosX + fPlayerVelX * fElapsedTime;
		float fNewPlayerPosY = fPlayerPosY + fPlayerVelY * fElapsedTime;

		// Collision detection
		if (fPlayerVelX <= 0) {
			if (GetTile(fNewPlayerPosX + 0.0f, fPlayerPosY + 0.0f) != '.' || GetTile(fNewPlayerPosX + 0.0f, fPlayerPosY + 0.9f) != '.') {
				fNewPlayerPosX = (int)fNewPlayerPosX + 1;
				fPlayerVelX = 0;
			}
		}
		else {
			if (GetTile(fNewPlayerPosX + 1.0f, fPlayerPosY + 0.0f) != '.' || GetTile(fNewPlayerPosX + 1.0f, fPlayerPosY + 0.9f) != '.') {
				fNewPlayerPosX = (int)fNewPlayerPosX;
				fPlayerVelX = 0;
			}
		}

		bPlayerOnGround = false;
		if (fPlayerVelY <= 0) {
			if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 0.0f) != '.' || GetTile(fNewPlayerPosX + 0.9f, fNewPlayerPosY + 0.0f) != '.') {
				fNewPlayerPosY = (int)fNewPlayerPosY + 1;
				fPlayerVelY = 0;
			}
		}
		else {
			if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f) != '.' || GetTile(fNewPlayerPosX + 0.9f, fNewPlayerPosY + 1.0f) != '.') {
				fNewPlayerPosY = (int)fNewPlayerPosY;
				fPlayerVelY = 0;
				bPlayerOnGround = true;
			}
		}

		if (fNewPlayerPosX < 0.0f) { // resolve small oob bug on left border
			fNewPlayerPosX = 0.0f;
			fPlayerVelX = 0.0f;
		}

		fPlayerPosX = fNewPlayerPosX;
		fPlayerPosY = fNewPlayerPosY;

		fCameraPosX = fPlayerPosX;
		fCameraPosY = fPlayerPosY;



		// Draw level
		
		int nVisibleTilesX = ScreenWidth() / nTileWidth;
		int nVisibleTilesY = ScreenHeight() / nTileHeight;


		float fOffsetX = fCameraPosX - (float)nVisibleTilesX / 2.0f;
		float fOffsetY = fCameraPosY - (float)nVisibleTilesY / 2.0f;

		// Clamp camera to boundaries
		if (fOffsetX < 0) fOffsetX = 0;
		if (fOffsetY < 0) fOffsetY = 0;
		if (fOffsetX > nLevelWidth - nVisibleTilesX) fOffsetX = nLevelWidth - nVisibleTilesX;
		if (fOffsetY > nLevelHeight - nVisibleTilesY) fOffsetY = nLevelHeight - nVisibleTilesY;

		float fTileOffsetX = (fOffsetX - (int)fOffsetX) * nTileWidth;
		float fTileOffsetY = (fOffsetY - (int)fOffsetY) * nTileHeight;

		// Draw visible tiles
		for (int x = -1; x < nVisibleTilesX + 1; x++) {
			for (int y = -1; y < nVisibleTilesY + 1; y++) {

				char sTileID = GetTile(x + fOffsetX, y + fOffsetY);

				olc::vi2d TileSize = { nTileWidth, nTileHeight };

				switch (sTileID) {
				case '.': // Sky
					FillRect(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, nTileWidth, nTileHeight, olc::CYAN);
					break;

				case '#': // Ground
					DrawPartialSprite({ (int)(x * nTileWidth - fTileOffsetX), (int)(y * nTileHeight - fTileOffsetY) }, TileSheet, { nTileWidth * 0, 0 }, TileSize);
					break;

				case 'B': // Brick
					DrawPartialSprite({ (int)(x * nTileWidth - fTileOffsetX), (int)(y * nTileHeight - fTileOffsetY) }, TileSheet, { nTileWidth * 1, 0 }, TileSize);
					break;

				case 'o': // coin
					DrawPartialSprite({ (int)(x * nTileWidth - fTileOffsetX), (int)(y * nTileHeight - fTileOffsetY) }, TileSheet, { nTileWidth * 2, 0 }, TileSize);
					break;

				case '?': // ?box
					DrawPartialSprite({ (int)(x * nTileWidth - fTileOffsetX), (int)(y * nTileHeight - fTileOffsetY) }, TileSheet, { nTileWidth * 3, 0 }, TileSize);
					break;

				default:
					break;
				}
			}
		}

		// Draw player
		
		olc::vi2d playerpos = { (int)((fPlayerPosX - fOffsetX) * nTileWidth), (int)((fPlayerPosY - fOffsetY) * nTileHeight) };
		

		if (fPlayerVelX > 0) {
			bPlayerDir = true;
		}
		else if (fPlayerVelX < 0) {
			bPlayerDir = false;
		}

		olc::vi2d offset = {bPlayerDir ? 16 : 0, bPlayerOnGround ? 0 : 16};

		DrawPartialSprite(playerpos, PlayerSprite, offset, TileSize);

		DrawString({ 0, 0 }, std::to_string(coins) + " coins");

		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}