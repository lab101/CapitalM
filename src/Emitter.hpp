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


class Emitter{
    
public:
    ci::vec2 mPosition;
    float mForce = 0;
    
    Emitter(ci::vec2 position,float force){
        mPosition = position;
        mForce = force;
    }
    
    void setup();
    void udpate();
    
};
#endif /* Emitter_hpp */
