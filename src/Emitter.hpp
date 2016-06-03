//
//  Emitter.hpp
//  BabyM
//
//  Created by Kris Meeusen on 31/05/16.
//
//

#ifndef Emitter_hpp
#define Emitter_hpp

#include <stdio.h>
#include "cinder/Vector.h"
#include "cinder/CinderMath.h"


class Emitter{
    
public:
    ci::vec2 mPosition;
    float mForce = 0;
    float mTargetForce = 0;

    
    Emitter(ci::vec2 position,float force){
        mPosition = position;
        mForce = force;
    }
    
    void setup();
    void update(){
        
        mForce  = ci::lerp<float>(mForce,mTargetForce,0.2);
        
        mForce  = ci::lerp<float>(mForce,0,0.02);
//        if(mForce > 0)
    }
    
};
#endif /* Emitter_hpp */
