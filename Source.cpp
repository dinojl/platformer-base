#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "Levels.h"




class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

private:

	lvl::Level CurrentLevel;
	std::map<std::string, lvl::Level> Manifest = lvl::init();
	bool bAdvanceLevel = false;

	std::string sLevel;
	int nLevelWidth;
	int nLevelHeight;

	bool bPlayerOnGround = false;
	bool bPlayerDir = true;
	bool bPlayerAlive = true;
	float fDeathTimer = 0.0f;

	float fCameraPosX = 0.0f;
	float fCameraPosY = 0.0f;

	float fPlayerPosX = 0.0f;
	float fPlayerPosY = 0.0f;

	float fNewPlayerPosX;
	float fNewPlayerPosY;

	float fPlayerVelX = 0.0f;
	float fPlayerVelY = 0.0f;

	int nStartingPoints = 30;
	int nPoints = nStartingPoints;
	float fPointTimer = 0.0f;

	// jump vars + precomp
	float fJumpTimer = 0.0;
	float fJumpFloatTime = 0.1f;
	float fJumpImpulse = 10.0f;
	float fJumpPower = fJumpImpulse / fJumpFloatTime;

	olc::vi2d TileSize = { 16, 16 };
	int nTileWidth = TileSize.x;
	int nTileHeight = TileSize.y;

	olc::Renderable* TileSheet = nullptr;
	olc::Renderable* PlayerSprite = nullptr;

	void LoadLevel(std::string ID) {
		lvl::Level lvl = Manifest[ID];
		CurrentLevel = lvl;
		CurrentLevel.sID = ID;
		sLevel = lvl.sMap;
		nLevelWidth = lvl.nWidth;
		nLevelHeight = lvl.nHeight;
		fPlayerVelX = 0.0f;
		fPlayerVelY = 0.0f;
		fPlayerPosX = lvl.nPlayerX;
		fPlayerPosY = lvl.nPlayerY;
		bPlayerDir = lvl.bPlayerDir;
		bAdvanceLevel = false;
	}

