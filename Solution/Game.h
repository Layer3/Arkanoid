#pragma once

#include "Player.h"
#include "Constexpr.h"
#include <SDL.h>
#include <SDL_keycode.h>
#include <vector>

namespace Arkanoid::Audio
{
class CAudioManager;
} // Arkanoid::Audio

namespace Arkanoid::Game
{
class CTile;
class CProjectile;

class CGame
{
public:

	CGame() = default;
	~CGame();

	void Initialize(SDL_Renderer* pRenderer, Arkanoid::Audio::CAudioManager* pAudioManager, const char* const levelAsset);
	void Update(unsigned int const frameTime);
	void Reset();

private:

	void LoadLevel(const char* levelPath);

	void Input();
	void Render();
	void UpdateObjects(unsigned int const frameTime);
	void UpdateProjectiles(unsigned int const frameTime);

	void SoftReset();

	SDL_Renderer*                   m_pRenderer = nullptr;
	Arkanoid::Audio::CAudioManager* m_pAudioManager;
	
	std::tuple<unsigned char, CTile*> m_level[g_levelHeightTiles][g_levelWidthTiles]{};
	std::vector<CTile*>               m_pTiles{};
	std::vector<CProjectile*>         m_pProjectiles{};
	std::vector<CProjectile*>         m_pAttachedProjectiles{};
	SDL_Texture*                      m_pBackgroundGame = nullptr;
	
	CPlayer       m_player;
	SDL_KeyCode   m_dominantDirectionKey = SDLK_UNKNOWN;
	SDL_KeyCode   m_recessiveDirectionKey = SDLK_UNKNOWN;
	bool          m_aKeyDown = false;
	bool          m_dKeyDown = false;
	bool          m_wKeyDown = false;
	bool          m_waitForWKeyDown = false;
	
	bool          m_roundStarted = false;
	bool          m_gameRunning = true;
};
} // Arkanoid::Game
