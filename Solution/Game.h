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
	unsigned char                   m_currentLevel = 0;
	
	std::tuple<unsigned char, CTile*>         m_level[g_levelHeightTiles][g_levelWidthTiles]{};
	std::vector<std::unique_ptr<CTile>>       m_pTiles{};
	std::vector<std::unique_ptr<CProjectile>> m_pProjectiles{};
	std::vector<std::unique_ptr<CProjectile>> m_pAttachedProjectiles{};
	std::unique_ptr<SCustomTexture>           m_pBackgroundGame;
	std::unique_ptr<SCustomTexture>           m_pEnergyBar;
	std::unique_ptr<SCustomTexture>           m_pScoreTitle;
	std::unique_ptr<SCustomTexture>           m_pScore;
	std::unique_ptr<SCustomTexture>           m_pPlayerLivesTitle;

	unsigned int m_score = 0;
	TTF_Font*    m_font = nullptr;
	SDL_Color    m_textColor = { 255, 255, 255 };
	float        m_timeFactor = 1.0f;
	float        m_oldTimeFactor = m_timeFactor;
	float        m_energy = 1.0f;

	SDL_KeyCode   m_dominantDirectionKey = SDLK_UNKNOWN;
	SDL_KeyCode   m_recessiveDirectionKey = SDLK_UNKNOWN;
	bool          m_aKeyDown = false;
	bool          m_dKeyDown = false;
	bool          m_wKeyDown = false;
	bool          m_sKeyDown = false;
	bool          m_waitForWKeyUp = false;
	bool          m_waitForSKeyUp = false;
	bool          m_roundStarted = false;
	CPlayer       m_player;
};
} // Arkanoid::Game
