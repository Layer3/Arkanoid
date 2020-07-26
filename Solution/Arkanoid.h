#pragma once
#include "AudioManager.h"
#include "Game.h"
#include "RenderUtil.h"
#include <SDL.h>
#include <memory>
#include <vector>

namespace Arkanoid
{
enum class EMenuButtons
{
	None,
	Play,
	Quit,
};

class CArkanoid final
{
public:

	CArkanoid() = default;
	~CArkanoid() = default;

	void Initialize();
	void Shutdown();
	void Run();

private:

	void MainLoop();
	void Input();
	void SelectedButtonDown();
	void SelectedButtonUp();
	void ButtonAction();
	void RenderMenu();

	// General
	SDL_Window*                     m_pWindow = nullptr;
	SDL_Renderer*                   m_pRenderer = nullptr;
	Arkanoid::Audio::CAudioManager* m_pAudioManager = nullptr;

	unsigned int m_lastFrameTime = 0;
	unsigned int m_currentFrameTime = 0;
	unsigned int m_highScore = 0;

	// Menu specifics
	std::unique_ptr<SCustomTexture> m_pBackgroundMenu;
	std::unique_ptr<SCustomTexture> m_pHighScoreTitle;
	std::unique_ptr<SCustomTexture> m_pMenuButtonDefault;
	std::unique_ptr<SCustomTexture> m_pMenuButtonSelected;
	std::unique_ptr<SCustomTexture> m_pTexturePlay;
	std::unique_ptr<SCustomTexture> m_pTextureQuit;
	TTF_Font*                       m_font;

	std::vector<std::unique_ptr<SCustomTexture>> m_menuButtonTexts;

	unsigned char m_selectedButton = 0;
	bool          m_enterKeyDown = false;
	bool          m_wKeyDown = false;
	bool          m_sKeyDown = false;

	Arkanoid::Game::CGame m_game;
};
} // Arkanoid
