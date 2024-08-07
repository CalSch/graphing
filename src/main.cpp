#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include "util.h"
#include "tinyexpr.h"
#include "defines.h"
#include "equations.h"

// #define WIDTH  640
// #define HEIGHT 480
#define PIX_TO_UNIT float(window.width)/float(WIDTH)
#define windowRight window.x+window.width
#define windowBottom window.y+window.height

RenderTexture2D scr;
RenderTexture2D gridTex;
Color cursorColor=BLUE;

bool windowMoved=true;
float graphStep=1;

std::regex eqLineRegex("(\\d{1,3}) (\\d{1,3}) (\\d{1,3}) (.*)");

// #region window
Rectangle window={-10.f,-10*RATIO,20.f,20.f*RATIO};
void scaleWindow(float v) {
    windowMoved=true;
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


//#endregion

// #region equations

EquationWindow eqWin;

void save() {
    std::string s="";
    for (auto eq : eqWin.equations) {
        s+=TextFormat("%d %d %d %s\n",eq.color.r,eq.color.g,eq.color.b,eq.str.c_str());
    }

    std::ofstream out("equations.txt");
    out << s;
    out.close();
}
void load() {
    eqWin.equations={};
    std::ifstream in("equations.txt");
    std::string line;
    if (in.is_open()) {
        while (getline(in,line)) {
            std::smatch m;
            if (std::regex_search(line,m,eqLineRegex)) {
                std::string eqStr = m[4];
                for (int i=0;i<m.size();i++) {
                    auto s=m[i];
                    printf("'%s' ",s.str().c_str());
                }
                printf("\n");
                Editor ed(&eqStr);
                eqWin.equations.push_back((equation){
                    eqStr, (Color){
                        std::stoi(m[1]),
                        std::stoi(m[2]),
                        std::stoi(m[3]),
                        255
                    }, -1, ed
                });
            } else {
                throw "aaaaa help!";
            }
        }
        in.close();
    }
}

//#endregion

te_parser tep;


bool isDraggingEQWindow=false;
bool isDraggingGraph=false;

const std::regex varDefRegex=std::regex("^(([a-zA-Z])[a-zA-Z0-9_]*)=");


void drawGrid() {
    BeginTextureMode(gridTex);

    ClearBackground(BLANK);

    for (float x = ceilBy(window.x+eqWin.width*PIX_TO_UNIT,getGridUnit()); x < windowRight; x += getGridUnit()) {
        DrawLineEx(
            {translate(x,window.x,windowRight,0,WIDTH),0},
            {translate(x,window.x,windowRight,0,WIDTH),HEIGHT},
            1,
            GRAY
        );
        Vector2 textPoint=translatePoint({x,0});
        auto text=TextFormat("%.6g",x);
        textPoint.x=clamp(textPoint.x,eqWin.width,WIDTH-MeasureText(text,10)-10);
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
        textPoint.x=clamp(textPoint.x,eqWin.width,WIDTH-MeasureText(text,10)-10);
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

    EndTextureMode();
}

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
    gridTex=LoadRenderTexture(WIDTH,HEIGHT);

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

        // #region window dragging

        // This is before drag detection because we want to start the movement the frame after the click.
        // This is because on a touchscreen, you simultaneously move and click when you tap it, which leads to the window moving every time you tap it.
        if (isDraggingGraph) {
            windowMoved=true;
            window.x-=mouseDelta.x*window.width /WIDTH /(IsKeyDown(KEY_LEFT_SHIFT) ? 5 : 1);
            window.y+=mouseDelta.y*window.height/HEIGHT/(IsKeyDown(KEY_LEFT_SHIFT) ? 5 : 1);
        }

        // Detect window dragging
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mousePos.x>eqWin.width+EQ_WINDOW_HANDLE_SIZE/2+1) {
            isDraggingGraph=true;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isDraggingGraph=false;
        }
        // #endregion

        // #region equation window update

        // Detect equation window dragging
        if (CheckCollisionPointRec(mousePos,{(float)eqWin.width-EQ_WINDOW_HANDLE_SIZE/2,0,EQ_WINDOW_HANDLE_SIZE,HEIGHT})) {
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
            windowMoved=true;
            eqWin.width=max(0,mousePos.x);
        }

        eqWin.update(mousePos);

        // #endregion

        if (abs(GetMouseWheelMove())!=0)
            scaleWindow(-GetMouseWheelMove());

        if (windowMoved) {
            drawGrid();
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
            save();
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L)) {
            load();
        }
        
        // #endregion

        // #region drawing
        BeginTextureMode(scr);

        ClearBackground(RAYWHITE);
        
        // #region grid

        // DrawTexture(gridTex.texture,0,0,WHITE);
        DrawTexturePro(
            gridTex.texture,
            (Rectangle) {
                0,0,
                (float)gridTex.texture.width, (float)-gridTex.texture.height
            },
            (Rectangle) {
                0,0,
                WIDTH,HEIGHT
            },
            (Vector2) { 0, 0 }, 0, WHITE
        );


        // #endregion

        // #region equations
        double x{ 0 };
        double t{ GetTime() };
        std::vector<te_variable> vars = {{"x", &x},{"t", &t}};
        std::vector<te_variable> user_vars = {};

        for (size_t e=0;e<eqWin.equations.size();e++) {
            equation eq=eqWin.equations.at(e);

            std::string str = eq.str;

            if (str.size()==0) {
                eqWin.equations[e].error=-1;
                continue;
            }

            bool useX = false;
            if (str.rfind("x=",0) == 0) {
                str=str.substr(2,str.size()-2);
                useX=true;
            }

            if (useX) {
                vars = {{"y", &x},{"t", &t}};
            } else {
                vars = {{"x", &x},{"t", &t}};
            }

            std::smatch match;
            if (std::regex_search(str,match,varDefRegex)) {
                std::string varName(match[1]);
                bool exists=false;
                for (auto var : vars) {
                    if (std::string(var.m_name.c_str())==lowerText(varName)) {
                        exists = true;
                        break;
                    }
                }
                if (exists) {
                    eqWin.equations[e].error=-2;
                    continue;
                }
                std::string sliced=str.substr(varName.size()+1);
                auto result = tep.evaluate(sliced.c_str());

                if (!tep.success()) {
                    eqWin.equations[e].error=tep.get_last_error_position();
                    continue;
                } else {
                    eqWin.equations[e].error=-1;
                }

                user_vars.push_back({varName.c_str(),result});
            } else {
                if (str.rfind("y=", 0) == 0) {
                    str=str.substr(2,str.size()-2);
                }

                std::vector<te_variable> new_vars;
                for (auto var : vars) {
                    new_vars.push_back(var);
                }
                for (auto var : user_vars) {
                    new_vars.push_back(var);
                }
                tep.set_variables_and_functions(new_vars);
                // tep.set_variables_and_functions(user_vars);

                auto result = tep.evaluate(str.c_str());

                if (!tep.success()) {
                    eqWin.equations[e].error=tep.get_last_error_position();
                    continue;
                } else {
                    eqWin.equations[e].error=-1;
                }

                Vector2 lastPoint=useX ? (Vector2){0,window.y-1} : (Vector2){window.x-1,0};
                for (double i=0; i<=double(useX ? HEIGHT : WIDTH); i+=graphStep) {
                    x=i/double(useX ? HEIGHT : WIDTH);
                    x*=double(useX ? window.height : window.width);
                    x+=double(useX ? window.y : window.x);
                    // printf("x=%f %f %f\n",x,window.x,windowRight);
                    // WaitTime(0.1);
                    Vector2 point=useX ? (Vector2){tep.evaluate(),x} : (Vector2){x,tep.evaluate()};
                    DrawLineEx(translatePoint(lastPoint),translatePoint(point),1,eq.color);
                    lastPoint=point;
                }
            }
        }
        std::string varsString("[ ");
        for (auto& var : tep.get_variables_and_functions()) {
            if (var.m_name=="x" || var.m_name=="y") continue;
            double value=tep.evaluate(var.m_name.c_str());
            varsString.append(TextFormat("(%s: %.4g) ",var.m_name.c_str(),value));
        }
        varsString.append("]");

        // #endregion

        eqWin.draw(mousePos);

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

        windowMoved=false;

    }
    UnloadRenderTexture(scr);

    CloseWindow();
}
