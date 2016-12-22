#include "Vector2f.h"

Vector2f operator+(Vector2f v1, Vector2f v2){
    return Vector2f(v1.x+v2.x,v1.y+v2.y);
};

Vector2f operator-(Vector2f v1, Vector2f v2) {
	return Vector2f(v1.x - v2.x, v1.y - v2.y);
};

float operator*(Vector2f v1, Vector2f v2){
    return v1.x*v2.y+v1.y*v2.x;
};

Vector2f operator*(Vector2f v1, float number){
    return Vector2f(v1.x*number,v1.y*number);
};

Vector2f operator*(float number, Vector2f v1){
    return Vector2f(v1.x*number,v1.y*number);
};

float operator^(Vector2f v1, Vector2f v2) {
	return 0.5 * (v1.x * v2.y - v1.y * v2.x);
};
