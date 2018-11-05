#include "colour.h"

Colour::Colour(int r, int g, int b) : r(r & 0xFF), g(g & 0xFF), b(b & 0xFF) { }

Colour RED(0, 255, 255);
Colour GREEN(255, 0, 255);
Colour BLUE(255, 255, 0);
Colour YELLOW(0, 0, 255);
Colour ORANGE(0, 200, 255);
Colour WHITE(0, 0, 0);
Colour BLACK(255, 255, 255);
Colour PURPLE(107, 255, 44);
Colour MAUVE(224, 176, 255); // Mauve has the most RAM

Colour RAINBOW[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE};
