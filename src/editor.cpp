#include <string>
#include <raylib.h>
#include <regex>
#include "editor.h"
#include "util.h"
#include "defines.h"


bool isWordSeparator(char c) {
    int i=0;
    while (wordSeparators[i]) {
        if (wordSeparators[i]==c) {
            return true;
        }
        i++;
    }

    return false;
}

void Editor::update(Vector2 mousePos, bool doBackspace) {
    if (!focused) return;

    if (IsKeyPressed(KEY_RIGHT)) {
        if (cursor<text.size()) {
            cursor++;

            if (IsKeyDown(KEY_LEFT_CONTROL)) {
                while (!isWordSeparator(text[cursor])) {
                    cursor++;
                }
            }
        }
    }
    if (IsKeyPressed(KEY_LEFT)) {
        if (cursor>0) {
            cursor--;

            if (IsKeyDown(KEY_LEFT_CONTROL)) {
                while (!isWordSeparator(text[cursor-1])) {
                    cursor--;
                }
            }
        }
    }

    if (doBackspace && IsKeyPressed(KEY_BACKSPACE)) {
        if (cursor>0) {
            text.erase(cursor-1,1);
            cursor--;
        }
    }

    cursor=clamp(cursor,0,text.size());

    char ch=GetCharPressed();
    if (ch!=0) {
        std::string s(1,ch);
        text.insert(cursor,s);
        cursor++;
    }
}

void Editor::draw(Rectangle rect, int error) {
    // Rectangle rect=getEquationRect((int)i);
    // DrawRectangleRec(rect,eq.color);

    Vector2 textPos={rect.x+20,rect.y+(rect.height-fontSize)/2};
    DrawText(text.c_str(),textPos.x,textPos.y,fontSize,BLACK); // Equation text

    if (focused && std::fmod(GetTime(),CURSOR_BLINK_SPEED)<CURSOR_BLINK_SPEED/2) {
        std::string s=text.substr(0,cursor);
        DrawRectangle(rect.x+20+MeasureText(s.c_str(),10),rect.y+6,1,12,BLACK);
    }

    if (error != -1) {
        DrawCircle(rect.x+2,rect.y+5,2,RED);
        if (error>-1) {
            std::string beforeError = text.substr(0,error);
            std::string errorChar = text.substr(error,1);
            int barX = textPos.x + MeasureText(beforeError.c_str(),fontSize);
            int barW = MeasureText(errorChar.c_str(),fontSize) + 2;
            DrawRectangle(barX,textPos.y+fontSize,barW,1,RED);
        }
    }
}