public:

	bool OnUserCreate() override
	{
		lvl::init();
		LoadLevel("Start");

		TileSheet = new olc::Renderable();
		TileSheet->Load("assets/SpriteSheet.png");
		PlayerSprite = new olc::Renderable();
		PlayerSprite->Load("assets/PlayerSprite.png");

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
		auto KillPlayer = [&]() {
			fPlayerVelY = -15;
			fPlayerVelX = 0;
			bPlayerAlive = false;
		};
		auto PickupHandler = [&](int x, int y, char side = ' ') {
			switch (GetTile(x, y)) {
			case 'o':  // Coin pickup
				SetTile(x, y, '.');
				nPoints += 5;
				break;
			case '?':  // ?box pickup
				if (side == 't') {
					SetTile(x, y, 'B');
					nPoints += (rand() % 4 + 1) * 4;
				}
				break;
			case 'l':  // Level advance
			case 'L':
				bAdvanceLevel = true;
				break;
			case 'x':  // Spikes
				if(side != 't' && (fPlayerPosX > x - 0.75f && fPlayerPosX < x + 0.25))
					KillPlayer();
				break;
			default:
				break;
			}
		};

		// Input handling

		bool bPlayerMoving = false;
		if (IsFocused() && bPlayerAlive) {
			if (GetKey(olc::LEFT).bHeld) {
				fPlayerVelX += (bPlayerOnGround ? -20.0f : -4.0f) * fElapsedTime;
				bPlayerMoving = true;
			}
			if (GetKey(olc::RIGHT).bHeld) {
				fPlayerVelX += (bPlayerOnGround ? 20.0f : 4.0f) * fElapsedTime;
				bPlayerMoving = true;
			}
			if (GetKey(olc::UP).bHeld) {
				if (fJumpTimer > 0 || bPlayerOnGround) {
					fJumpTimer += fElapsedTime;
					if (fJumpTimer < fJumpFloatTime) {
						fPlayerVelY -= (20.0f * fElapsedTime) + fJumpPower * fElapsedTime;
					}
				}
			}
			else
				fJumpTimer = 0.0f;  // reset jump timer
		}

		if (bPlayerOnGround) {
			fPlayerVelX += -3.0f * fPlayerVelX * fElapsedTime;
			if (!bPlayerMoving && (fabs(fPlayerVelX) < 0.1f)) // stops if no input and too slow
				fPlayerVelX = 0;
		}

		if (fPlayerPosY >= CurrentLevel.nHeight - 1 && bPlayerAlive && CurrentLevel.nHeight > 0)
			KillPlayer();

		fPlayerVelY += 20.0f * fElapsedTime;

		if (fPlayerVelX > 10.0f)
			fPlayerVelX = 10.0f;
		if (fPlayerVelX < -10.0f)
			fPlayerVelX = -10.0f;
		if (fPlayerVelY > 100.0f)
			fPlayerVelY = 100.0f;
		if (fPlayerVelY < -100.0f)
			fPlayerVelY = -100.0f;

		// calc new player pos
		float fNewPlayerPosX = fPlayerPosX + fPlayerVelX * fElapsedTime;
		float fNewPlayerPosY = fPlayerPosY + fPlayerVelY * fElapsedTime;

		if (bPlayerAlive) { // noclip if dead
			// Handle coin pickups
			PickupHandler((int)(fNewPlayerPosX + 0), (int)(fNewPlayerPosY + 0), 't');
			PickupHandler((int)(fNewPlayerPosX + 0), (int)(fNewPlayerPosY + 1), 'b');
			PickupHandler((int)(fNewPlayerPosX + 1), (int)(fNewPlayerPosY + 0), 't');
			PickupHandler((int)(fNewPlayerPosX + 1), (int)(fNewPlayerPosY + 1), 'b');


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
		}

		if (fNewPlayerPosX < 0.0f) { // resolves small oob bug on left border
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

		Clear(olc::CYAN);  // Clear screen

		// Draw visible tiles
		for (int x = -1; x < nVisibleTilesX + 1; x++) {
			for (int y = -1; y < nVisibleTilesY + 1; y++) {

				char sTileID = GetTile(x + fOffsetX, y + fOffsetY);

				olc::vi2d TileSize = { nTileWidth, nTileHeight };

				switch (sTileID) {
				case '.': // Sky
					//FillRect(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, nTileWidth, nTileHeight, olc::CYAN); //Old, slow method
					break;

				case '#': // Ground
					DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, TileSize, TileSheet->Decal(),
						{ (float)nTileWidth * 0, (float)nTileWidth * 0 }, TileSize);
					break;

				case 'B': // Brick
					DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, TileSize, TileSheet->Decal(),
						{ (float)nTileWidth * 1, (float)nTileWidth * 0 }, TileSize);
					break;

				case 'o': // coin
					DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, TileSize, TileSheet->Decal(),
						{ (float)nTileWidth * 2, (float)nTileWidth * 0 }, TileSize);
					break;

				case '?': // ?box
					DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, TileSize, TileSheet->Decal(),
						{ (float)nTileWidth * 3, (float)nTileWidth * 0 }, TileSize);
					break;

				case 'l': // Portal top
					DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, TileSize, TileSheet->Decal(),
						{ (float)nTileWidth * 4, (float)nTileWidth * 0 }, TileSize);
					break;

				case 'L': // Portal base
					DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, TileSize, TileSheet->Decal(),
						{ (float)nTileWidth * 4, (float)nTileWidth * 1 }, TileSize);
					break;

				case 'x': // Spikes
					DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, TileSize, TileSheet->Decal(),
						{ (float)nTileWidth * 0, (float)nTileWidth * 1 }, TileSize);
					break;

				default:
					break;
				}
			}
		}

		
		
		olc::vi2d playerpos = { (int)((fPlayerPosX - fOffsetX) * nTileWidth), (int)((fPlayerPosY - fOffsetY) * nTileHeight) };
		
		// Find player direction
		if (fPlayerVelX > 0) {
			bPlayerDir = true;
		}
		else if (fPlayerVelX < 0) {
			bPlayerDir = false;
		}

		olc::vi2d offset = {bPlayerDir ? 16 : 0, bPlayerOnGround ? 0 : 16};
		DrawPartialDecal(playerpos, TileSize, PlayerSprite->Decal(), offset, TileSize);  // Draw player

		// Draw point counter
		DrawStringDecal({ 1.0f, 1.0f }, std::to_string(nPoints));
		if (nPoints == 1)
			DrawStringPropDecal({ (float)(std::to_string(nPoints).length() + 1) * 8 - 4, 1.0f }, " point");
		else
			DrawStringPropDecal({ (float)(std::to_string(nPoints).length() + 1) * 8 - 4, 1.0f }, " points");


		if(bAdvanceLevel && bPlayerAlive)
			LoadLevel(CurrentLevel.sNextLevelID);
		

		if (bPlayerAlive) {  // Handle point timer
			fPointTimer += fElapsedTime;
			if (fPointTimer >= 1) {
				nPoints--;
				fPointTimer--;
			}
			if (nPoints <= 0) {
				nPoints = nStartingPoints;
				KillPlayer();
			}
		}

		if (!bPlayerAlive) {
			// Draw game over box
			FillRectDecal({ (float)ScreenWidth() / 4, (float)ScreenHeight() / 8 * 3 }, { (float)ScreenWidth() / 2, (float)ScreenHeight() / 4 }, olc::BLACK);
			FillRectDecal({ (float)ScreenWidth() / 4 + 1, (float)ScreenHeight() / 8 * 3 + 1 }, { (float)ScreenWidth() / 2 - 2, (float)ScreenHeight() / 4 - 2 }, olc::GREY);

			std::string p = "Game Over";
			DrawStringPropDecal({ (float)ScreenWidth() / 2 - (float)GetTextSizeProp(p).x / 2, (float)ScreenHeight() / 2 - 8 }, p, olc::BLACK);
			p = std::to_string(5 - (int)(fDeathTimer));
			DrawStringPropDecal({ (float)ScreenWidth() / 2 - (float)GetTextSizeProp(p).x / 2, (float)ScreenHeight() / 2 + 1 }, p, olc::BLACK);

			// Start game over timer
			fDeathTimer += fElapsedTime;
		}

		if (fDeathTimer >= 5.0f) {
			// Restart after death
			fPointTimer = 0;
			fDeathTimer = 0.0f;
			bPlayerAlive = true;
			LoadLevel(CurrentLevel.sID);
		}

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