#pragma once
#include <iostream>
#include <cmath>

class Vector2f{
public:
    Vector2f(){
        x=0;
        y=0;
    };

    Vector2f(float _x, float _y){
        x=_x;
        y=_y;
    };

    Vector2f(float ang){
        x=cos(ang);
        y=sin(ang);
    };

    ~Vector2f(){
    };

	Vector2f GetNorm() {
		return (Vector2f(x*(1.0f/Length()), y*(1.0f / Length())));
	};

	Vector2f GetPerp()
	{
		return Vector2f((-1)*y / Length(), x / Length());
	};

    void Print(){
        std::cout<<"["<<x<<", "<<y<<"]"<<std::endl;
    };

	float Length() {
		return (sqrt(x*x + y*y));
	};

	float x, y;
};


Vector2f operator+(Vector2f v1, Vector2f v2);

Vector2f operator-(Vector2f v1, Vector2f v2);

float operator*(Vector2f v1, Vector2f v2);

Vector2f operator*(Vector2f v1, float number);

Vector2f operator*(float number, Vector2f v1);

float operator^(Vector2f v1, Vector2f v2);
