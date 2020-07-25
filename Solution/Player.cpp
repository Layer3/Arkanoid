#include "Player.h"

namespace Arkanoid::Game
{
//////////////////////////////////////////////////////////////////////////////////
void CPlayer::Move(unsigned int const frameTime, float const modifier)
{
	float const distanceToMove = ((static_cast<float>(frameTime) / 1000.0f) * g_playerSpeed) * modifier; // TODO: frametime as 1/sec maybe
	m_positionXY.x += distanceToMove;

	if (m_positionXY.x < static_cast<float>(g_borderLeft))
	{
		m_positionXY.x = static_cast<float>(g_borderLeft);
	}
	else if (m_positionXY.x > static_cast<float>(g_borderRight - m_playerWidth))
	{
		m_positionXY.x = static_cast<float>(g_borderRight - m_playerWidth);
	}

	SetRenderPostitionX(static_cast<int>(m_positionXY.x));
}

//////////////////////////////////////////////////////////////////////////////////
bool CPlayer::Damage()
{
	bool isDead = false;

	if (--m_lives <= 0)
	{
		isDead = true;
	}
	
	return isDead;
}

//////////////////////////////////////////////////////////////////////////////////
void CPlayer::Reset()
{
	m_positionXY = { static_cast<float>(Arkanoid::g_mapWidth / 2), static_cast<float>(Arkanoid::g_mapHeight - (Arkanoid::Game::g_tileHeight * 2)) };
	SetRenderPosition( {static_cast<short>(m_positionXY.x), static_cast<short>(m_positionXY.y) });
	m_lives = 3;
}
} // Arkanoid::Game
