#ifndef CSE169_COLOR_H
#define CSE169_COLOR_H

class Color
{
public:
	Color();
	Color(float, float, float, float);
	~Color();

	float r, g, b, a;
};

#endif