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



void Dot::draw(float alpha){

	
    gl::color(ColorA(mColor,alpha));
    gl::drawStrokedCircle(mPosition, 10);

  //  gl::color(1, 0, 0);
    gl::drawSolidCircle(mTargetPosition, 10);

}