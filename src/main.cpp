#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "util.h"
#include "tinyexpr.h"

#define WIDTH  320
#define HEIGHT 240
// #define WIDTH  640
// #define HEIGHT 480
#define RATIO HEIGHT/WIDTH
#define PIX_TO_UNIT float(window.width)/float(WIDTH)
#define windowRight window.x+window.width
#define windowBottom window.y+window.height

// #region window
Rectangle window={-10.f,-10*RATIO,20.f,20.f*RATIO};
void scaleWindow(float v) {
    v*=window.width/10;
    window.width +=v;
    // window.height+=v;
    window.height=window.width*RATIO;
    window.x     -=v/2;
    window.y     -=v/2;
}
float getGridUnit() {
    if (window.width<5 && window.width>0) {
        return powf(10,floorBy(log10f(window.width),1));
    }
    return roundBy(window.width/10,1);
}

Vector2 translatePoint(Vector2 p) {
    return Vector2{
        translate(p.x,window.x,windowRight,0,WIDTH),
        translate(p.y,window.y,windowBottom,HEIGHT,0),
    };
}

float graphStep=1;

//#endregion

// #region input

struct equation {
    std::string str;
    Color color;
};

std::vector<equation> equations={
    (equation){std::string("sin(x)"),(Color){250,10,10,255}},
    (equation){std::string("aaaa hello i am borgus the frog "),(Color){10,10,250,255}},
};

int focusedEq=0;
int cursor=0;
int equationWindowSize=150;

void drawEquations() {
    DrawRectangle(equationWindowSize,0,4,HEIGHT,{0,0,0,30});
    BeginScissorMode(0,0,equationWindowSize,HEIGHT);
    DrawRectangle(0,0,equationWindowSize,HEIGHT,WHITE);
    for (size_t i=0; i<equations.size(); i++) {
        equation eq=equations.at(i);
        DrawCircle(15,15+i*25,6,eq.color);
        DrawText(eq.str.c_str(),25,10+i*25,10,BLACK);
        DrawRectangle(5,25+i*25,equationWindowSize-10,1,GRAY);
        if (focusedEq==i && std::fmod(GetTime(),0.5)<0.25) {
            std::string s=eq.str.substr(0,cursor);
            DrawRectangle(25+MeasureText(s.c_str(),10),10+i*25-1,1,12,BLACK);
        }
    }
    EndScissorMode();
}

void save() {
    std::string s="";
    for (size_t i=0;i<equations.size();i++) {
        s+=equations[i].str;
        s+="\n";
    }

    SaveFileText("save.txt",s.c_str());
}

//#endregion


float func(float x) {
    // return powf(x,2);
    // return GetRandomValue(-1,1);
    // return asinf(sinf(x));
    // return 2*powf(x,3) + 4*powf(x,2) + 1.5*x - 0.1;
    // return 1/x;
    // return sin(2 * PI * x) + cos(x / 2 * PI);
    // return sqrt(1-pow(x,2));
    // return cos(x*14+GetTime()*2+1)/2+pow(0.7,sin(x+GetTime()*10+cos(GetTime()*5))+cos(x*2.3)/1.1+sin(x*3-31)*2-cos(x*12+GetTime()*15+1)/2);
    // return pow(2.7,x);
    return -abs(2*x)+pow(x,2);
}

te_parser tep;

Color cursorColor=BLUE;
bool isDraggingEQWindow=false;

