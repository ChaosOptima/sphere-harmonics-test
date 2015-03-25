#pragma once

#include <math.h>
#include <functional>

int doubleFactorial(int x)
{
	int result;
	if (x == 0 || x == -1)
		return 1;

	result = x;
	while ((x -= 2) > 0)
		result *= x;
	return result;
}

double P(int l, int m, double x)
{
	// evaluate an Associated Legendre Polynomial P(l,m,x) at x
	double pmm = 1.0;
	if (m > 0) 
	{
		double somx2 = sqrt((1.0 - x)*(1.0 + x));
		double fact = 1.0;
		for (int i = 1; i <= m; i++) {
			pmm *= (-fact) * somx2;
			fact += 2.0;
		}
	}

	if (l == m) return pmm;

	double pmmp1 = x * (2.0*m + 1.0) * pmm;
	if (l == m + 1) return pmmp1;
	double pll = 0.0;
	for (int ll = m + 2; ll <= l; ++ll) {
		pll = ((2.0*ll - 1.0)*x*pmmp1 - (ll + m - 1.0)*pmm) / (ll - m);
		pmm = pmmp1;
		pmmp1 = pll;
	}
	return pll;
}

double ALPStd(float x, int l, int m)
{
	return P(l, m, x);

	if (l == m)
		return pow(-1, m) * doubleFactorial(2 * m - 1) * pow(sqrt(1 - x * x), m);

	if (l == m + 1)
		return x * (2 * m + 1) * ALPStd(x, m, m);

	return (x * (2 * l - 1) * ALPStd(x, l - 1, m) - (l + m - 1) * ALPStd(x, l - 2, m)) / (l - m);
}

int factorial(int x)
{
	int result;
	if (x == 0 || x == -1) return (1);
	result = x;
	while ((x -= 1) > 0) result *= x;
	return result;
}

double evaluateK(int l, int m)
{
	static auto PI = 3.14159265;
	double result = ((2.0 * l + 1.0) * factorial(l - m)) / (4.0 * PI * factorial(l + m));
	return sqrt(result);
}

static auto PI = 3.14159265;

std::function<double(double, double)> SphereFunctions[] =
{
	[](double theta, double phi){return sqrt(1.0f / (4.0f * PI)); }, // Y l0 m0
	[](double theta, double phi){return sqrt(3.0f / (4.0f * PI)) * sin(phi) * sin(theta); }, // Y l1 m-1
	[](double theta, double phi){return sqrt(3.0f / (4.0f * PI)) * cos(theta); }, // Y l1 m-1
	[](double theta, double phi){return sqrt(3.0f / (4.0f * PI)) * cos(phi) * sin(theta); }, // Y l1 m1
};
const int SF_Count = sizeof(SphereFunctions) / sizeof(std::function<double(double, double)>);


double evaluateSH(int l, int m, double theta, double phi)
{
	static auto SQRT2 = sqrt(2.);
	double SH = 0.0;
	if (m == 0)
	{
		SH = evaluateK(l, 0) * ALPStd(cos(theta),l, m);
	}
	else if (m > 0)
	{
		SH = SQRT2 * evaluateK(l, m) * cos(m * phi) * ALPStd(cos(theta), l, m);
	}
	else
	{
		SH = SQRT2 * evaluateK(l, -m) * sin(-m * phi) * ALPStd(cos(theta), l, -m);
	}
	return SH;
}

double FevaluateSH(int l, int m, double theta, double phi)
{
	auto ind = l * (l + 1) + m;
	if (ind < SF_Count)	return SphereFunctions[ind](theta, phi);

	return evaluateSH(l, m, theta, phi);
}


struct SHVector3d
{
	double x, y, z;
};


struct SHSample
{
	SHVector3d sph;
	SHVector3d vec;
	double *coeff = 0;
};


double rnd()
{
	return (double)rand() / (double)RAND_MAX;
}

void sphericalStratifiedSampling(SHSample *_samples, int _sqrt_samples_count, int _coef_count)
{
	double lv_InvNumSamples = 1.0 / _sqrt_samples_count;

	int i = 0;
	for (int a = 0; a < _sqrt_samples_count; a++)
	{
		for (int b = 0; b < _sqrt_samples_count; b++)
		{
			double x = ((double)a + rnd()) * lv_InvNumSamples;
			double y = ((double)b + rnd()) * lv_InvNumSamples;
			
			double theta = 2.0 * acos(sqrt(1.0 - x));
			double phi = 2.0 * PI * y;
			
			_samples[i].sph.x = theta;
			_samples[i].sph.y = phi;
			_samples[i].sph.z = 1.0;
			
			_samples[i].vec.x = sin(theta) * cos(phi);
			_samples[i].vec.y = sin(theta) * sin(phi);
			_samples[i].vec.z = cos(theta);

			if (!_samples[i].coeff)
				_samples[i].coeff = new double[_coef_count * _coef_count];

			for (int l = 0; l < _coef_count; ++l)
			{
				for (int m = -l; m <= l; ++m)
				{
					int index = l * (l + 1) + m;
					_samples[i].coeff[index] = evaluateSH(l, m, theta, phi);
				}
			}
			++i;
		}
	}
}

typedef double(*SphericalFunction)(double theta, double phi);

void SHProjectSphericalFunction(SphericalFunction SPFunc, SHSample *samples, double *result, int numSamples, int numCoeffs)
{
	double dWeight = 4.0 * PI;
	double factor = dWeight / numSamples;

	for (int i = 0; i < numSamples; i++)
	{
		double theta = samples[i].sph.x;
		double phi = samples[i].sph.y;
		double SP_Val = SPFunc(theta, phi);

		for (int n = 0; n < numCoeffs; n++)
		{
			result[n] += SP_Val * samples[i].coeff[n];
		}
	}

	for (int i = 0; i < numCoeffs; i++)
	{
		result[i] = result[i] * factor;
	}
}