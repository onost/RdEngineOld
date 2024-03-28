#pragma once

// 色
class Color
{
public:
	float r;
	float g;
	float b;
	float a;

	Color()
		: r(0.0f)
		, g(0.0f)
		, b(0.0f)
		, a(0.0f)
	{}

	Color(float r, float g, float b, float a)
		: r(r)
		, g(g)
		, b(b)
		, a(a)
	{}

	static const Color kBlack;// 黒
	static const Color kWhite;// 白
	static const Color kRed;// 赤
	static const Color kGreen;// 緑
	static const Color kBlue;// 青
	static const Color kCyan;// シアン
	static const Color kMagenta;// マゼンタ
	static const Color kYellow;// 黄
};

inline Color operator-(const Color& a)
{
	return Color(-a.r, -a.g, -a.b, -a.a);
}

inline Color operator+(const Color& a, const Color& b)
{
	return Color(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
}

inline Color operator-(const Color& a, const Color& b)
{
	return Color(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
}

inline Color operator*(const Color& a, const Color& b)
{
	return Color(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
}

inline Color operator*(const Color& a, float b)
{
	return Color(a.r * b, a.g * b, a.b * b, a.a * b);
}

inline Color operator*(float a, const Color& b)
{
	return Color(a * b.r, a * b.g, a * b.b, a * b.a);
}

inline Color operator/(const Color& a, float b)
{
	float oneOverA = 1.0f / b;
	return Color(a.r * oneOverA, a.g * oneOverA, a.b * oneOverA, a.a * oneOverA);
}

inline Color& operator+=(Color& a, const Color& b)
{
	a = a + b;
	return a;
}

inline Color& operator-=(Color& a, const Color& b)
{
	a = a - b;
	return a;
}

inline Color& operator*=(Color& a, const Color& b)
{
	a = a * b;
	return a;
}

inline Color& operator*=(Color& a, float b)
{
	a = a * b;
	return a;
}

inline Color& operator/=(Color& a, float b)
{
	float oneOverA = 1.0f / b;
	a *= oneOverA;
	return a;
}

// HSV色空間から作成
Color CreateFromHSV(float h, float s, float v, float a = 1.0f);
