#pragma once
#include "RenderedObject.h"

namespace Arkanoid::Game
{
class CTile final : public CRenderedObject
{
public:

	CTile(SDL_Rect const& position, SDL_Rect const& source, char const hp)
		: CRenderedObject(position, source)
		, m_hp(hp)
	{}

	~CTile() = default;

	bool Damage(SDL_Renderer* const pRenderer);

private:

	char m_hp;
};
} // namespace Arkanoid::Game
