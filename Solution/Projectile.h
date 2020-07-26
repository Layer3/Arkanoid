#pragma once
#include "Global.h"
#include "RenderedObject.h"

namespace Arkanoid::Audio
{
class CAudioManager;
}

namespace Arkanoid::Game
{
class CProjectile final : public CRenderedObject
{
public:

	CProjectile(SDL_Rect const& position, SDL_Rect const& source)
		: CRenderedObject(position, source)
		, m_attached(true)
		, m_velocity(0.0f)
		, m_directionXY(0.0f, 0.0f)
		, m_position(static_cast<float>(position.x), static_cast<float>(position.y))
		, m_collisionCount(0)
	{}

	Vec2D const& GetPosition() const { return m_position; }
	void         SetPosition(Vec2D const& position);
	
	Vec2D const& GetDirection() const { return m_directionXY; }
	void         SetDirection(float const x, float const y);

	void ReleaseFromPlayer();
	bool Collision(SDL_Rect const& rect);
	void UpdatePosition(unsigned int const frameTime, float const modifier);

	void                IncrementCollisionCounter();
	void                ResetCollisionCounter() { m_collisionCount = 0; }
	unsigned char const GetCollisionCount() const { return m_collisionCount; }

private:

	bool          m_attached;
	float         m_velocity;
	Vec2D         m_directionXY;
	Vec2D         m_position;
	unsigned char m_collisionCount;
};
} // namespace Arkanoid::Game
