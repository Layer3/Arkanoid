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
	GameOver,
	ShutDown,
};

extern EGameState s_gameState;

constexpr float          g_musicVolume = 0.05f;

constexpr unsigned short g_mapWidth = 400;
constexpr unsigned short g_mapHeight = 500;

constexpr unsigned short g_menuButtonWidth = 180;
constexpr unsigned short g_menuButtonHeight = 50;
constexpr unsigned char  g_numMenuButtons = 2;

constexpr unsigned int   g_targetFPS = 100;
constexpr unsigned int   g_targetFrameTime = 1000 / g_targetFPS;

constexpr int const      g_numCollisionSounds = 10;

// Levels
constexpr unsigned char const g_numLevels = 3;

constexpr char const* asset_level01 = "../../../Assets/Levels/1.level";
constexpr char const* asset_level02 = "../../../Assets/Levels/2.level";
constexpr char const* asset_level03 = "../../../Assets/Levels/3.level";

constexpr char const* asset_levels[g_numLevels]
{
	asset_level01,
	asset_level02
};

// Font
constexpr char const* asset_font_sans = "../../../Assets/Graphics/OpenSans-Regular.ttf";

// Textures Menu
constexpr char const* asset_texture_backgroundMenu = "../../../Assets/Graphics/BackgroundMenu.png";
constexpr char const* asset_texture_buttonDefault = "../../../Assets/Graphics/ButtonDefault.png";
constexpr char const* asset_texture_buttonSelected = "../../../Assets/Graphics/ButtonSelected.png";

// Textures Game
constexpr char const* asset_texture_tile01 = "../../../Assets/Graphics/Tile01.png";
constexpr char const* asset_texture_tile02 = "../../../Assets/Graphics/Tile02.png";
constexpr char const* asset_texture_tile03 = "../../../Assets/Graphics/Tile03.png";
constexpr char const* asset_texture_tile04 = "../../../Assets/Graphics/Tile04.png";
constexpr char const* asset_texture_backgroundGame = "../../../Assets/Graphics/BackgroundGame.png";
constexpr char const* asset_texture_energyBar = "../../../Assets/Graphics/Energy.png";
constexpr char const* asset_texture_player = "../../../Assets/Graphics/Player.png";
constexpr char const* asset_texture_projectile = "../../../Assets/Graphics/Projectile.png";

constexpr char const* asset_tileTextures[4]
{
	asset_texture_tile01,
	asset_texture_tile02,
	asset_texture_tile03,
	asset_texture_tile04
};

// Audio
constexpr char const* asset_audio_ui_buttonSwitch = "../../../Assets/Audio/UIButtonSelect.wav";
constexpr char const* asset_audio_ui_buttonSelect = "../../../Assets/Audio/UIButtonSwitch.wav";

constexpr char const* asset_audio_tileCollision01 = "../../../Assets/Audio/Arkanoid_Impact_Counter_01.wav";
constexpr char const* asset_audio_tileCollision02 = "../../../Assets/Audio/Arkanoid_Impact_Counter_02.wav";
constexpr char const* asset_audio_tileCollision03 = "../../../Assets/Audio/Arkanoid_Impact_Counter_03.wav";
constexpr char const* asset_audio_tileCollision04 = "../../../Assets/Audio/Arkanoid_Impact_Counter_04.wav";
constexpr char const* asset_audio_tileCollision05 = "../../../Assets/Audio/Arkanoid_Impact_Counter_05.wav";
constexpr char const* asset_audio_tileCollision06 = "../../../Assets/Audio/Arkanoid_Impact_Counter_06.wav";
constexpr char const* asset_audio_tileCollision07 = "../../../Assets/Audio/Arkanoid_Impact_Counter_07.wav";
constexpr char const* asset_audio_tileCollision08 = "../../../Assets/Audio/Arkanoid_Impact_Counter_08.wav";
constexpr char const* asset_audio_tileCollision09 = "../../../Assets/Audio/Arkanoid_Impact_Counter_09.wav";
constexpr char const* asset_audio_tileCollision10 = "../../../Assets/Audio/Arkanoid_Impact_Counter_10.wav";
constexpr char const* asset_audio_wallImpact = "../../../Assets/Audio/Arkanoid_Impact_01.wav";
constexpr char const* asset_audio_playerImpact = "../../../Assets/Audio/Arkanoid_Impact_Player_01.wav";

constexpr char const* asset_audio_bullettimeDrop = "../../../Assets/Audio/Arkanoid_Bullettime_Drop_01.wav";

constexpr char const* asset_audio_playerLoseLife = "../../../Assets/Audio/Arkanoid_Lose_Life_01.wav";

constexpr char const* asset_audio_tileCollisions[g_numCollisionSounds]
{
	asset_audio_tileCollision01,
	asset_audio_tileCollision02,
	asset_audio_tileCollision03,
	asset_audio_tileCollision04,
	asset_audio_tileCollision05,
	asset_audio_tileCollision06,
	asset_audio_tileCollision07,
	asset_audio_tileCollision08,
	asset_audio_tileCollision09,
	asset_audio_tileCollision10
};


constexpr char const* asset_audio_musicMenu = "../../../Assets/Audio/Music_menu.wav";
constexpr char const* asset_audio_musicGameTension = "../../../Assets/Audio/Music_game_tension.wav";
constexpr char const* asset_audio_musicGameDefault = "../../../Assets/Audio/Music_game.wav";

constexpr char const* asset_audio_musicTracks[3]
{
	asset_audio_musicMenu,
	asset_audio_musicGameDefault,
	asset_audio_musicGameTension
};

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

	constexpr float          g_playerSpeed = 320.0f;// in pixels/sec
	constexpr unsigned short g_playerHeight = 460;

	constexpr float          g_projectileSpeed = 250.0f;
	constexpr float          g_maxVelocity = 1.5f;

	constexpr unsigned int   g_score_tileDestroyed = 100;
	constexpr unsigned int   g_score_tileDamaged = 50;
}

namespace Arkanoid::Audio
{
	constexpr float g_euler = 2.718f;
}
