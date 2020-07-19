#pragma once

#include "RenderedObject.h"
#include <SDL_render.h>

namespace Arkanoid::Game
{
class CTile final : public CRenderedObject
{
public:

	CTile(SDL_Rect const& position, SDL_Rect const& source, char const hp)
		: CRenderedObject(position, source)
		, m_hp(hp)
	{
	}

	bool Damage();

private:

	char m_hp;
};
} // namespace Arkanoid::Game
