#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include "editor.h"

struct equation {
    std::string str;
    Color color;
    int error;
    Editor editor;
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
            std::string str("sin(x)");
            this->equations={
                {str, RED, -1, Editor(
                    &str
                )}
            };
            this->width=150;
            this->focusedEq=0;
            this->cursor=0;
        }

        Rectangle getEquationRect(int i);
        void draw(Vector2 mousePos);
        void update(Vector2 mousePos);
};
