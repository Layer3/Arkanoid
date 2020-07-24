#pragma once
#include "AudioManager.h"
#include "Game.h"
#include <SDL.h>
#include <vector>

namespace Arkanoid
{
class CArkanoid
{
public:

	CArkanoid() = default;
	~CArkanoid();
	
	void Initialize();
	void Run();

private:

	void MainLoop();
	void Input();
	void RenderMenu();

	// General
	SDL_Window*                     m_pWindow = nullptr;
	SDL_Renderer*                   m_pRenderer = nullptr;
	Arkanoid::Audio::CAudioManager* m_pAudioManager = nullptr;
	Arkanoid::Game::CGame           m_game;

	unsigned int  m_lastFrameTime = 0;
	unsigned int  m_currentFrameTime = 0;

	// Menu specifics
	SDL_Texture* m_pBackgroundMenu = nullptr;
	SDL_Texture* m_pMenuButtonDefault = nullptr;
	SDL_Texture* m_pMenuButtonSelected = nullptr;

	bool m_enterKeyDown = false;
	bool m_wKeyDown = false;
	bool m_sKeyDown = false;
};
} // Arkanoid
