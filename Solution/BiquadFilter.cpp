#include "BiquadFilter.h"
#define _USE_MATH_DEFINES
#include <math.h>
#pragma once

namespace Arkanoid::Audio
{
//////////////////////////////////////////////////////////////////////////
void CBiquadFilter::ComputeCoefficients(int const frequency, float const qualityFactor, float const peakGain)
{
	double const factorV = pow(10.0, (fabs(peakGain) / 20.0));
	double const factorK = tan(M_PI * (frequency / m_sampleRate));
	double const factorKSpuare = factorK * factorK;

	switch (m_filterType)
	{
	case EBiquadType::Lowpass:
	{
		double const norm = 1.0 / (1.0 + factorK / qualityFactor + factorKSpuare);
		m_coefficientA0 = static_cast<float>(factorKSpuare * norm);
		m_coefficientA1 = static_cast<float>(2.0 * m_coefficientA0);
		m_coefficientA2 = m_coefficientA0;
		m_coefficientB0 = static_cast<float>(2.0 * (factorKSpuare - 1.0) * norm);
		m_coefficientB1 = static_cast<float>((1.0 - factorK / qualityFactor + factorKSpuare) * norm);
		break;
	}
	case EBiquadType::Highpass:
	{
		double const norm = 1.0 / (1.0 + factorK / qualityFactor + factorKSpuare);
		m_coefficientA0 = static_cast<float>(1.0 * norm);
		m_coefficientA1 = static_cast<float>(-2.0 * m_coefficientA0);
		m_coefficientA2 = m_coefficientA0;
		m_coefficientB0 = static_cast<float>(2.0 * (factorKSpuare - 1.0) * norm);
		m_coefficientB1 = static_cast<float>((1.0 - factorK / qualityFactor + factorKSpuare) * norm);
		break;
	}
	case EBiquadType::Bandpass:
	{
		double const norm = 1.0 / (1.0 + factorK / qualityFactor + factorKSpuare);
		m_coefficientA0 = static_cast<float>(factorK / qualityFactor * norm);
		m_coefficientA1 = 0.0f;
		m_coefficientA2 = -m_coefficientA0;
		m_coefficientB0 = static_cast<float>(2.0 * (factorKSpuare - 1.0) * norm);
		m_coefficientB1 = static_cast<float>((1.0 - factorK / qualityFactor + factorKSpuare) * norm);
		break;
	}
	case EBiquadType::Notch:
	{
		double const norm = 1.0 / (1.0 + factorK / qualityFactor + factorKSpuare);
		m_coefficientA0 = static_cast<float>((1.0 + factorKSpuare) * norm);
		m_coefficientA1 = static_cast<float>(2.0 * (factorKSpuare - 1.0) * norm);
		m_coefficientA2 = m_coefficientA0;
		m_coefficientB0 = m_coefficientA1;
		m_coefficientB1 = static_cast<float>((1.0 - factorK / qualityFactor + factorKSpuare) * norm);
		break;
	}
	case EBiquadType::Peak:
	{
		if (peakGain >= 0)  // boost
		{
			double const x1 = 1.0 / qualityFactor * factorK;
			double const x2 = factorV / qualityFactor * factorK;
			double const norm = 1.0 / (1.0 + x1 + factorKSpuare);
			m_coefficientA0 = static_cast<float>((1.0 + x2 + factorKSpuare) * norm);
			m_coefficientA1 = static_cast<float>(2.0 * (factorKSpuare - 1.0) * norm);
			m_coefficientA2 = static_cast<float>((1.0 - x2 + factorKSpuare) * norm);
			m_coefficientB0 = m_coefficientA1;
			m_coefficientB1 = static_cast<float>((1.0 - x1 + factorKSpuare) * norm);
		}
		else      // cut
		{
			double const x1 = 1.0 / qualityFactor * factorK;
			double const x2 = factorV / qualityFactor * factorK;
			double const norm = 1.0 / (1.0 + x2 + factorKSpuare);
			m_coefficientA0 = static_cast<float>((1.0 + x1 + factorKSpuare) * norm);
			m_coefficientA1 = static_cast<float>(2.0 * (factorKSpuare - 1.0) * norm);
			m_coefficientA2 = static_cast<float>((1.0 - x1 + factorKSpuare) * norm);
			m_coefficientB0 = m_coefficientA1;
			m_coefficientB1 = static_cast<float>((1.0 - x2 + factorKSpuare) * norm);
		}

		break;
	}
	case EBiquadType::Lowshelf:
	{
		if (peakGain >= 0) // boost
		{
			double const x1 = sqrt(2.0 * factorV) * factorK;
			double const x2 = factorV * factorKSpuare;
			double const x3 = M_SQRT2 * factorK;
			double const norm = 1.0 / (1.0 + x3 + factorKSpuare);
			m_coefficientA0 = static_cast<float>((1.0 + x1 + x2) * norm);
			m_coefficientA1 = static_cast<float>(2.0 * (x2 - 1.0) * norm);
			m_coefficientA2 = static_cast<float>((1.0 - x1 + x2) * norm);
			m_coefficientB0 = static_cast<float>(2.0 * (factorKSpuare - 1.0) * norm);
			m_coefficientB1 = static_cast<float>((1.0 - x3 + factorKSpuare) * norm);
		}
		else      // cut
		{
			double const x1 = sqrt(2.0 * factorV) * factorK;
			double const x2 = factorV * factorKSpuare;
			double const x3 = M_SQRT2 * factorK;
			double const norm = 1.0 / (1.0 + x1 + x2);
			m_coefficientA0 = static_cast<float>((1.0 + x3 + factorKSpuare) * norm);
			m_coefficientA1 = static_cast<float>(2.0 * (factorKSpuare - 1.0) * norm);
			m_coefficientA2 = static_cast<float>((1.0 - x3 + factorKSpuare) * norm);
			m_coefficientB0 = static_cast<float>(2.0 * (x2 - 1.0) * norm);
			m_coefficientB1 = static_cast<float>((1.0 - x1 + x2) * norm);
		}

		break;
	}
	case EBiquadType::Highshelf:
	{
		if (peakGain >= 0) // boost
		{
			double const x1 = sqrt(2.0 * factorV) * factorK;
			double const x2 = M_SQRT2 * factorK;
			double const norm = 1.0 / (1.0 + x2 + factorKSpuare);
			m_coefficientA0 = static_cast<float>((factorV + x1 + factorKSpuare) * norm);
			m_coefficientA1 = static_cast<float>(2.0 * (factorKSpuare - factorV) * norm);
			m_coefficientA2 = static_cast<float>((factorV - x1 + factorKSpuare) * norm);
			m_coefficientB0 = static_cast<float>(2.0 * (factorKSpuare - 1) * norm);
			m_coefficientB1 = static_cast<float>((1.0 - x2 + factorKSpuare) * norm);
		}
		else      // cut
		{
			double const x1 = sqrt(2.0 * factorV) * factorK;
			double const x2 = M_SQRT2 * factorK;
			double const norm = 1.0 / (factorV + x1 + factorKSpuare);
			m_coefficientA0 = static_cast<float>((1.0 + x1 + factorKSpuare) * norm);
			m_coefficientA1 = static_cast<float>(2.0 * (factorKSpuare - 1.0) * norm);
			m_coefficientA2 = static_cast<float>((1.0 - x1 + factorKSpuare) * norm);
			m_coefficientB0 = static_cast<float>(2.0 * (factorKSpuare - factorV) * norm);
			m_coefficientB1 = static_cast<float>((factorV - x1 + factorKSpuare) * norm);
		}

		break;
	}
	}
}

//////////////////////////////////////////////////////////////////////////
float CBiquadFilter::ProcessSample(float const sample)
{
	float const fCurOut = sample * m_coefficientA0 + m_lastSample1;
	m_lastSample1 = sample * m_coefficientA1 + m_lastSample2 - m_coefficientB0 * fCurOut;
	m_lastSample2 = sample * m_coefficientA2 - m_coefficientB1 * fCurOut;
	return fCurOut;
}
} // Arkanoid::Audio
