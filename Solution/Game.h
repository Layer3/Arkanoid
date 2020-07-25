#pragma once

#include "Player.h"
#include "Global.h"
#include "Projectile.h"
#include "RenderUtil.h"
#include "Tile.h"
#include <SDL.h>
#include <SDL_keycode.h>
#include <SDL_ttf.h>
#include <vector>

namespace Arkanoid::Audio
{
class CAudioManager;
} // Arkanoid::Audio

namespace Arkanoid::Game
{
class CGame final
{
public:

	CGame() = default;
	~CGame();

	void               Initialize(SDL_Renderer* const pRenderer, Arkanoid::Audio::CAudioManager* const pAudioManager);
	void               Update(unsigned int const frameTime);
	void               Reset();
	unsigned int const GetScore() const { return m_score; }

private:

	void LoadLevel(char const* levelPath);

	void Input();
	void Render();
	void UpdateObjects(unsigned int const frameTime);
	void UpdateProjectiles(unsigned int const frameTime);

	void SoftReset();

	SDL_Renderer*                   m_pRenderer = nullptr;
	Arkanoid::Audio::CAudioManager* m_pAudioManager = nullptr;
	
	std::tuple<unsigned char, CTile*>         m_level[g_levelHeightTiles][g_levelWidthTiles]{};
	std::vector<std::unique_ptr<CTile>>       m_pTiles{};
	std::vector<std::unique_ptr<CProjectile>> m_pProjectiles{};
	std::vector<std::unique_ptr<CProjectile>> m_pAttachedProjectiles{};
	std::unique_ptr<SCustomTexture>           m_pBackgroundGame;
	std::unique_ptr<SCustomTexture>           m_pScoreTitle;
	std::unique_ptr<SCustomTexture>           m_pScore;
	std::unique_ptr<SCustomTexture>           m_pPlayerLivesTitle;

	unsigned int m_score = 0;
	SDL_Color    m_textColor = { 255, 255, 255 };
	TTF_Font*    m_font = nullptr;
	
	CPlayer       m_player;
	SDL_KeyCode   m_dominantDirectionKey = SDLK_UNKNOWN;
	SDL_KeyCode   m_recessiveDirectionKey = SDLK_UNKNOWN;
	bool          m_aKeyDown = false;
	bool          m_dKeyDown = false;
	bool          m_wKeyDown = false;
	bool          m_waitForWKeyDown = false;
	
	bool          m_roundStarted = false;
	bool          m_gameRunning = true;

	unsigned char m_currentLevel = 0;
};
} // Arkanoid::Game
