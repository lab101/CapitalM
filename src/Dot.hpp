//
//  Dot.hpp
//  BabyM
//
//  Created by Kris Meeusen on 31/05/16.
//
//

#ifndef Dot_hpp
#define Dot_hpp

#include <stdio.h>


class Dot{
    
public:
    
    ci::Color mColor;
    ci::vec2 mPosition;
    ci::vec2 mVelocity;
    ci::vec2 mDirection;
    
    ci::vec2 mStartPosition;
    ci::vec2 mTargetPosition;
    
    bool isHitTarget = false;
    
    float recordDistance = 1000;
    
    void setup(ci::vec2 start,ci::vec2 end,ci::Color color);
    void update();
    void draw();
    
    void resetPosition();
    void resetForces();
    
};

#endif /* Dot_hpp */
