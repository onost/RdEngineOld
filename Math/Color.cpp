#include "Color.h"

const Color Color::kBlack(0.0f, 0.0f, 0.0f, 1.0f);// 黒
const Color Color::kWhite(1.0f, 1.0f, 1.0f, 1.0f);// 白
const Color Color::kRed(1.0f, 0.0f, 0.0f, 1.0f);// 赤
const Color Color::kGreen(0.0f, 1.0f, 0.0f, 1.0f);// 緑
const Color Color::kBlue(0.0f, 0.0f, 1.0f, 1.0f);// 青
const Color Color::kCyan(0.0f, 1.0f, 1.0f, 1.0f);// シアン
const Color Color::kMagenta(1.0f, 0.0f, 1.0f, 1.0f);// マゼンタ
const Color Color::kYellow(1.0f, 1.0f, 0.0f, 1.0f);// 黄

// HSV色空間から作成
Color CreateFromHSV(float h, float s, float v, float a)
{
	//Color result = Color(v, v, v, a);
	float r = v;
	float g = v;
	float b = v;
	if (s > 0.0f)
	{
		h *= 6.0f;
		int i = int(h);
		float f = h - float(i);
		switch (i)
		{
		default:
		case 0:
			g *= 1.0f - s * (1.0f - f);
			b *= 1.0f - s;
			break;
		case 1:
			r *= 1.0f - s * f;
			b *= 1.0f - s;
			break;
		case 2:
			r *= 1.0f - s;
			b *= 1.0f - s * (1.0f - f);
			break;
		case 3:
			r *= 1.0f - s;
			g *= 1.0f - s * f;
			break;
		case 4:
			r *= 1.0f - s * (1.0f - f);
			g *= 1.0f - s;
			break;
		case 5:
			g *= 1.0f - s;
			b *= 1.0f - s * f;
			break;
		}
	}
	return Color(r, g, b, a);
}
