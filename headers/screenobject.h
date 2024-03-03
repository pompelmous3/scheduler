#ifndef SCREENOBJECT_H
#define SCREENOBJECT_H
#include <string>
#include <vector>

class ScreenObject
{
protected: // only accessible by the derived classes
    int init_x, init_y;
    int x, y;
    int height, width;
    std::vector <std::string> printMap;

public:
    ScreenObject(int sc_h, int sc_w); // center-aligned
    ScreenObject(int x, int y, int h, int w);
    ~ScreenObject();
    void print();
};
#endif