#pragma once

#include <math.h>

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