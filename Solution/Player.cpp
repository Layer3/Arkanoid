#include "Player.h"

namespace Arkanoid::Game
{
//////////////////////////////////////////////////////////////////////////////////
void CPlayer::Move(unsigned int const frameTime, float const modifier)
{
	int const distanceToMove = static_cast<int>(((static_cast<float>(frameTime) / 1000.0f) * g_playerSpeed) * modifier); // TODO: frametime as 1/sec maybe
	m_positionXY.x += distanceToMove;

	if (m_positionXY.x < g_borderLeft)
	{
		m_positionXY.x = g_borderLeft;
	}
	else if (m_positionXY.x > (g_borderRight - m_playerWidth))
	{
		m_positionXY.x = (g_borderRight - m_playerWidth);
	}

	SetRenderPostitionX(m_positionXY.x);
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
} // Arkanoid::Game
