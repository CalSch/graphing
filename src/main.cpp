#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <regex>
#include "../include/util.h"
#include "../include/tinyexpr.h"

#define WIDTH  320
#define HEIGHT 240
// #define WIDTH  640
// #define HEIGHT 480
#define RATIO HEIGHT/WIDTH
#define PIX_TO_UNIT float(window.width)/float(WIDTH)
#define windowRight window.x+window.width
#define windowBottom window.y+window.height
#define EQ_WINDOW_HANDLE_SIZE 4

RenderTexture2D scr;

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

// #region equations

struct equation {
    std::string str;
    Color color;
    int error;
};

std::vector<equation> equations={
    (equation){std::string("sin(x)"),(Color){250,10,10,255},-1},
    (equation){std::string("x^2 "),(Color){10,10,250,255},-1},
};

int focusedEq=0;
int cursor=0;
int equationWindowSize=150;

Rectangle getEquationRect(int i) {
    return {
        5,
        5+i*25,
        equationWindowSize-10,
        25
    };
}

void drawEquationsWindow(Vector2 mousePos) {
    DrawRectangle(equationWindowSize,0,4,HEIGHT,{0,0,0,30});
    BeginScissorMode(0,0,equationWindowSize,HEIGHT);
    DrawRectangle(0,0,equationWindowSize,HEIGHT,WHITE);

    for (size_t i=0; i<equations.size(); i++) {
        equation eq=equations.at(i);
        Rectangle rect=getEquationRect((int)i);
        // DrawRectangleRec(rect,eq.color);

        DrawCircle(rect.x+10,rect.y+rect.height/2,6,eq.color); // Equation color, circle at vertical center of rect
        Vector2 textPos={rect.x+20,rect.y+rect.height/2-5};
        DrawText(eq.str.c_str(),textPos.x,textPos.y,10,BLACK); // Equation text

        DrawRectangle(rect.x+5,rect.y+rect.height,rect.width-10,1,GRAY);
        if (focusedEq==i && std::fmod(GetTime(),0.5)<0.25) {
            std::string s=eq.str.substr(0,cursor);
            DrawRectangle(rect.x+20+MeasureText(s.c_str(),10),rect.y+6,1,12,BLACK);
        }

        if (eq.error != -1) {
            DrawCircle(rect.x+2,rect.y+5,2,RED);
            if (eq.error>-1) {
                std::string beforeError = eq.str.substr(0,eq.error);
                std::string errorChar = eq.str.substr(eq.error,1);
                int barX = textPos.x + MeasureText(beforeError.c_str(),10);
                int barW = MeasureText(errorChar.c_str(),10) + 2;
                DrawRectangle(barX,textPos.y+10,barW,1,RED);
            }
        }
    }

    EndScissorMode();
}

// void save() {
//     std::string s="";
//     for (size_t i=0;i<equations.size();i++) {
//         s+=equations[i].str;
//         s+="\n";
//     }

//     SaveFileText("save.txt",s.c_str());
// }

//#endregion

te_parser tep;

Color cursorColor=BLUE;
bool isDraggingEQWindow=false;
bool isDraggingGraph=false;

const std::regex varDefRegex=std::regex("^(([a-zA-Z])[a-zA-Z0-9_]*)=");

