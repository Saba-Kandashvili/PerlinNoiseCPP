#include <iostream>
#include <cmath>
#include "SFML/Graphics.hpp"

typedef struct
{
	float x, y;
} vector2;

float Perlin(float, float);
float DotGridGradient(int, int, float, float);
vector2 RandomGradient(int, int);
float Interpolate(float, float, float);

int main()
{
	const int windowWidth = 1920;
	const int windowHeight = 1080;

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight, 32), "Perlin");

	sf::Uint8* pixels = new sf::Uint8[windowWidth * windowHeight * 4];

	const int GRID_SIZE = 400;

	for (int  x = 0; x < windowWidth; x++)
	{
		for (int y = 0; y < windowHeight; y++)
		{
			int index = (y * windowWidth + x) * 4;

			float val = 0;

			float freq = 1;
			float amp = 1;
			
			for (int i = 0; i < 12; i++)
			{
				val += Perlin(x * freq / GRID_SIZE, y * freq / GRID_SIZE) * amp;
				freq *= 2;
				amp /= 2;
			}

			// contrast
			val *= 1.4;

			// clamp
			if (val > 1.0f)
				val = 1.0f;
			else if (val < -1.0f)
				val = -1.0f;

			// convert (-1, 1) to (0, 255)
			int color = (int)(((val + 1.0f) * 0.5f) * 255);

			// set pixel color
			pixels[index] = color;
			pixels[index + 1] = color;
			pixels[index + 2] = color;
			pixels[index + 3] = 255;
		}
	}

	sf::Texture texture;
	sf::Sprite sprite;

	texture.create(windowWidth, windowHeight);

	texture.update(pixels);
	sprite.setTexture(texture);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(sprite);

		window.display();
	}

	return 0;
}


float Perlin(float x, float y)
{
	// grid cell corner coordinates
	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	// distance weights from corners
	float sx = x - (float)x0;
	float sy = y - (float)y0;

	// compute top corners
	float n0 = DotGridGradient(x0, y0, x, y);
	float n1 = DotGridGradient(x1, y0, x, y);
	float ix0 = Interpolate(n0, n1, sx);

	// compute bottom corners
	n0 = DotGridGradient(x0, y1, x, y);
	n1 = DotGridGradient(x1, y1, x, y);
	float ix1 = Interpolate(n0, n1, sx);

	// interpolate between previous interpolations now in y
	float value = Interpolate(ix0, ix1, sy);

	return value;
}

float DotGridGradient(int ix, int iy, float x, float y)
{
	// gradient from int coordinates
	vector2 gradient = RandomGradient(ix, iy);

	// distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;

	// dot-product
	return (dx * gradient.x + dy * gradient.y);
}

vector2 RandomGradient(int ix, int iy)
{
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;
	unsigned a = ix;
	unsigned b = iy;
	a *= 3284157443;

	b ^= a << s | a >> w - s;
	b *= 1911520717;

	a ^= b << s | b >> w - s;
	a *= 2048419325;
	float random = a * (3.14159265 / ~(~0u >> 1));
	// hashing black magic and bit manipulation that no one fully understands

	vector2 v;
	v.x = sin(random);
	v.y = cos(random);

	return v;
}

float Interpolate(float a0, float a1, float w)
{
	//cubic interpolation
	return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}