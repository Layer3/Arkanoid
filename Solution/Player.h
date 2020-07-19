#pragma once

#include "RenderedObject.h"
#include "Constexpr.h"

namespace Arkanoid::Game
{
class CPlayer final : public CRenderedObject
{
public:

	// TODO: Construction is ugly
	CPlayer()
		: CRenderedObject(SDL_Rect(Arkanoid::g_mapWidth / 2, Arkanoid::g_mapHeight - (Arkanoid::Game::g_tileHeight * 2), 60, 20), SDL_Rect(0, 0, 60, 20)) // create player one tile-height above bottom border and in center
		, m_positionXY{ Arkanoid::g_mapWidth / 2, Arkanoid::g_mapHeight - (Arkanoid::Game::g_tileHeight * 2) }
		, m_playerWidth(60)
		, m_lives(3)
	{}

	void Move(unsigned int const frameTime, float const modifier);
	Pos2D const& GetPosition() const { return m_positionXY; }

	char const& Lives() const { return m_lives; }
	bool Damage();

private:

	Pos2D m_positionXY;
	short m_playerWidth;
	char m_lives;
};
} // namespace Arkanoid::Games