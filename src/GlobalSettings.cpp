//
//  GlobalSettings.cpp
//  InnovationWall
//
//  Created by Kris Meeusen on 01/02/16.
//
//

#include <stdio.h>
#include "GlobalSettings.h"

#include "cinder/Utilities.h"

using namespace std;
using namespace ci;


GlobalSettings::GlobalSettings(){
    
    
    lerpFalloffForce        = 0.001;
    lerpTargetForce         = 0.2;
    isBackgroundDrawingOff  = true;
    maxSpeed                = 7;
    gravity                 = vec2(0,0.06);
    lerpBallVelocity        = 0.02;
    
   // addSetting(&lerpFalloffForce);
}





