#pragma once

#include "Global.h"
#include "RenderUtil.h"
#include <memory>

namespace Arkanoid::Game
{
class CRenderedObject
{
public:

	explicit CRenderedObject(SDL_Rect const& position, SDL_Rect const& source)
		: m_position(position)
		, m_source(source)
		, m_pTexture()
	{}

	virtual ~CRenderedObject() = default;

	SDL_Rect const& GetRenderPosition() const { return m_position; }
	void			SetRenderPosition(Pos2D const& pos);
	void			SetRenderPostitionX(int const& x) { m_position.x = x; } // convenience for player

	SDL_Rect const& GetSource() const { return m_source; }

	void               SetTexture(SDL_Renderer* const pRenderer, char const* const filePath);
	SDL_Texture* const GetTexture() const { return (*m_pTexture).m_pTexture; }

private:

	SDL_Rect m_position;
	SDL_Rect m_source;
	std::unique_ptr<SCustomTexture> m_pTexture;
};
} // namespace Arkanoid::Game
