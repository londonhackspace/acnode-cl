#include "colour.h"

Colour::Colour(int r, int g, int b) : r(r & 0xFF), g(g & 0xFF), b(b & 0xFF) { }

Colour RED(255, 0, 0);
Colour GREEN(0, 255, 0);
Colour BLUE(0, 0, 255);
Colour YELLOW(255, 255, 0);
Colour ORANGE(255, 55, 0);
Colour WHITE(255, 255, 255);
Colour BLACK(0, 0, 0);
Colour PURPLE(148, 0, 211);
Colour MAUVE(102, 51, 71); // Mauve has the most RAM

Colour RAINBOW[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE};
