#include "Projectile.h"

namespace Arkanoid::Game
{
//////////////////////////////////////////////////////////////////////////////////
void CProjectile::SetDirection(float const x, float const y)
{
	m_directionXY.x = x;
	m_directionXY.y = y;

	if (m_velocity < g_maxVelocity)
	{
		m_velocity += 0.01f;
	}
}

//////////////////////////////////////////////////////////////////////////////////
void CProjectile::ReleaseFromPlayer()
{
	m_velocity = 1.0f;
	m_directionXY.y = g_projectileSpeed;
}

//////////////////////////////////////////////////////////////////////////////////
bool CProjectile::Collision(SDL_Rect const& rect)
{
	SDL_Rect projRect = GetRenderPosition();
	short const projectileCenterX = static_cast<short>(projRect.x + (projRect.w / 2));
	short const projectileCenterY = static_cast<short>(projRect.y + (projRect.h / 2));

	bool collisionValid = false;

	// Top/Bottom/Left/Right collisions
	if ((projectileCenterX >= rect.x) && (projectileCenterX <= rect.x + rect.w))
	{
		if ((projRect.y + projRect.h >= rect.y) && (projRect.y <= rect.y + rect.h))
		{
			m_directionXY.y = -m_directionXY.y;
			collisionValid = true;
		}
	}
	else if ((projectileCenterY > rect.y) && (projectileCenterY < rect.y + rect.h))
	{
		if ((projRect.x <= rect.x + rect.w) && (projRect.x + projRect.w >= rect.x))
		{
			m_directionXY.x = -m_directionXY.x;
			collisionValid = true;
		}
	}
	else // Corners
	{
		if ((projRect.x + projRect.w > rect.x + rect.w) && (projRect.x <= rect.x + rect.w)) // right corners
		{
			if ((projRect.y < rect.y + rect.h) && (projRect.y + projRect.h > rect.y + rect.h))
			{
				m_directionXY.x = g_projectileSpeed;
				m_directionXY.y = g_projectileSpeed;
				collisionValid = true;
			}
			else if ((projRect.y < rect.y) && (projRect.y + projRect.h > rect.y))
			{
				m_directionXY.x = g_projectileSpeed;
				m_directionXY.y = -g_projectileSpeed;
				collisionValid = true;
			}
		}
		else if ((projRect.x < rect.x) && (projRect.x + projRect.w >= rect.x))
		{
			if ((projRect.y < rect.y + rect.h) && (projRect.y + projRect.h > rect.y + rect.h))
			{
				m_directionXY.x = -g_projectileSpeed;
				m_directionXY.y = g_projectileSpeed;
				collisionValid = true;
			}
			else if ((projRect.y < rect.y) && (projRect.y + projRect.h > rect.y))
			{
				m_directionXY.x = -g_projectileSpeed;
				m_directionXY.y = -g_projectileSpeed;
				collisionValid = true;
			}
		}
	}

	return collisionValid;
}

//////////////////////////////////////////////////////////////////////////////////
void CProjectile::UpdatePosition(unsigned int const frameTime, float const modifier)
{
	float const frameMovementFactor = (static_cast<float>(frameTime) / 1000.0f) * modifier;
	m_position.x += frameMovementFactor * m_directionXY.x;
	m_position.y -= frameMovementFactor * m_directionXY.y; // positive y is down

	SetRenderPosition(Pos2D{ static_cast<short>(m_position.x), static_cast<short>(m_position.y) });
}

//////////////////////////////////////////////////////////////////////////////////
void CProjectile::IncrementCollisionCounter()
{
	if (++m_collisionCount >= g_numCollisionSounds)
	{
		m_collisionCount = (g_numCollisionSounds - 1);
	}
}

//////////////////////////////////////////////////////////////////////////////////
void CProjectile::SetPosition(Vec2D const& position)
{
	m_position = position;
	SetRenderPosition(Pos2D{ static_cast<short>(m_position.x), static_cast<short>(m_position.y) });
}
} // namespace Arkanoid::Game
