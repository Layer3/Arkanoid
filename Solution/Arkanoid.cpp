#include "Arkanoid.h"
#include "AudioBuffer.h"
#include "Global.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <portaudio.h>
#include <iostream>
#include <string>

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
	TTF_Init();
	Pa_Initialize(); // Takes ages to initialize

	m_pAudioManager = new Arkanoid::Audio::CAudioManager();
	m_pBackgroundMenu = std::make_unique<SCustomTexture>(IMG_LoadTexture(m_pRenderer, asset_texture_backgroundMenu));
	m_pMenuButtonDefault = std::make_unique<SCustomTexture>(IMG_LoadTexture(m_pRenderer, asset_texture_buttonDefault));
	m_pMenuButtonSelected = std::make_unique<SCustomTexture>(IMG_LoadTexture(m_pRenderer, asset_texture_buttonSelected));

	// Prepare menu buttons
	m_font = TTF_OpenFont(asset_font_sans, 20);
	SDL_Color textColor = { 255, 255, 255 };

	SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, "High Score", textColor);
	m_pHighScoreTitle = std::make_unique<SCustomTexture>(SDL_CreateTextureFromSurface(m_pRenderer, textSurface));
	SDL_FreeSurface(textSurface);

	textSurface = TTF_RenderText_Solid(m_font, "Play", textColor);
	m_menuButtonTexts.emplace_back(new SCustomTexture(SDL_CreateTextureFromSurface(m_pRenderer, textSurface)));
	SDL_FreeSurface(textSurface);

	textSurface = TTF_RenderText_Solid(m_font, "Quit", textColor);
	m_menuButtonTexts.emplace_back(new SCustomTexture(SDL_CreateTextureFromSurface(m_pRenderer, textSurface)));
	SDL_FreeSurface(textSurface);

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
				m_game.Initialize(m_pRenderer, m_pAudioManager);

				m_pAudioManager->SetMusic(Audio::EMusic::Game);
				s_gameState = EGameState::Playing;
				break;
			}
		case EGameState::Playing:
			{
				m_game.Update(m_currentFrameTime);
				break;
			}
		case EGameState::GameOver:
			{
				if (m_game.GetScore() > m_highScore)
				{
					m_highScore = m_game.GetScore();
				}

				m_game.Reset();

				m_pAudioManager->SetMusic(Audio::EMusic::Menu);

				s_gameState = EGameState::Menu;
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
		else if (event.type == static_cast<Uint32>(expectedEvent))
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				{
					wDown = (expectedEvent == SDL_KEYDOWN);
					
					if (wDown)
					{
						SelectedButtonUp();
						m_pAudioManager->Play(asset_audio_ui_buttonSwitch);
					}

					break;
				}
			case SDLK_s:
				{
					sDown = (expectedEvent == SDL_KEYDOWN);

					if (sDown)
					{
						SelectedButtonDown();
						m_pAudioManager->Play(asset_audio_ui_buttonSwitch);
					}

					break;
				}
			case SDLK_RETURN:
				{
					enterDown = (expectedEvent == SDL_KEYDOWN);

					if (enterDown)
					{
						ButtonAction();
						m_pAudioManager->Play(asset_audio_ui_buttonSelect);
						enterDown = false;
					}

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
		}
	}

	m_enterKeyDown = enterDown;
	m_wKeyDown = wDown;
	m_sKeyDown = sDown;
}

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::SelectedButtonDown()
{
	++m_selectedButton;

	if (m_selectedButton >= g_numMenuButtons)
	{
		m_selectedButton = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::SelectedButtonUp()
{
	// intentional underflow
	--m_selectedButton;

	if (m_selectedButton >= g_numMenuButtons)
	{
		m_selectedButton = g_numMenuButtons - 1;
	}
}

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::ButtonAction()
{
	EMenuButtons const selectedButton = static_cast<EMenuButtons>(m_selectedButton + 1);

	switch (selectedButton)
	{
	case EMenuButtons::Play:
		{
			s_gameState = EGameState::Start;
			break;
		}
	case EMenuButtons::Quit:
		{
			s_gameState = EGameState::ShutDown;
			break;
		}
	case EMenuButtons::None: // intentional fall-through
	default:
		{
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::RenderMenu()
{
	// Background
	SDL_RenderCopy(m_pRenderer, m_pBackgroundMenu->m_pTexture, nullptr, nullptr);

	// HighScore
	unsigned int scoreDigits = 1;
	unsigned int score = m_highScore;
	score /= 10;

	while (score > 0)
	{
		score /= 10;
		++scoreDigits;
	}

	SDL_Rect scorePosition = { 110, 150, 180, 50 };
	SDL_RenderCopy(m_pRenderer, m_pHighScoreTitle->m_pTexture, nullptr, &scorePosition);

	if (scoreDigits > 0)
	{
		SDL_Color stillWhite{ 255,255,255 };
		SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, std::to_string(m_highScore).c_str(), stillWhite);
		SDL_Texture* pScore = SDL_CreateTextureFromSurface(m_pRenderer, textSurface);

		scorePosition.y += scorePosition.h;
		scorePosition.x = 207 - static_cast<int>(7.5f * scoreDigits);
		scorePosition.w = 15 * scoreDigits;

		SDL_RenderCopy(m_pRenderer, pScore, nullptr, &scorePosition);

		SDL_DestroyTexture(pScore);
		SDL_FreeSurface(textSurface);
	}

	// Buttons
	SDL_Rect buttonPosition = { 110, 250, g_menuButtonWidth, g_menuButtonHeight};
	SDL_Rect textPosition = { buttonPosition.x + 60, buttonPosition.y + 7, 60, 30 };

	for (char i = 0; i < g_numMenuButtons; ++i)
	{
		SDL_RenderCopy(m_pRenderer, (m_selectedButton == i) ? m_pMenuButtonSelected->m_pTexture : m_pMenuButtonDefault->m_pTexture, nullptr, &buttonPosition);
		SDL_RenderCopy(m_pRenderer, m_menuButtonTexts[i]->m_pTexture, nullptr, &textPosition);

		buttonPosition.y += 70; // magic number that is the button width + some room 
		textPosition.y += 70; 
	}

	// Send off
	SDL_RenderPresent(m_pRenderer);
}

//////////////////////////////////////////////////////////////////////////////////
void CArkanoid::Shutdown()
{
	delete m_pAudioManager;

	Pa_Terminate();
	TTF_Quit();
	IMG_Quit();
	SDL_DestroyWindow(m_pWindow);
	SDL_DestroyRenderer(m_pRenderer);
	SDL_Quit();
}
} // Arkanoid
