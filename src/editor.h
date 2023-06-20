#pragma once
#include <string>
#include <raylib.h>
#include <regex>

const std::regex wordRegex("`~!@#\$%\^&\*\(\)-=\+\[{]}\\\|;:'\",\.<>\/\?");

class Editor {
private:
    /* data */
public:
    std::string text;
    int cursor;
    // Vector2 pos;
    int fontSize;
    bool focused;

    Editor() {
        this->text=std::string("");
        // this->pos={0,0};
        this->cursor=0;
        this->fontSize=10;
    }
    Editor(std::string* text) {
        this->text=*text;
        cursor=0;
        fontSize=10;
        focused=false;
    }

    void draw(Rectangle rect, int error);
    void update(Vector2 mousePos);
};
