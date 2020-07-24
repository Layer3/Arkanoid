#include "Projectile.h"
#include "AudioManager.h"

namespace Arkanoid::Game
{
//////////////////////////////////////////////////////////////////////////////////
void CProjectile::SetDirection(float const x, float const y)
{
	m_directionXY.x = x;
	m_directionXY.y = y;
}

//////////////////////////////////////////////////////////////////////////////////
void CProjectile::ReleaseFromPlayer()
{
	m_velocity = 1.0f;
	m_directionXY.y = g_projectileSpeed;
}

//////////////////////////////////////////////////////////////////////////////////
bool CProjectile::Collision(SDL_Rect const& rect, Arkanoid::Audio::CAudioManager* pAudioManager)
{
	SDL_Rect projRect = GetRenderPosition();
	short const projectileCenterX = projRect.x + (projRect.w / 2);
	short const projectileCenterY = projRect.y + (projRect.h / 2);

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

	if (collisionValid)
	{
		pAudioManager->Play(asset_audio_tileDestroy, true, { static_cast<float>(projRect.x), static_cast<float>(projRect.y) });
	}

	return collisionValid;
}

//////////////////////////////////////////////////////////////////////////////////
void CProjectile::UpdatePosition(unsigned int const frameTime)
{
	float const frameMovementFactor = (static_cast<float>(frameTime) / 1000.0f);
	m_position.x += static_cast<short>(frameMovementFactor * m_directionXY.x);
	m_position.y -= static_cast<short>(frameMovementFactor * m_directionXY.y); // positive y is down

	SetRenderPosition(m_position);
}

//////////////////////////////////////////////////////////////////////////////////
void CProjectile::SetPosition(Pos2D const& position)
{
	m_position = position;
	SetRenderPosition(position);
}
} // namespace Arkanoid::Game
