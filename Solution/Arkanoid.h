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

	CArkanoid();
	~CArkanoid();

private:

	SDL_Window*   m_pWindow = nullptr;
	SDL_Renderer* m_pRenderer = nullptr;

	Arkanoid::Audio::CAudioManager* m_pAudioManager;
	Arkanoid::Game::CGame           m_game;
};
} // Arkanoid
