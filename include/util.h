#pragma once
#include <raymath.h>
float max(float x, float y);
float min(float x, float y);

float translate(float x, float a, float b, float c, float d);
float roundBy(float x, float y);
float floorBy(float x, float y);
float ceilBy(float x, float y);
float clamp(float x, float min_, float max_);


Vector2 add(Vector2 v1,Vector2 v2);
Vector2 sub(Vector2 v1,Vector2 v2);
Vector2 mul(Vector2 v1,Vector2 v2);
Vector2 div(Vector2 v1,Vector2 v2);