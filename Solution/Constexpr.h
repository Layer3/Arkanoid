#pragma once
#include <atomic>

namespace Arkanoid
{
	enum class EGameState
	{
		None = 0,
		Menu,
		Start,
		Playing,
		Paused,
		ShutDown,
	};

	extern EGameState s_gameState;

	constexpr unsigned short g_mapWidth = 400;
	constexpr unsigned short g_mapHeight = 500;

	constexpr unsigned int   g_targetFPS = 50;
	constexpr unsigned int   g_targetFrameTime = 1000 / g_targetFPS;

	// Levels
	constexpr const char* asset_Level_1 = "../../../Assets/Levels/1.level";
	constexpr const char* asset_Level_2 = "../../../Assets/Levels/2.level";

	// Textures Menu
	constexpr const char* asset_texture_backgroundMenu = "../../../Assets/Graphics/backgroundMenu.png";
	constexpr const char* asset_texture_buttonDefault = "../../../Assets/Graphics/buttonDefault.png";
	constexpr const char* asset_texture_buttonSelected = "../../../Assets/Graphics/buttonSelected.png";

	// Textures Game
	constexpr const char* asset_texture_tile01 = "../../../Assets/Graphics/tile01.png";
	constexpr const char* asset_texture_tile02 = "../../../Assets/Graphics/tile02.png";
	constexpr const char* asset_texture_tile03 = "../../../Assets/Graphics/tile03.png";
	constexpr const char* asset_texture_tile04 = "../../../Assets/Graphics/tile04.png";
	constexpr const char* asset_texture_backgroundGame = "../../../Assets/Graphics/backgroundGame.png";
	constexpr const char* asset_texture_player = "../../../Assets/Graphics/player.png";
	constexpr const char* asset_texture_projectile = "../../../Assets/Graphics/projectile.png";

	constexpr const char* asset_tileTextures[4]
	{
		asset_texture_tile01,
		asset_texture_tile02,
		asset_texture_tile03,
		asset_texture_tile04
	};

	// Audio
	constexpr const char* asset_audio_projectileCollision1 = "../../../Assets/Audio/projectileCollision1.wav";
	constexpr const char* asset_audio_tileDestroy = "../../../Assets/Audio/tileDestroy.wav";
	constexpr const char* asset_audio_music = "../../../Assets/Audio/music.WAV";

	struct Vec2D final
	{
		Vec2D() = default;
		Vec2D(float x_, float y_)
			: x(x_)
			, y(y_)
		{}

		~Vec2D() = default;

		float x, y;
	};

	struct AtomicVec2D final
	{
		AtomicVec2D() = default;
		AtomicVec2D(float x_, float y_)
			: x(x_)
			, y(y_)
		{}

		AtomicVec2D(Vec2D vec)
			: x(vec.x)
			, y(vec.y)
		{}

		~AtomicVec2D() = default;

		std::atomic<float> x;
		std::atomic<float> y;
	};

	struct Pos2D final
	{
		Pos2D() = default;
		Pos2D(short x_, short y_)
			: x(x_)
			, y(y_)
		{}

		~Pos2D() = default;

		short x, y;
	};

}

namespace Arkanoid::Game
{
	constexpr unsigned char  g_levelWidthTiles = 9;   // 10 * 40
	constexpr unsigned char  g_levelHeightTiles = 21; // 30 * 20
	constexpr unsigned short g_tileWidth = 40;
	constexpr unsigned short g_tileHeight = 20;

	constexpr unsigned short g_borderLeft = static_cast<unsigned short>(g_tileWidth * 0.5f);
	constexpr unsigned short g_borderRight = g_mapWidth - g_borderLeft;
	constexpr unsigned short g_borderTop = g_tileHeight;
	constexpr unsigned short g_borderBottom = g_mapHeight - g_tileHeight;

	constexpr float          g_playerSpeed = 200.0f;// in pixels/sec
	constexpr unsigned short g_playerHeight = 460;

	constexpr float          g_projectileSpeed = 250.0f;
}

namespace Arkanoid::Audio
{
	constexpr float g_euler = 2.718f;
}
