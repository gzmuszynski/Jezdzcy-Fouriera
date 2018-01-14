#ifndef ELEMENT_H
#define ELEMENT_H


class Element
{
public:
    Element();
    Element(int x, int y, int r, int g, int b);

    int getX() const;
    int getY() const;

    int getR() const;
    int getG() const;
    int getB() const;

private:
    int x,y;
    int r,g,b;
};

#endif // ELEMENT_H
