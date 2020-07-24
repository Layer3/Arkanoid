#include "AudioManager.h"
#include "Constexpr.h"
#include "RenderedObject.h"
#include <complex>

namespace Arkanoid::Audio
{
//////////////////////////////////////////////////////////////////////////////////
static int StreamCallback(
	void const* pInputBuffer,
	void* pOutputBuffer,
	long unsigned framesPerBuffer,
	PaStreamCallbackTimeInfo const* pTimeInfo,
	PaStreamCallbackFlags statusFlags,
	void* pUserData)
{
	auto const pAudioManager = static_cast<CAudioManager*>(pUserData);

	if (pOutputBuffer != nullptr)
	{
		pAudioManager->RenderAudio(pOutputBuffer);

		return PaStreamCallbackResult::paContinue;
	}

	return PaStreamCallbackResult::paAbort;
}

//////////////////////////////////////////////////////////////////////////////////
CAudioManager::CAudioManager()
{ //TODO: This should all live in a general initialize
	m_numChannels = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->maxOutputChannels;
	m_sampleRate = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->defaultSampleRate;
	m_bufferLength = 2048;
	m_pOutputBuffer = new Arkanoid::Audio::SAudioBuffer(m_sampleRate, m_bufferLength, m_numChannels, nullptr);
	m_pMixer = new Arkanoid::Audio::CAudioMixer(m_sampleRate, m_bufferLength, m_numChannels);

	Pa_OpenDefaultStream(
		&m_pStream,
		0,
		m_numChannels,
		paFloat32,
		m_sampleRate, // System sampleRate
		m_bufferLength,
		StreamCallback,
		this);

	Pa_StartStream(m_pStream);
}

//////////////////////////////////////////////////////////////////////////////////
CAudioManager::~CAudioManager()
{
	Pa_StopStream(m_pStream); //wait for stream to have stopped so we can cleanly dealloc all used memory
	Pa_CloseStream(m_pStream);

	for (int i = static_cast<int>(m_pPlayingVoices.size() - 1); i >= 0 ; --i)
	{
		delete m_pPlayingVoices[i];
		m_pPlayingVoices.pop_back();
	}

	delete m_pOutputBuffer;
	delete m_pMixer;
}

//////////////////////////////////////////////////////////////////////////////////
void CAudioManager::Play(char const* filePath, bool const positioned/* = false*/, Vec2D position/* = Vec2D(0.0f, 0.0f)*/)
{
	SF_INFO sfInfo{ 0 };
	SNDFILE* pFile = sf_open(filePath, SFM_READ, &sfInfo);
	
	if (pFile != nullptr)
	{
		SPlayingVoice* pPlayingVoice =
			new SPlayingVoice(
				pFile,
				sfInfo.channels,
				sfInfo.samplerate,
				positioned,
				position);

		m_pPlayingVoices.push_back(pPlayingVoice);
	}
}

//////////////////////////////////////////////////////////////////////////////////
void CAudioManager::RenderAudio(void* pOutputBuffer)
{
	std::fill(static_cast<float*>(pOutputBuffer), static_cast<float*>(pOutputBuffer) + m_bufferLength * m_numChannels, 0.0f);

	if (!m_pPlayingVoices.empty())
	{
		m_pOutputBuffer->pData = pOutputBuffer;

		for (int i = static_cast<int>(m_pPlayingVoices.size() - 1); i >= 0; --i)
		{
			SPlayingVoice* pPlayingVoice = m_pPlayingVoices[i];

			if ((pPlayingVoice != nullptr) && (pPlayingVoice->isValid))
			{
				float attenuation = 0.0f;

				if (pPlayingVoice->positioned)
				{
					float const horizontalCenter = ((static_cast<float>(Arkanoid::Game::g_borderRight) - static_cast<float>(Arkanoid::Game::g_borderLeft)) * 0.5f);
					float const posX = (pPlayingVoice->position.x - horizontalCenter) / horizontalCenter;
					float const posY = (static_cast<float>(Arkanoid::Game::g_borderBottom) - pPlayingVoice->position.y) / static_cast<float>(Arkanoid::Game::g_borderBottom);
					
					if (posY < 0)
					{
						bool debug = true;
					}

					float const distance = std::sqrtf(posX * posX + posY * posY);
					float const scale = 1.0f / distance;
					Vec2D pos;
					pos.x = -posX * scale;
					pos.y = posY * scale;
					
					if (m_pMixer->MixFile1InNPositional(m_pOutputBuffer, pPlayingVoice->pFile, pPlayingVoice->numChannels, pos, distance) != m_bufferLength)
					{
						delete pPlayingVoice;
						m_pPlayingVoices.erase(m_pPlayingVoices.begin() + i);
					}
				}
				else
				{
					if (m_pMixer->MixFileNInN(m_pOutputBuffer, pPlayingVoice->pFile, pPlayingVoice->numChannels) != m_bufferLength)
					{
						delete pPlayingVoice;
						m_pPlayingVoices.erase(m_pPlayingVoices.begin() + i);
					}
				}
			}
			else
			{
				delete pPlayingVoice;
				m_pPlayingVoices.erase(m_pPlayingVoices.begin() + i);
			}
		}
	}
}

// TODO: Will implement a command queue if this crashes once.
//////////////////////////////////////////////////////////////////////////////////
bool CAudioManager::UpdatePosition(SPlayingVoice* pPlayingVoice_, Vec2D const& pos)
{
	bool success = false;

	for (SPlayingVoice* pPlayingVoice : m_pPlayingVoices)
	{
		if (pPlayingVoice == pPlayingVoice_ && pPlayingVoice->isValid)
		{
			pPlayingVoice->position.x = pos.x;
			pPlayingVoice->position.y = pos.y;
			success = true;
		}
	}

	return success;
}
} //Arkanoid::Audio
