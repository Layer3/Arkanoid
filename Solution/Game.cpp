#include "Game.h"
#include "AudioManager.h"
#include "Projectile.h"
#include "Tile.h"
#include <cmath>
#include <fstream>
#include <SDL_image.h>

namespace Arkanoid::Game
{
//////////////////////////////////////////////////////////////////////////////////
CGame::~CGame()
{
	for (CTile* pTile : m_pTiles)
	{
		delete pTile;
	}

	m_pTiles.clear();

	for (CProjectile* pProjectile : m_pAttachedProjectiles)
	{
		delete pProjectile;
	}

	m_pAttachedProjectiles.clear();

	for (CProjectile* pProjectile : m_pProjectiles)
	{
		delete pProjectile;
	}

	m_pProjectiles.clear();
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::Initialize(SDL_Renderer* const pRenderer, Arkanoid::Audio::CAudioManager* pAudioManager, const char* const levelAsset)
{
	m_pRenderer = pRenderer;
	m_pAudioManager = pAudioManager;
	m_pBackgroundGame = IMG_LoadTexture(m_pRenderer, asset_texture_backgroundGame);
	LoadLevel(levelAsset);
	
	m_player.SetTexture(m_pRenderer, asset_texture_player);

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
	m_pAttachedProjectiles.push_back(pProjectile);
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::Update(unsigned int const frameTime)
{
	Input();
	UpdateObjects(frameTime);
	Render();
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::Reset()
{
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::Render()
{
	// Background
	SDL_RenderCopy(m_pRenderer, m_pBackgroundGame, nullptr, nullptr);

	// Tiles
	for (CTile* pTile : m_pTiles)
	{
		SDL_RenderCopy(m_pRenderer, pTile->GetTexture(), &pTile->GetSource(), &pTile->GetRenderPosition());
	}

	// Projectiles
	if (!m_pAttachedProjectiles.empty())
	{
		CProjectile* pAttachedProjectile = m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1];
		SDL_RenderCopy(m_pRenderer, pAttachedProjectile->GetTexture(), &pAttachedProjectile->GetSource(), &pAttachedProjectile->GetRenderPosition());
	}
	
	for (CProjectile* pProjectile : m_pProjectiles)
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
		m_player.Move(frameTime, (m_dominantDirectionKey == SDLK_a) ? -1.0f : 1.0f);
	}
	else if (m_aKeyDown || m_dKeyDown)
	{
		m_player.Move(frameTime, (m_aKeyDown) ? -1.0f : 1.0f);
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
				if (dominantKey == SDLK_UNKNOWN)
				{
					if (event.key.keysym.sym == SDLK_a)
					{
						aDown = true;
						dominantKey = SDLK_a;
					}
					else if (event.key.keysym.sym == SDLK_d)
					{
						dominantKey = SDLK_d;
						dDown = true;
					}
				}
				else
				{
					if (event.key.keysym.sym == SDLK_a)
					{
						aDown = true;
					}
					else if (event.key.keysym.sym == SDLK_d)
					{
						dDown = true;
					}
				}

				if (event.key.keysym.sym == SDLK_w)
				{
					wDown = true;
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
						m_waitForWKeyDown = false;
						break;
					}
				case SDLK_s:
					{
						break;
					}
				case SDLK_ESCAPE:
					{
						s_gameState = EGameState::Paused;
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
	m_pAttachedProjectiles.push_back(pProjectile);
}

//////////////////////////////////////////////////////////////////////////////////
void CGame::LoadLevel(const char* levelPath)
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
					CTile* const pTile = new CTile(SDL_Rect(borderWidth + index * g_tileWidth, (line + 1) * g_tileHeight, g_tileWidth, g_tileHeight), SDL_Rect(0, 0, g_tileWidth, g_tileHeight), currentTile);

					pTile->SetTexture(m_pRenderer, (currentTile < 4) ? asset_tileTextures[currentTile] : asset_tileTextures[3]);
					m_pTiles.push_back(pTile);

					m_level[line][index] = std::make_tuple(currentTile, pTile);
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
			CProjectile* pDetatchProjectile = m_pAttachedProjectiles.back();
			pDetatchProjectile->ReleaseFromPlayer();
			m_pProjectiles.push_back(pDetatchProjectile);
			m_pAttachedProjectiles.pop_back();

			m_waitForWKeyDown = true;
			m_roundStarted = true;

			// TODO: Delete. Only necessary for debugging, or if player has multiple projectiles from beginning.
			if (!m_pAttachedProjectiles.empty())
			{
				Pos2D pos = m_player.GetPosition();
				pos.x += 23;
				pos.y -= 15;
				m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1]->SetPosition(pos);
			}
		}
		else
		{
			Pos2D pos = m_player.GetPosition();
			pos.x += 23;
			pos.y -= 15;
			m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1]->SetPosition(pos);
		}
	}
	else
	{
		if (!m_pAttachedProjectiles.empty())
		{
			// TODO: haven't checked if key down events are being repeated while a key stays pressed
			// Detach
			if (!m_waitForWKeyDown && m_wKeyDown)
			{
				CProjectile* pDetatchProjectile = m_pAttachedProjectiles.back();
				pDetatchProjectile->ReleaseFromPlayer();
				m_pProjectiles.push_back(pDetatchProjectile);
				m_pAttachedProjectiles.pop_back();

				m_waitForWKeyDown = true;

				if (!m_pAttachedProjectiles.empty())
				{
					Pos2D pos = m_player.GetPosition();
					pos.x += 23;
					pos.y -= 15;
					m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1]->SetPosition(pos);
				}
			}
			else // Update Rendering on the next attached bullet
			{
				Pos2D pos = m_player.GetPosition();
				pos.x += 23;
				pos.y -= 15;
				m_pAttachedProjectiles[m_pAttachedProjectiles.size() - 1]->SetPosition(pos);
			}
		}