int main() {
    // printf("%d\n",1/0);
    printf("------\n%f\n------\n",float(HEIGHT)/float(WIDTH));
    InitWindow(WIDTH*2,HEIGHT*2,"graphing");
    SetTargetFPS(60);
    HideCursor();
    SetWindowState(FLAG_WINDOW_UNDECORATED);
    // Fullscreen if on Raspberry Pi
    #ifdef __aarch64__
    printf("\nDetected Raspberry Pi!\n\n");
    SetWindowPosition(0,0);
    ToggleFullscreen();
    #endif

    scr=LoadRenderTexture(WIDTH,HEIGHT);

    while (!WindowShouldClose()) {
        // #region update
        Vector2 mousePos = GetMousePosition();
        mousePos.x/=2;
        mousePos.y/=2;
        Vector2 mouseDelta = GetMouseDelta();
        mouseDelta.x/=2;
        mouseDelta.y/=2;

        if (window.width<=0) window.width=1;
        if (window.height<=0) window.height=1;

        // This is before drag detection because we want to start the movement the frame after the click.
        // This is because on a touchscreen, you simultaneously move and click when you tap it, which leads to the window moving every time you tap it.
        if (isDraggingGraph) {
            window.x-=mouseDelta.x*window.width /WIDTH /(IsKeyDown(KEY_LEFT_SHIFT) ? 5 : 1);
            window.y+=mouseDelta.y*window.height/HEIGHT/(IsKeyDown(KEY_LEFT_SHIFT) ? 5 : 1);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mousePos.x>equationWindowSize+EQ_WINDOW_HANDLE_SIZE/2+1) {
            isDraggingGraph=true;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isDraggingGraph=false;
        }

        if (CheckCollisionPointRec(mousePos,{(float)equationWindowSize-EQ_WINDOW_HANDLE_SIZE/2,0,EQ_WINDOW_HANDLE_SIZE,HEIGHT})) {
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
            equationWindowSize=mousePos.x;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mousePos.x<=equationWindowSize) {
            for (int i=0;i<equations.size();i++) {
                if (CheckCollisionPointRec(mousePos,getEquationRect(i))) {
                    focusedEq=i;
                }
            }
        }

        // int key=GetKeyPressed();
        if (IsKeyPressed(KEY_BACKSPACE) && focusedEq>=0) {
            if (cursor>0) {
                equations[focusedEq].str.erase(cursor-1,1);
                cursor--;
            } else if (equations.size()>1) {
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
        
        // #endregion

        // #region drawing
        BeginTextureMode(scr);

        ClearBackground(RAYWHITE);
        
        // #region grid

        for (float x = ceilBy(window.x+equationWindowSize*PIX_TO_UNIT,getGridUnit()); x < windowRight; x += getGridUnit()) {
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
            DrawText(text,textPoint.x+5,textPoint.y+5,10,BLACK);
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
            DrawText(text,textPoint.x+5,textPoint.y+5,10,BLACK);
        }

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

        // #endregion

        // #region equations
        double x{ 0 };
        double t{ GetTime() };
        std::vector<te_variable> vars = {{"x", &x},{"t", &t}};

        for (size_t e=0;e<equations.size();e++) {
            equation eq=equations.at(e);

            if (eq.str.size()==0) {
                equations[e].error=-1;
                continue;
            }

            std::smatch match;
            if (std::regex_search(eq.str,match,varDefRegex)) {
                std::string varName(match[1]);
                bool exists=false;
                for (auto var : vars) {
                    if (std::string(var.m_name.c_str())==varName) {
                        exists = true;
                        break;
                    }
                }
                if (exists) {
                    equations[e].error=-2;
                    continue;
                }
                std::string sliced=eq.str.substr(varName.size()+1);
                auto result = tep.evaluate(sliced.c_str());

                if (!tep.success()) {
                    equations[e].error=tep.get_last_error_position();
                    continue;
                } else {
                    equations[e].error=-1;
                }

                vars.push_back({varName.c_str(),result});
            } else {
                if (eq.str.rfind("y=", 0) == 0) {
                    eq.str=eq.str.substr(2,eq.str.size()-2);
                }

                tep.set_variables_and_functions(vars);

                auto result = tep.evaluate(eq.str.c_str());

                if (!tep.success()) {
                    equations[e].error=tep.get_last_error_position();
                    continue;
                } else {
                    equations[e].error=-1;
                }

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
        }
        std::string varsString("[ ");
        for (auto& var : tep.get_variables_and_functions()) {
            if (var.m_name=="x") continue;
            double value=tep.evaluate(var.m_name.c_str());
            varsString.append(TextFormat("(%s: %.4g) ",var.m_name.c_str(),value));
        }
        varsString.append("]");

        // #endregion

        drawEquationsWindow(mousePos);

        DrawText(varsString.c_str(),30,HEIGHT-20,10,BLACK);


        DrawLineEx(sub(mousePos,mouseDelta),mousePos,3,cursorColor);
        DrawCircle(mousePos.x,mousePos.y,2,cursorColor);
        DrawText(TextFormat("%d",GetFPS()),10,HEIGHT-10-10,10,BLACK);
        // DrawText(TextFormat("(%.2f,%.2f) %.2fx%.2f",window.x,window.y,window.width,window.height),10,25,10,BLACK);

        EndTextureMode();

        // #endregion

        BeginDrawing();
        DrawTexturePro(scr.texture, (Rectangle) { 0,0,(float)scr.texture.width, (float)-scr.texture.height }, (Rectangle) {0,0,WIDTH*2,HEIGHT*2}, (Vector2) { 0, 0 }, 0, WHITE);
        EndDrawing();

    }
    UnloadRenderTexture(scr);

    CloseWindow();
}
