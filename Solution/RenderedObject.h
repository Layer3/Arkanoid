#pragma once

#include <SDL_render.h>
#include "Constexpr.h"

namespace Arkanoid::Game
{
class CRenderedObject
{
public:

	CRenderedObject() = delete;

	CRenderedObject(SDL_Rect const& position, SDL_Rect const& source)
		: m_position(position)
		, m_source(source)
		, m_pTexture(nullptr)
	{
	}

	SDL_Rect const GetRenderPosition() const         { return m_position; }
	void           SetRenderPosition(Pos2D const& pos);
	void           SetRenderPostitionX(int const& x) { m_position.x = x; } // convenience for player

	SDL_Rect const GetSource() const { return m_source; }

	void               SetTexture(SDL_Renderer* const pRenderer, const char* const filePath);
	SDL_Texture* const GetTexture() const { return m_pTexture; }

private:

	SDL_Rect m_position;
	SDL_Rect m_source;
	SDL_Texture* m_pTexture;
};
} // namespace Arkanoid::Game