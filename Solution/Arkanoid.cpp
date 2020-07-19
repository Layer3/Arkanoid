#include "Arkanoid.h"
#include "AudioBuffer.h"
#include "Constexpr.h"
#include <SDL_image.h>
#include <portaudio.h>

namespace Arkanoid
{
//////////////////////////////////////////////////////////////////////////////////
CArkanoid::CArkanoid()
{
	SDL_Init(0);
	SDL_CreateWindowAndRenderer(400, 500, 0, &m_pWindow, &m_pRenderer);
	SDL_SetWindowTitle(m_pWindow, "Arkanoid");
	IMG_Init(IMG_INIT_PNG);
	Pa_Initialize();
	m_pAudioManager = new Arkanoid::Audio::CAudioManager();
	m_game.RunGame(m_pRenderer, m_pAudioManager, asset_Level_1);
}

//////////////////////////////////////////////////////////////////////////////////
CArkanoid::~CArkanoid()
{
	Pa_Terminate();
	IMG_Quit();
	SDL_DestroyWindow(m_pWindow);
	SDL_DestroyRenderer(m_pRenderer);
	SDL_Quit();
}
} // Arkanoid
