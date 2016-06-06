//
//  Dot.cpp
//  BabyM
//
//  Created by Kris Meeusen on 31/05/16.
//
//

#include "Dot.hpp"
#include "cinder/gl/gl.h"


using namespace ci;
using namespace std;

void Dot::setup(vec2 start,vec2 end,ci::Color color){
    mVelocity  = vec2(0,0);
    mDirection = vec2(0,0);
    mColor = color;
    
    mStartPosition = start;
    mTargetPosition = end;

}


void Dot::update(){
    
    mVelocity += mDirection;
    mPosition += mVelocity;
    
    
}


void Dot::resetPosition(){
    mPosition = mStartPosition;
}



void Dot::resetForces(){
    mVelocity = vec2(0,0);
    mDirection = vec2(0,0);
}


float Dot::getSpeed(){
    return length(mVelocity);
}



void Dot::draw(bool inColor){

    if(inColor){
        gl::color(Color(mColor));
    }else{
        gl::color(ColorA(mColor,1));

    }
    
    float lineWidth = inColor == true ? 8 : 1;
    float segments = inColor == true ? 8 : 4;
    gl::drawStrokedCircle(mPosition, 10,lineWidth,segments );

  //  gl::color(1, 0, 0);
    gl::drawSolidCircle(mTargetPosition, 10);

}