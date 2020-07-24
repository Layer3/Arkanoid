#include "Arkanoid.h"
#include "AudioBuffer.h"
#include "Constexpr.h"
#include <SDL_image.h>
#include <portaudio.h>
#include <iostream>

namespace Arkanoid
{
EGameState s_gameState = EGameState::None;

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::Initialize()
{
	SDL_Init(0);
	SDL_CreateWindowAndRenderer(400, 500, 0, &m_pWindow, &m_pRenderer);
	SDL_SetWindowTitle(m_pWindow, "Arkanoid");
	IMG_Init(IMG_INIT_PNG);
	Pa_Initialize();

	m_pAudioManager = new Arkanoid::Audio::CAudioManager();
	m_pBackgroundMenu = IMG_LoadTexture(m_pRenderer, asset_texture_backgroundMenu);

	s_gameState = EGameState::Menu;
}

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::Run()
{
	MainLoop();
}

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::MainLoop()
{
	bool peopleAreWatching = true;

	while (peopleAreWatching)
	{
		m_currentFrameTime = SDL_GetTicks() - m_lastFrameTime;
		m_lastFrameTime += m_currentFrameTime;

		switch (s_gameState)
		{
		case EGameState::Menu: // intentional fall-through
		case EGameState::Paused:
			{
				Input();
				RenderMenu();
				break;
			}
		case EGameState::Start:
			{
				m_game.Initialize(m_pRenderer, m_pAudioManager, asset_Level_1);
				s_gameState = EGameState::Playing;
				break;
			}
		case EGameState::Playing:
			{
				m_game.Update(m_currentFrameTime);
				break;
			}
		default:
			{
				peopleAreWatching = false;
				break;
			}
		}

		unsigned int const updateTime = SDL_GetTicks() - m_lastFrameTime;

		if (updateTime < g_targetFrameTime)
		{
			SDL_Delay(g_targetFrameTime - updateTime);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::Input()
{
	SDL_Event event;
	bool enterDown = m_enterKeyDown;
	bool wDown = m_wKeyDown;
	bool sDown = m_sKeyDown;

	SDL_EventType expectedEvent = (enterDown || wDown || sDown) ? SDL_KEYUP : SDL_KEYDOWN;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			s_gameState = EGameState::ShutDown;
			break;
		}
		else if (event.type == expectedEvent)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				{
					wDown = (expectedEvent == SDL_KEYDOWN);
					break;
				}
			case SDLK_s:
				{
					sDown = (expectedEvent == SDL_KEYDOWN);
					break;
				}
			case SDLK_RETURN:
				{
					enterDown = (expectedEvent == SDL_KEYDOWN);
					break;
				}
			case SDLK_ESCAPE:
				{
					s_gameState = EGameState::ShutDown;
					break;
				}
			default:
				{
					break;
				}
			}

			if (enterDown)
			{
				enterDown = false;
				s_gameState = EGameState::Start;
			}
		}
	}

	m_enterKeyDown = enterDown;
	m_wKeyDown = wDown;
	m_sKeyDown = sDown;
}

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::RenderMenu()
{
	// Background
	SDL_RenderCopy(m_pRenderer, m_pBackgroundMenu, nullptr, nullptr);

	// Send off
	SDL_RenderPresent(m_pRenderer);
}

//////////////////////////////////////////////////////////////////////////////////
CArkanoid::~CArkanoid()
{
	delete m_pAudioManager;
	Pa_Terminate();
	IMG_Quit();
	SDL_DestroyWindow(m_pWindow);
	SDL_DestroyRenderer(m_pRenderer);
	SDL_Quit();
}
} // Arkanoid
