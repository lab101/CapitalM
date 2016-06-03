#pragma once


#include "SettingManager.h"
#include "Singleton.hpp"
#include "cinder/Json.h"
#include "cinder/Text.h"
#include "cinder/BSpline.h"
#include "cinder/Vector.h"


class GlobalSettings{


    
public:


    ci::Rectf   mScreen;
    
    float     lerpTargetForce;
    float     lerpFalloffForce;
    bool      isBackgroundDrawingOff;
    ci::vec2  gravity;

	GlobalSettings();
    
    

};


typedef Singleton<GlobalSettings> GlobalSettingsSingleton;

inline GlobalSettings* GS(){
	return GlobalSettingsSingleton::Instance();
}