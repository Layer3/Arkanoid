#include "Tile.h"

namespace Arkanoid::Game
{
//////////////////////////////////////////////////////////////////////////////////
bool CTile::Damage()
{
	bool destroyed = false;

	if (--m_hp <= 0)
	{
		destroyed = true;
	}

	return destroyed;	
}
}// namespace Arkanoid::Game
