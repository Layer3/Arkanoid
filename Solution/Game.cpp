#include "Game.h"
#include "AudioManager.h"
#include <cmath>
#include <fstream>
#include <memory>
#include <SDL_image.h>
#include <string>

namespace Arkanoid::Game
{
//////////////////////////////////////////////////////////////////////////////////
CGame::~CGame()
{
	m_pTiles.clear();
	m_pAttachedProjectiles.clear();
	m_pProjectiles.clear();
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::Initialize(SDL_Renderer* const pRenderer, Arkanoid::Audio::CAudioManager* const pAudioManager)
{
	m_pRenderer = pRenderer;
	m_pAudioManager = pAudioManager;
	
	// Init all needed textures
	m_pBackgroundGame = std::make_unique<SCustomTexture>(IMG_LoadTexture(m_pRenderer, asset_texture_backgroundGame));
	m_pEnergyBar = std::make_unique<SCustomTexture>(IMG_LoadTexture(m_pRenderer, asset_texture_energyBar));
	
	m_player.SetTexture(m_pRenderer, asset_texture_player);

	m_font = TTF_OpenFont(asset_font_sans, 10);
	SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, "Score: ", m_textColor);
	m_pScoreTitle = std::make_unique<SCustomTexture>(SDL_CreateTextureFromSurface(m_pRenderer, textSurface));
	SDL_FreeSurface(textSurface);
	
	textSurface = TTF_RenderText_Solid(m_font, "Lives: ", m_textColor);
	m_pPlayerLivesTitle = std::make_unique<SCustomTexture>(SDL_CreateTextureFromSurface(m_pRenderer, textSurface));
	SDL_FreeSurface(textSurface);

	//
	LoadLevel(asset_levels[m_currentLevel]);

	// TODO: This projectile construction is ugly and hard coded.
	SDL_Rect projectilePosition = m_player.GetRenderPosition();
	SDL_Rect projectileSource = m_player.GetSource();

	projectilePosition.w = 15;
	projectilePosition.h = 15;
	projectileSource.w = 15;
	projectileSource.h = 15;

	CProjectile* pProjectile = new CProjectile(projectilePosition, projectileSource);
	pProjectile->SetTexture(m_pRenderer, asset_texture_projectile);
	pProjectile->SetPosition(m_player.GetPosition());
	m_pAttachedProjectiles.push_back(std::unique_ptr<CProjectile>(pProjectile));
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::Update(unsigned int const frameTime)
{
	// Level Won
	if (m_pTiles.empty())
	{
		SoftReset();

		if (++m_currentLevel >= g_numLevels)
		{
			return;
		}

		m_player.AddLives(2);

		LoadLevel(asset_levels[m_currentLevel]);
	}

	if (m_energy >= 1.0f)
	{
		if (m_sKeyDown && !m_waitForSKeyUp)
		{
			m_waitForSKeyUp = true;
			m_oldTimeFactor = m_timeFactor + 0.1f;
			m_timeFactor = m_timeFactor * 0.5f;
			m_energy = 0.0f;
			m_pAudioManager->SetFilterAmount(1.0f);
			m_pAudioManager->Play(asset_audio_bullettimeDrop);
		}
	}
	else
	{
		m_timeFactor = (0.5f * m_oldTimeFactor) + (0.5f * m_energy * m_oldTimeFactor);
		m_energy += static_cast<float>(frameTime) / 8000.0f; // take 8 seconds to refill
		
		if (m_energy >= 1.0f)
		{
			m_pAudioManager->SetFilterAmount(0.0f);
		}
		else
		{
			m_pAudioManager->SetFilterAmount(1.0f - m_energy);
		}
	}

	Input();
	UpdateObjects(frameTime);
	Render();
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::Reset()
{
	m_aKeyDown = false;
	m_dKeyDown = false;
	m_wKeyDown = false;
	m_sKeyDown = false;
	m_waitForWKeyUp = false;
	m_waitForSKeyUp = false;
	m_dominantDirectionKey = SDLK_UNKNOWN;
	m_recessiveDirectionKey = SDLK_UNKNOWN;
	m_timeFactor = 1.3f;
	m_oldTimeFactor = m_timeFactor;
	m_score = 0;
	m_roundStarted = false;
	m_currentLevel = 0;

	m_pTiles.clear();
	m_pAttachedProjectiles.clear();
	m_pProjectiles.clear();

	m_player.Reset();
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::Render()
{
	// Background
	SDL_RenderCopy(m_pRenderer, m_pBackgroundGame->m_pTexture, nullptr, nullptr);
	SDL_Rect energyBarPosition{ static_cast<int>(static_cast<float>(g_mapWidth / 2) * (1.0f - m_energy)), g_borderBottom, static_cast<int>(g_mapWidth * m_energy), 20};
	SDL_RenderCopy(m_pRenderer, m_pEnergyBar->m_pTexture, nullptr, &energyBarPosition);

	// UI - Score
	unsigned int scoreDigits = 1;
	unsigned int score = m_score;
	score /= 10;

	while (score > 9)
	{
		score /= 10;
		++scoreDigits;
	}

	SDL_Rect currentPosition = { 10, 3, 30, 14 };
	SDL_RenderCopy(m_pRenderer, m_pScoreTitle->m_pTexture, nullptr, &currentPosition);

	if (scoreDigits > 0)
	{
		SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, std::to_string(m_score).c_str(), m_textColor);
		SDL_Texture* pScore = SDL_CreateTextureFromSurface(m_pRenderer, textSurface);

		currentPosition.x += currentPosition.w;
		currentPosition.w = 6 * scoreDigits;

		SDL_RenderCopy(m_pRenderer, pScore, nullptr, &currentPosition);

		SDL_DestroyTexture(pScore);
		SDL_FreeSurface(textSurface);
	}

	// UI - Lives
	currentPosition.x = 320;
	currentPosition.w = 30;

	SDL_RenderCopy(m_pRenderer, m_pPlayerLivesTitle->m_pTexture, nullptr, &currentPosition);

	SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, std::to_string(m_player.GetLives()).c_str(), m_textColor);
	SDL_Texture* pLives = SDL_CreateTextureFromSurface(m_pRenderer, textSurface);

	currentPosition.x += currentPosition.w;
	currentPosition.w = 6;

	SDL_RenderCopy(m_pRenderer, pLives, nullptr, &currentPosition);

	SDL_DestroyTexture(pLives);
	SDL_FreeSurface(textSurface);

	// Tiles
	for (auto& pTile : m_pTiles)
	{
		SDL_RenderCopy(m_pRenderer, pTile->GetTexture(), &pTile->GetSource(), &pTile->GetRenderPosition());
	}

	// Projectiles
	if (!m_pAttachedProjectiles.empty())
	{
		auto& pAttachedProjectile = m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1];
		SDL_RenderCopy(m_pRenderer, pAttachedProjectile->GetTexture(), &pAttachedProjectile->GetSource(), &pAttachedProjectile->GetRenderPosition());
	}
	
	for (auto& pProjectile : m_pProjectiles)
	{
		SDL_RenderCopy(m_pRenderer, pProjectile->GetTexture(), &pProjectile->GetSource(), &pProjectile->GetRenderPosition());
	}

	// Player
	SDL_RenderCopy(m_pRenderer, m_player.GetTexture(), &m_player.GetSource(), &m_player.GetRenderPosition());

	// Send off
	SDL_RenderPresent(m_pRenderer);
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::UpdateObjects(unsigned int const frameTime)
{
	if (m_dominantDirectionKey != SDLK_UNKNOWN)
	{
		m_player.Move(frameTime, m_timeFactor * (m_dominantDirectionKey == SDLK_a) ? -1.0f : 1.0f);
	}
	else if (m_aKeyDown || m_dKeyDown)
	{
		m_player.Move(frameTime, m_timeFactor * (m_aKeyDown) ? -1.0f : 1.0f);
	}

	UpdateProjectiles(frameTime);
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::Input()
{
	SDL_Event event;
	SDL_KeyCode dominantKey = m_dominantDirectionKey;
	bool aDown = m_aKeyDown;
	bool dDown = m_dKeyDown;
	bool wDown = m_wKeyDown;
	bool sDown = m_sKeyDown;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			{
				s_gameState = EGameState::ShutDown;
				break;
			}
		case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_a:
					{
						aDown = true;

						if (dominantKey == SDLK_UNKNOWN)
						{
							dominantKey = SDLK_a;
						}

						break;
					}
				case SDLK_d:
					{
						dDown = true;

						if (dominantKey == SDLK_UNKNOWN)
						{
							dominantKey = SDLK_d;
						}

						break;
					}
				case SDLK_w:
					{
						wDown = true;
						break;
					}
				case SDLK_s:
					{
						sDown = true;
						break;
					}
				default:
					{
						break;
					}
				}

				break;
			}
		case SDL_KEYUP:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_a:
					{
						aDown = false;
						dominantKey = dDown ? SDLK_d : SDLK_UNKNOWN;
						break;
					}
				case SDLK_d:
					{
						dDown = false;
						dominantKey = aDown ? SDLK_a : SDLK_UNKNOWN;
						break;
					}
				case SDLK_w:
					{
						wDown = false;
						m_waitForWKeyUp = false;
						break;
					}
				case SDLK_s:
					{
						sDown = false;
						m_waitForSKeyUp = false;
						break;
					}
				case SDLK_ESCAPE:
					{
						s_gameState = EGameState::GameOver;
						break;
					}
				default:
					{
						break;
					}
				}

