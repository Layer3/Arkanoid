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
void CRenderedObject::SetTexture(SDL_Renderer* const pRenderer, const char* const filePath)
{
	m_pTexture = IMG_LoadTexture(pRenderer, filePath);
}
} // namespace Arkanoid::Game
