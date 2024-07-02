#include "util.h"
#include <raymath.h>
#include <string>
#include <vector>

float max(float x, float y) {
    return x>y?x:y;
}
float min(float x, float y) {
    return x<y?x:y;
}

float translate(float x, float a, float b, float c, float d) {
    return (x-a)/(b-a)*(d-c)+c;
}
float roundBy(float x, float y) {
    return roundf(x/y)*y;
}
float floorBy(float x, float y) {
    return floorf(x/y)*y;
}
float ceilBy(float x, float y) {
    return ceilf(x/y)*y;
}
float clamp(float x, float min_, float max_) {
    return max(min(x,max_),min_);
}


Vector2 add(Vector2 v1,Vector2 v2) {return Vector2{v1.x+v2.x,v1.y+v2.y};}
Vector2 sub(Vector2 v1,Vector2 v2) {return Vector2{v1.x-v2.x,v1.y-v2.y};}
Vector2 mul(Vector2 v1,Vector2 v2) {return Vector2{v1.x*v2.x,v1.y*v2.y};}
Vector2 div(Vector2 v1,Vector2 v2) {return Vector2{v1.x/v2.x,v1.y/v2.y};}

std::string lowerText(std::string str) {
    std::string out="";
    for (int i=0;i<str.size();i++) {
        out.append(std::string(1,(char)tolower(str.at(i))));
    }
    return out;
}

std::vector<std::string> splitText(std::string str, char delimeter) {
    std::vector<std::string> list;
    std::string lastStr;
    for (char c : str) {
        if (c==delimeter) {
            list.push_back(lastStr);
            lastStr="";
        } else {
            lastStr.push_back(c);
        }
    }

    list.push_back(lastStr);
    return list;
}