		// Update positions and check for collisions. This is where i keep my boilerplate for now
		if (!m_pProjectiles.empty())
		{
			for (int i = static_cast<int>(m_pProjectiles.size() - 1); i >= 0; --i)
			{
				bool didCollide = false;

				CProjectile* pProjectile = m_pProjectiles[i];
				pProjectile->UpdatePosition(frameTime);

				SDL_Rect const& pos = pProjectile->GetRenderPosition();

				// collision with borders take precedence
				if ((pos.y + pos.h) >= g_borderBottom)
				{
					delete pProjectile;
					m_pProjectiles.erase((m_pProjectiles.begin() + i));

					if (!m_player.Damage())
					{
						SoftReset();
					}
					else
					{
						m_gameRunning = false;
					}
				}
				else if (pos.y <= g_borderTop)
				{
					Vec2D const& dir = pProjectile->GetDirection();
					pProjectile->SetDirection(dir.x, -std::abs(dir.y));

					didCollide = true;
				}
				else if (pos.x <= g_borderLeft)
				{
					Vec2D const& dir = pProjectile->GetDirection();
					pProjectile->SetDirection(std::abs(dir.x), dir.y);

					didCollide = true;
				}
				else if ((pos.x + pos.w) >= g_borderRight)
				{
					Vec2D const& dir = pProjectile->GetDirection();
					pProjectile->SetDirection(-std::abs(dir.x), dir.y);

					didCollide = true;
				}
				else if ((pos.y + pos.h) >= g_playerHeight && (pos.y + pos.h) < (g_playerHeight + pos.h)) // check collisions for player and enemies
				{
					SDL_Rect playerPos = m_player.GetRenderPosition();

					if ((pos.x > (playerPos.x - pos.w)) && (pos.x < (playerPos.x + playerPos.w)))
					{
						Vec2D const& dir = pProjectile->GetDirection();

						float const playerCenter = (static_cast<float>(playerPos.x) + static_cast<float>(playerPos.w) * 0.5f);
						float const projectileCenter = (static_cast<float>(pos.x) + static_cast<float>(pos.h) * 0.5f);
						float horizontalSpeedFactor = -(playerCenter - projectileCenter) / (static_cast<float>(playerPos.w) * 0.5f);

						pProjectile->SetDirection(g_projectileSpeed * horizontalSpeedFactor, std::abs(dir.y));

						didCollide = true;
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

					short projectileCenterX = pos.x + (pos.w / 2);
					short projectileCenterY = pos.y + (pos.h / 2);

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

					// TODO: this is ugly
					if (pTile1 != nullptr && pProjectile->Collision(pTile1->GetRenderPosition(), m_pAudioManager))
					{
						for (int i = static_cast<int>(m_pTiles.size() - 1); i >= 0; --i)
						{
							if (m_pTiles[i] == pTile1)
							{
								if (pTile1->Damage(m_pRenderer))
								{
									m_pTiles.erase(m_pTiles.begin() + i);

									delete pTile1;
									std::get<1>(m_level[row1][index1]) = nullptr;
								}
							
								break;
							}
						}
					}
					else if (pTile2 != nullptr && pProjectile->Collision(pTile2->GetRenderPosition(), m_pAudioManager))
					{
						for (int i = static_cast<int>(m_pTiles.size() - 1); i >= 0; --i)
						{
							if (m_pTiles[i] == pTile2)
							{
								if (pTile2->Damage(m_pRenderer))
								{
									m_pTiles.erase(m_pTiles.begin() + i);

									delete pTile2;
									std::get<1>(m_level[row2][index2]) = nullptr;
								}
								
								break;
							}
						}
					}
					else if (pTile3 != nullptr && pProjectile->Collision(pTile3->GetRenderPosition(), m_pAudioManager))
					{
						for (int i = static_cast<int>(m_pTiles.size() - 1); i >= 0; --i)
						{
							if (m_pTiles[i] == pTile3)
							{
								if (pTile3->Damage(m_pRenderer))
								{
									m_pTiles.erase(m_pTiles.begin() + i);

									delete pTile3;
									std::get<1>(m_level[row2][index2]) = nullptr;
								}

								break;
							}
						}
					}
				}

				if (didCollide)
				{
					m_pAudioManager->Play(asset_audio_projectileCollision1, true, { static_cast<float>(pos.x), static_cast<float>(pos.y) });
				}
			}
		}
	}
}
} // Arkanoid::Game
