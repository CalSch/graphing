#pragma once

#include <raylib.h>
#include <string>
#include <vector>

struct equation {
    std::string str;
    Color color;
    int error;
};

class EquationWindow {
    private:
        /* data */
    public:
        std::vector<equation> equations;
        int width;
        int focusedEq;
        int cursor;
        EquationWindow() {
            this->equations={
                {std::string("sin(x)"), RED, -1}
            };
            this->width=150;
            this->focusedEq=0;
            this->cursor=0;

        }

        Rectangle getEquationRect(int i);
        void draw(Vector2 mousePos);
        void update(Vector2 mousePos);
};