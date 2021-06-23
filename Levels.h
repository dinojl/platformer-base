#pragma once

namespace lvl {
	struct Level {
		Level(std::string Map, int Width, int Height, int PlayerX = 0, int PlayerY = 0, bool PlayerDir = true, std::string NextLevelID = "") {
			sMap = Map;
			nWidth = Width;
			nHeight = Height;
			nPlayerX = PlayerX;
			nPlayerY = PlayerY;
			bPlayerDir = PlayerDir;
			sNextLevelID = NextLevelID;
		}
		Level() {}
		std::string sMap = "";
		int nWidth = 0;
		int nHeight = 0;
		int nPlayerX = 0;
		int nPlayerY = 0;
		bool bPlayerDir = true;
		std::string sNextLevelID = "";
	};

	
	
	std::map<std::string, Level> init() {

		std::map<std::string, Level> Manifest;

		// Debug
		Manifest.insert(std::pair<std::string, Level>("Debug", Level((
			"................................................................"
			"................................................................"
			"................................................................"
			"................................................................"
			"................................................................"
			"................................................................"
			"....................................B?B........................."
			"................................................................"
			".....................o............................????x???......"
			"............#######.........##.................................."
			"...........###..............##.................................l"
			"..........#####...........................x....................L"
			"####x#################..###########.############################"
			"#######################.###########.############################"
			"#######################.............############################"
			"#############################..#################################"), 64, 16, 1, 1, true, "Debug")));

		// Start
		Manifest.insert(std::pair<std::string, Level>("Start", Level((
			"..............................................................." 
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..............................................................."
			"..................ooooooooooooooooooooooooooooooooooo.........."
			"................ooooooooooooooooooooooooooooooooooooooo.......l"
			"..............ooooooooooooooooooooooooooooooooooooooooooo.....L"
			"###############################################################"
			"###############################################################"
			"###############################################################"
			"###############################################################"), 63, 20, 1, 1, true, "Brickworld")));



		return Manifest;
	}

	
}

/*


*/