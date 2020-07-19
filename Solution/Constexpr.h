#pragma once
#include <atomic>

namespace Arkanoid
{
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

	constexpr unsigned short g_mapWidth = 400;
	constexpr unsigned short g_mapHeight = 500;

	constexpr const char* asset_Level_1 = "../../../Assets/Levels/1.level";
	constexpr const char* asset_Level_2 = "../../../Assets/Levels/2.level";

	constexpr const char* asset_texture_defaultTile = "../../../Assets/Graphics/TileDefault.png";
	constexpr const char* asset_texture_background = "../../../Assets/Graphics/Background.png";
	constexpr const char* asset_texture_player = "../../../Assets/Graphics/Player.png";
	constexpr const char* asset_texture_projectile = "../../../Assets/Graphics/Projectile.png";

	constexpr const char* asset_audio_projectileCollision1 = "../../../Assets/Audio/ProjectileCollision1.wav";
	constexpr const char* asset_audio_tileDestroy = "../../../Assets/Audio/TileDestroy.wav";
	constexpr const char* asset_audio_music = "../../../Assets/Audio/Music.WAV";
}

namespace Arkanoid::Game
{
	constexpr unsigned int   g_targetFPS = 50;
	constexpr unsigned int   g_targetFrameTime = 1000 / g_targetFPS;

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
