#include "Tile.h"

namespace Arkanoid::Game
{
//////////////////////////////////////////////////////////////////////////////////
bool CTile::Damage(SDL_Renderer* const pRenderer)
{
	bool destroyed = false;

	if (--m_hp <= 0)
	{
		destroyed = true;
	}

	SetTexture(pRenderer, (m_hp < 4) ? asset_tileTextures[m_hp] : asset_tileTextures[3]);

	return destroyed;	
}
}// namespace Arkanoid::Game
