#ifndef TRANSFORMWINDOW_HPP
#define TRANSFORMWINDOW_HPP

#include "Components.hpp"
#include "Register.hpp"

class TransformWindow{
public:
    inline static int current = 0;
    bool isActive = true;
    inline static Transform* target;
    void show();
    void init();
    static void updateTarget();
};


#endif