int main() {
    // printf("%d\n",1/0);
    printf("------\n%f\n------\n",float(HEIGHT)/float(WIDTH));
    InitWindow(WIDTH,HEIGHT,"graphing");
    SetTargetFPS(60);
    HideCursor();
    SetWindowState(FLAG_WINDOW_UNDECORATED);

    while (!WindowShouldClose()) {
        if (GetMouseX()>WIDTH) SetMousePosition(0,GetMouseY());
        if (GetMouseY()>HEIGHT) SetMousePosition(GetMouseX(),0);
        if (GetMouseX()<0) SetMousePosition(WIDTH,GetMouseY());
        if (GetMouseY()<0) SetMousePosition(GetMouseX(),HEIGHT);
        if (window.width<=0) window.width=1;
        if (window.height<=0) window.height=1;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !isDraggingEQWindow) {
            window.x-=GetMouseDelta().x*window.width /WIDTH /(IsKeyDown(KEY_LEFT_SHIFT) ? 5 : 1);
            window.y+=GetMouseDelta().y*window.height/HEIGHT/(IsKeyDown(KEY_LEFT_SHIFT) ? 5 : 1);
        }

        if (CheckCollisionPointRec(GetMousePosition(),{equationWindowSize-2,0,4,HEIGHT})) {
            cursorColor=RED;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                isDraggingEQWindow=true;
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                isDraggingEQWindow=false;
            }
        } else {
            cursorColor=BLUE;
        }
        if (isDraggingEQWindow) {
            equationWindowSize=GetMouseX();
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX()<=equationWindowSize) {
            for (size_t i=0;i<equations.size();i++) {
                if (CheckCollisionPointRec(GetMousePosition(),{
                    2,5+i*25,equationWindowSize-10,25
                })) {
                    focusedEq=i;
                }
            }
        }

        // int key=GetKeyPressed();
        if (IsKeyPressed(KEY_BACKSPACE) && focusedEq>=0) {
            if (cursor>0) {
                equations[focusedEq].str.erase(cursor-1,1);
                cursor--;
            } else {
                equations.erase(equations.begin()+focusedEq);
                focusedEq--;
                focusedEq=max(focusedEq,0);
                cursor=equations[focusedEq].str.size();
            }
        }
        if (IsKeyPressed(KEY_RIGHT) && focusedEq>=0) {
            if (cursor<equations[focusedEq].str.size())
                cursor++;
        }
        if (IsKeyPressed(KEY_LEFT) && focusedEq>=0) {
            if (cursor>0)
                cursor--;
        }
        if (IsKeyPressed(KEY_DOWN) && focusedEq<equations.size()) {
            focusedEq++;
        }
        if (IsKeyPressed(KEY_UP) && focusedEq>0) {
            focusedEq--;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            equations.insert(equations.begin()+focusedEq+1,(equation){
                std::string(""),
                (Color){GetRandomValue(0,255),GetRandomValue(0,255),GetRandomValue(0,255),255},
            });
            focusedEq++;
        }
        
        if (focusedEq>=0)
            cursor=clamp(cursor,0,equations[focusedEq].str.size());

        char ch=GetCharPressed();
        if (ch!=0) {
            if (focusedEq>=0) {
                std::string s(1,ch);
                equations[focusedEq].str.insert(cursor,s);
                cursor++;
            }
            // printf("%c\n",ch);
        }

        if (abs(GetMouseWheelMove())!=0)
            scaleWindow(-GetMouseWheelMove());

        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        // x=0
        DrawLineEx(
            {translate(0,window.x,windowRight,0,WIDTH),0},
            {translate(0,window.x,windowRight,0,WIDTH),HEIGHT},
            2,BLACK
        );
        // y=0
        DrawLineEx(
            {0,    translate(0,window.y,windowBottom,HEIGHT,0)},
            {WIDTH,translate(0,window.y,windowBottom,HEIGHT,0)},
            2,BLACK
        );

        for (float x = roundBy(window.x+equationWindowSize*PIX_TO_UNIT,getGridUnit()); x < windowRight; x += getGridUnit()) {
            DrawLineEx(
                {translate(x,window.x,windowRight,0,WIDTH),0},
                {translate(x,window.x,windowRight,0,WIDTH),HEIGHT},
                1,
                GRAY
            );
            Vector2 textPoint=translatePoint({x,0});
            auto text=TextFormat("%.6g",x);
            textPoint.x=clamp(textPoint.x,equationWindowSize,WIDTH-MeasureText(text,10)-10);
            textPoint.y=clamp(textPoint.y,0,HEIGHT-25);
            // if (textPoint.x<0) printf("")
            DrawText(text,textPoint.x,textPoint.y+15,10,BLACK);
        }
        for (float y = roundBy(window.y,getGridUnit()); y < windowBottom; y += getGridUnit()) {
            if (y==0) continue;
            DrawLineEx(
                {0,translate(y,windowBottom,window.y,0,HEIGHT)},
                {WIDTH,translate(y,windowBottom,window.y,0,HEIGHT)},
                1,
                GRAY
            );
            Vector2 textPoint=translatePoint({0,y});
            auto text=TextFormat("%.6g",y);
            textPoint.x=clamp(textPoint.x,equationWindowSize,WIDTH-MeasureText(text,10)-10);
            textPoint.y=clamp(textPoint.y,0,HEIGHT-25);
            DrawText(text,textPoint.x+5,textPoint.y+15,10,BLACK);
        }

        for (size_t e=0;e<equations.size();e++) {
            equation eq=equations.at(e);

            double x{ 0 };
            tep.set_variables_and_functions({{"x", &x}});

            auto result = tep.evaluate(eq.str.c_str());

            if (!tep.success()) continue;

            Vector2 lastPoint={window.x-1,0};
            for (float i=0; i<=float(WIDTH); i+=graphStep) {
                x=i/float(WIDTH);
                x*=float(window.width);
                x+=window.x;
                // printf("x=%f %f %f\n",x,window.x,windowRight);
                // WaitTime(0.1);
                Vector2 point={x,tep.evaluate()};
                DrawLineEx(translatePoint(lastPoint),translatePoint(point),2,eq.color);
                lastPoint=point;
            }
        }
        // printf("----------------------------\n");

        drawEquations();


        DrawLineEx(sub(GetMousePosition(),GetMouseDelta()),GetMousePosition(),3,cursorColor);
        DrawCircle(GetMousePosition().x,GetMousePosition().y,2,cursorColor);
        DrawText(TextFormat("%d",GetFPS()),10,HEIGHT-10-10,10,BLACK);
        // DrawText(TextFormat("(%.2f,%.2f) %.2fx%.2f",window.x,window.y,window.width,window.height),10,25,10,BLACK);

        EndDrawing();
    }

    CloseWindow();
}
