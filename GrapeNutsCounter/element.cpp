#include "element.h"

Element::Element(int x, int y, int r, int g, int b) : x(x), y(y), r(r), g(g), b(b)
{

}

int Element::getX() const
{
    return x;
}

int Element::getY() const
{
    return y;
}

int Element::getR() const
{
    return r;
}

int Element::getG() const
{
    return g;
}

int Element::getB() const
{
    return b;
}
