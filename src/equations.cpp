#include <cmath>
#include <raylib.h>
#include "defines.h"
#include "util.h"
#include "equations.h"

Rectangle EquationWindow::getEquationRect(int i) {
    return {
        5,
        5+i*25,
        this->width-10,
        25
    };
}

void EquationWindow::draw(Vector2 mousePos) {
    DrawRectangle(this->width,0,4,HEIGHT,{0,0,0,30});
    BeginScissorMode(0,0,this->width,HEIGHT);
    DrawRectangle(0,0,this->width,HEIGHT,WHITE);

    for (size_t i=0; i<this->equations.size(); i++) {
        equation eq=equations.at(i);
        eq.editor.focused=(i==focusedEq);
        Rectangle rect=getEquationRect((int)i);
        eq.editor.draw(rect,eq.error);
        // DrawRectangleRec(rect,eq.color);

        DrawCircle(rect.x+10,rect.y+rect.height/2,6,eq.color); // Equation color, circle at vertical center of rect
        // Vector2 textPos={rect.x+20,rect.y+rect.height/2-5};
        // DrawText(eq.str.c_str(),textPos.x,textPos.y,10,BLACK); // Equation text

        DrawRectangle(rect.x+5,rect.y+rect.height,rect.width-10,1,GRAY);
        // if (focusedEq==i && std::fmod(GetTime(),CURSOR_BLINK_SPEED)<CURSOR_BLINK_SPEED/2) {
        //     std::string s=eq.str.substr(0,cursor);
        //     DrawRectangle(rect.x+20+MeasureText(s.c_str(),10),rect.y+6,1,12,BLACK);
        // }

        // if (eq.error != -1) {
        //     DrawCircle(rect.x+2,rect.y+5,2,RED);
        //     if (eq.error>-1) {
        //         std::string beforeError = eq.str.substr(0,eq.error);
        //         std::string errorChar = eq.str.substr(eq.error,1);
        //         int barX = textPos.x + MeasureText(beforeError.c_str(),10);
        //         int barW = MeasureText(errorChar.c_str(),10) + 2;
        //         DrawRectangle(barX,textPos.y+10,barW,1,RED);
        //     }
        // }
    }

    EndScissorMode();
}

void EquationWindow::update(Vector2 mousePos) {
    bool deletedEq = false;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mousePos.x<=width) {
        for (int i=0;i<equations.size();i++) {
            if (CheckCollisionPointRec(mousePos,getEquationRect(i))) {
                focusedEq=i;
                break;
            }
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && focusedEq>=0) {
        if (equations[focusedEq].editor.cursor==0 && equations.size()>1) {
            equations.erase(equations.begin()+focusedEq);
            focusedEq--;
            focusedEq=max(focusedEq,0);
            
            deletedEq = true;
            // equations[focusedEq].editor.cursor=equations[focusedEq].str.size();
        }
    }

    if (IsKeyPressed(KEY_DOWN) && focusedEq<equations.size()-1) {
        focusedEq++;
    }
    if (IsKeyPressed(KEY_UP) && focusedEq>0) {
        focusedEq--;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        equations.insert(equations.begin()+focusedEq+1,(equation){
            std::string(""),
            (Color){(unsigned char)GetRandomValue(0,255),(unsigned char)GetRandomValue(0,255),(unsigned char)GetRandomValue(0,255),255},
        });
        focusedEq++;
    }

    for (int i=0;i<equations.size();i++) {
        equation eq=equations[i];
        eq.editor.focused=(i==focusedEq);
        eq.editor.text=eq.str;
        eq.editor.update(mousePos, !deletedEq);
        eq.str=eq.editor.text;
        equations[i]=eq;
        // equations[i].editor.text=eq.editor.text;
    }
}