				break;
			}
		default:
			{
				break;
			}
		}
	}

	m_dominantDirectionKey = dominantKey;
	m_aKeyDown = aDown;
	m_dKeyDown = dDown;
	m_wKeyDown = wDown;
	m_sKeyDown = sDown;
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::SoftReset()
{
	SDL_Rect projectilePosition = m_player.GetRenderPosition();
	SDL_Rect projectileSource = m_player.GetSource();

	projectilePosition.w = 15;
	projectilePosition.h = 15;
	projectileSource.w = 15;
	projectileSource.h = 15;

	CProjectile* const pProjectile = new CProjectile(projectilePosition, projectileSource);
	pProjectile->SetTexture(m_pRenderer, asset_texture_projectile);
	pProjectile->SetPosition(m_player.GetPosition());
	m_pAttachedProjectiles.push_back(std::unique_ptr<CProjectile>(pProjectile));

	m_pProjectiles.clear();
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::LoadLevel(char const* levelPath)
{
	std::ifstream levelFile(levelPath);

	if (levelFile.is_open())
	{
		for (int line = 0; line < g_levelHeightTiles; ++line)
		{
			for (int index = 0; index < g_levelWidthTiles; ++index)
			{
				int currentTile = 0;
				levelFile >> currentTile;

				if (currentTile)
				{
					int const borderWidth = static_cast<int>(g_tileWidth * 0.5f);
					CTile* pTile = new CTile(SDL_Rect(borderWidth + index * g_tileWidth, (line + 1) * g_tileHeight, g_tileWidth, g_tileHeight), SDL_Rect(0, 0, g_tileWidth, g_tileHeight), static_cast<char>(currentTile));

					pTile->SetTexture(m_pRenderer, (currentTile < 4) ? asset_tileTextures[currentTile] : asset_tileTextures[3]);
					m_pTiles.push_back(std::unique_ptr<CTile>(pTile));

					m_level[line][index] = std::make_tuple(static_cast<unsigned char>(currentTile), pTile);
				}

			}
		}
	}

	levelFile.close();
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::UpdateProjectiles(unsigned int const frameTime)
{
	if (!m_roundStarted)
	{
		// just wait for key down to start the round
		if (m_wKeyDown)
		{
			auto& pDetatchProjectile = m_pAttachedProjectiles.back();
			pDetatchProjectile->ReleaseFromPlayer();
			m_pProjectiles.push_back(std::unique_ptr<CProjectile>(pDetatchProjectile.release()));
			m_pAttachedProjectiles.pop_back();

			m_waitForWKeyUp = true;
			m_roundStarted = true;

			if (!m_pAttachedProjectiles.empty())
			{
				Vec2D pos = m_player.GetPosition();
				pos.x += 23.0f;
				pos.y -= 15.0f;
				m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1]->SetPosition(pos);
			}
		}
		else
		{
			Vec2D pos = m_player.GetPosition();
			pos.x += 23.0f;
			pos.y -= 15.0f;
			m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1]->SetPosition(pos);
		}
	}
	else
	{
		if (!m_pAttachedProjectiles.empty())
		{
			// TODO: haven't checked if key down events are being repeated while a key stays pressed
			// Detach
			if (!m_waitForWKeyUp && m_wKeyDown)
			{
				auto& pDetatchProjectile = m_pAttachedProjectiles.back();
				pDetatchProjectile->ReleaseFromPlayer();
				m_pProjectiles.push_back(std::unique_ptr<CProjectile>(pDetatchProjectile.release()));
				m_pAttachedProjectiles.pop_back();

				m_waitForWKeyUp = true;

				if (!m_pAttachedProjectiles.empty())
				{
					Vec2D pos = m_player.GetPosition();
					pos.x += 23.0f;
					pos.y -= 15.0f;
					m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1]->SetPosition(pos);
				}
			}
			else // Update Rendering on the next attached bullet
			{
				Vec2D pos = m_player.GetPosition();
				pos.x += 23.0f;
				pos.y -= 15.0f;
				m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1]->SetPosition(pos);
			}
		}

		// Update positions and check for collisions. This is where I keep my boilerplate for now.
		if (!m_pProjectiles.empty())
		{
			Vec2D playerPosition = m_player.GetPosition();

			for (int i = static_cast<int>(m_pProjectiles.size() - 1); i >= 0; --i)
			{
				auto& pProjectile = m_pProjectiles[i];
				pProjectile->UpdatePosition(frameTime, m_timeFactor);

				SDL_Rect const& projectilePosition = pProjectile->GetRenderPosition();

				// collision with borders take precedence
				if ((projectilePosition.y + projectilePosition.h) >= g_borderBottom)
				{
					m_pProjectiles.erase((m_pProjectiles.begin() + i));

					if (m_pProjectiles.empty())
					{
						if (!m_player.Damage())
						{
							if (m_player.GetLives() == 1)
							{
								m_pAudioManager->SetMusic(Arkanoid::Audio::EMusic::Tension);
							}

							SoftReset();
						}
						else
						{
							m_pAudioManager->SetFilterAmount(0.0f);
							s_gameState = EGameState::GameOver;
						}

						m_pAudioManager->Play(asset_audio_playerLoseLife, true, { static_cast<float>(projectilePosition.x - (playerPosition.x + 30.0f)), static_cast<float>(projectilePosition.y) });
					}
				}
				else if (projectilePosition.y <= g_borderTop)
				{
					Vec2D const& dir = pProjectile->GetDirection();
					pProjectile->SetDirection(dir.x, -std::abs(dir.y));

					m_pAudioManager->Play(asset_audio_wallImpact, true, { static_cast<float>(projectilePosition.x - (playerPosition.x + 30.0f)), static_cast<float>(projectilePosition.y) });
				}
				else if (projectilePosition.x <= g_borderLeft)
				{
					Vec2D const& dir = pProjectile->GetDirection();
					pProjectile->SetDirection(std::abs(dir.x), dir.y);

					m_pAudioManager->Play(asset_audio_wallImpact, true, { static_cast<float>(projectilePosition.x - (playerPosition.x + 30.0f)), static_cast<float>(projectilePosition.y) });
				}
				else if ((projectilePosition.x + projectilePosition.w) >= g_borderRight)
				{
					Vec2D const& dir = pProjectile->GetDirection();
					pProjectile->SetDirection(-std::abs(dir.x), dir.y);

					m_pAudioManager->Play(asset_audio_wallImpact, true, { static_cast<float>(projectilePosition.x - (playerPosition.x + 30.0f)), static_cast<float>(projectilePosition.y) });
				}
				else if ((projectilePosition.y + projectilePosition.h) >= g_playerHeight && (projectilePosition.y + projectilePosition.h) < (g_playerHeight + projectilePosition.h)) // check collisions for player and enemies
				{
					SDL_Rect playerPos = m_player.GetRenderPosition();

					if ((projectilePosition.x > (playerPos.x - projectilePosition.w)) && (projectilePosition.x < (playerPos.x + playerPos.w)))
					{
						Vec2D const& dir = pProjectile->GetDirection();

						float const playerCenter = (static_cast<float>(playerPos.x) + static_cast<float>(playerPos.w) * 0.5f);
						float const projectileCenter = (static_cast<float>(projectilePosition.x) + static_cast<float>(projectilePosition.h) * 0.5f);
						float horizontalSpeedFactor = -(playerCenter - projectileCenter) / (static_cast<float>(playerPos.w) * 0.5f);

						pProjectile->SetDirection(g_projectileSpeed * horizontalSpeedFactor, std::abs(dir.y));
						pProjectile->ResetCollisionCounter();

						m_pAudioManager->Play(asset_audio_playerImpact, true, { static_cast<float>(projectilePosition.x - (playerPosition.x + 30.0f)), static_cast<float>(projectilePosition.y) });
					}
					else // TODO: Enemies
					{
					}
				}
				else // collisions with Tiles
				{
					Vec2D const& dir = pProjectile->GetDirection();

					char index1 = 0;
					char index2 = 0;
					char row1 = 0;
					char row2 = 0;

					short const projectileCenterX = static_cast<short>(projectilePosition.x + (projectilePosition.w / 2));
					short const projectileCenterY = static_cast<short>(projectilePosition.y + (projectilePosition.h / 2));

					if (dir.x > 0.0f)
					{
						if (dir.y > 0.0f) // above and right
						{
							row1 = static_cast<char>((projectileCenterY - g_borderTop) / g_tileHeight) - 1;
							index1 = static_cast<char>((projectileCenterX - g_borderLeft) / g_tileWidth);
							row2 = row1 + 1;
							index2 = index1 + 1;
						}
						else // bottom and right
						{
							row2 = static_cast<char>((projectileCenterY - g_borderTop) / g_tileHeight);
							index1 = static_cast<char>((projectileCenterX - g_borderLeft) / g_tileWidth);
							row1 = row2 + 1;
							index2 = index1 + 1;
						}
					}
					else
					{
						if (dir.y > 0.0f) // above and left
						{
							row1 = static_cast<char>((projectileCenterY - g_borderTop) / g_tileHeight) - 1;
							index1 = static_cast<char>((projectileCenterX - g_borderLeft) / g_tileWidth);
							row2 = row1 + 1;
							index2 = index1 - 1;
						}
						else // bottom and left
						{
							row2 = static_cast<char>((projectileCenterY - g_borderTop) / g_tileHeight);
							index1 = static_cast<char>((projectileCenterX - g_borderLeft) / g_tileWidth);
							row1 = row2 + 1;
							index2 = index1 - 1;
						}
					}

					if (index1 >= g_levelWidthTiles) index1 = g_levelWidthTiles - 1;
					if (index1 < 0) index1 = 0;
					if (index2 >= g_levelWidthTiles) index2 = g_levelWidthTiles - 1;
					if (index2 < 0) index2 = 0;
					if (row1 >= g_levelHeightTiles) row1 = g_levelHeightTiles - 1;
					if (row1 < 0) row1 = 0;
					if (row2 >= g_levelHeightTiles) row2 = g_levelHeightTiles - 1;
					if (row2 < 0) row2 = 0;

					CTile* pTile1 = nullptr; // Top Bottom
					CTile* pTile2 = nullptr; // Left Right
					CTile* pTile3 = nullptr; // Corner

					if ((index1 != index2) || (row1 != row2))
					{
						pTile1 = std::get<1>(m_level[row1][index1]);
						pTile2 = std::get<1>(m_level[row2][index2]);
						pTile3 = std::get<1>(m_level[row1][index2]);
					}
					else
					{
						pTile1 = std::get<1>(m_level[row1][index1]);
						pTile3 = std::get<1>(m_level[row1][index2]);
					}

					if (pTile1 != nullptr && pProjectile->Collision(pTile1->GetRenderPosition()))
					{
						for (int j = static_cast<int>(m_pTiles.size() - 1); j >= 0; --j)
						{
							if (&*m_pTiles[j] == pTile1)
							{
								if (pTile1->Damage(m_pRenderer))
								{
									m_pTiles.erase(m_pTiles.begin() + j);
									std::get<1>(m_level[row1][index1]) = nullptr;

									m_score += g_score_tileDestroyed;
								}
								else
								{
									m_score += g_score_tileDamaged;
								}
							
								m_pAudioManager->Play(asset_audio_tileCollisions[pProjectile->GetCollisionCount()], true, { static_cast<float>(projectilePosition.x - (playerPosition.x + 30.0f)), static_cast<float>(projectilePosition.y) });
								pProjectile->IncrementCollisionCounter();

								break;
							}
						}
					}
					else if (pTile2 != nullptr && pProjectile->Collision(pTile2->GetRenderPosition()))
					{
						for (int j = static_cast<int>(m_pTiles.size() - 1); j >= 0; --j)
						{
							if (&*m_pTiles[j] == pTile2)
							{
								if (pTile2->Damage(m_pRenderer))
								{
									m_pTiles.erase(m_pTiles.begin() + j);
									std::get<1>(m_level[row2][index2]) = nullptr;

									m_score += g_score_tileDestroyed;
								}
								else
								{
									m_score += g_score_tileDamaged;
								}
								
								m_pAudioManager->Play(asset_audio_tileCollisions[pProjectile->GetCollisionCount()], true, { static_cast<float>(projectilePosition.x - (playerPosition.x + 30.0f)), static_cast<float>(projectilePosition.y) });
								pProjectile->IncrementCollisionCounter();

								break;
							}
						}
					}
					else if (pTile3 != nullptr && pProjectile->Collision(pTile3->GetRenderPosition()))
					{
						for (int j = static_cast<int>(m_pTiles.size() - 1); j >= 0; --j)
						{
							if (&*m_pTiles[j] == pTile3)
							{
								if (pTile3->Damage(m_pRenderer))
								{
									m_pTiles.erase(m_pTiles.begin() + j);
									std::get<1>(m_level[row2][index2]) = nullptr;

									m_score += g_score_tileDestroyed;
								}
								else
								{
									m_score += g_score_tileDamaged;
								}

								m_pAudioManager->Play(asset_audio_tileCollisions[pProjectile->GetCollisionCount()], true, { static_cast<float>(projectilePosition.x - (playerPosition.x + 30.0f)), static_cast<float>(projectilePosition.y) });
								pProjectile->IncrementCollisionCounter();

								break;
							}
						}
					}
				}
			}
		}
	}
}
} // Arkanoid::Game
