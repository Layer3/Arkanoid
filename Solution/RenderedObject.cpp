#include "RenderedObject.h"
#include <SDL_image.h>

namespace Arkanoid::Game
{
//////////////////////////////////////////////////////////////////////////////////
void CRenderedObject::SetRenderPosition(Pos2D const& pos)
{
	m_position.x = pos.x;
	m_position.y = pos.y;
}

//////////////////////////////////////////////////////////////////////////////////
void CRenderedObject::SetTexture(SDL_Renderer* const pRenderer, char const* const filePath)
{
	m_pTexture.reset(new SCustomTexture(IMG_LoadTexture(pRenderer, filePath)));
}
} // namespace Arkanoid::Game
