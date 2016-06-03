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
    
    
    lerpFalloffForce    = 0.001;
    lerpTargetForce     = 0.2;
    isBackgroundDrawingOff = true;
   // addSetting(&lerpFalloffForce);
}






