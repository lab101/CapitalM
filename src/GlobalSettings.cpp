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
    isBackgroundDrawingOff  = false;
    maxSpeed                = 10;
    gravity                 = vec2(0,0.06);
    lerpBallVelocity        = 0.02;
    isReplay                = false;
    mutation                = 0.0003;
    noDraw                  = false;
    testSetMax              = 200;
    lockLimit               = 10;

    // console run.
    noDraw                  = true;
    testSetMax              = 9200;

    
     //replay
//    isReplay                = true;
//    testSetMax              = 1;
//
//    replayFile = "_s0_emmitterData0.csv";

    
}






