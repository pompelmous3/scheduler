#include "submodule.h"

SubModule::SubModule()
    :y(0),x(0),title(""),hovered(false)
{
    // LOG("[SubModule::SubModule] constructor");
};

void SubModule::print() {
    mvprintwColor(y, x+1, title.c_str(), hovered?10:3, true);
}

void SubModule::set_hovered(bool h) {
    hovered = h;
}