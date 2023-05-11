#include "util.h"
#include <raymath.h>

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
float clamp(float x, float min_, float max_) {
    return max(min(x,max_),min_);
}


Vector2 add(Vector2 v1,Vector2 v2) {return Vector2{v1.x+v2.x,v1.y+v2.y};}
Vector2 sub(Vector2 v1,Vector2 v2) {return Vector2{v1.x-v2.x,v1.y-v2.y};}
Vector2 mul(Vector2 v1,Vector2 v2) {return Vector2{v1.x*v2.x,v1.y*v2.y};}
Vector2 div(Vector2 v1,Vector2 v2) {return Vector2{v1.x/v2.x,v1.y/v2.y};}