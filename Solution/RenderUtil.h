#pragma once
#include <SDL_render.h>

// intentionally global
struct SCustomTexture
{
	SCustomTexture()
		: m_pTexture(nullptr)
	{}

	SCustomTexture(SDL_Texture* pTexture)
		: m_pTexture(pTexture)
	{}

	SCustomTexture(SCustomTexture& customTexture)
		: m_pTexture(customTexture.m_pTexture)
	{}

	void operator delete(void* p)
	{
		if (p != nullptr)
		{
			SDL_DestroyTexture(static_cast<SCustomTexture*>(p)->m_pTexture);
			delete p;
		}
	}

	SDL_Texture* m_pTexture;
};
