//
//  TestSet.cpp
//  BabyM
//
//  Created by Kris Meeusen on 31/05/16.
//
//

#include "TestSet.hpp"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace std;
using namespace ci;
using namespace ci::app;



void TestSet::setup(){
    
    float f = 1;
    
    float const offset = 324;
    for(int i=0; i < 3; ++i){
        emmitters.push_back(Emitter(vec2(38,offset + 260 * i),f));
        emmitters.push_back(Emitter(vec2(451,offset + 260 * i),f));
        emmitters.push_back(Emitter(vec2(848,offset + 260 * i),f));
        
    }
    
    
    emmitters.push_back(Emitter(vec2(451,100),f));
    
    ci::Color c(1.0, 0.2, 0);
    
    float bottom = 873;
    float top = 237;
    float left = 207;
    float right = 687;
    
    // INNER M
    
    //estra flap left
    dots.push_back(Dot(vec2(32, 04),  vec2(354, bottom), c));
    dots.push_back(Dot(vec2(284, 104),   vec2(354, 591), c));
    dots.push_back(Dot(vec2(384, 84),   vec2(372, 611), c));
    
    dots.push_back(Dot(vec2(652, 104),  vec2(372, 748), c));
    dots.push_back(Dot(vec2(452, 144),  vec2(520, 748), c));
    dots.push_back(Dot(vec2(184, 204),   vec2(520, 611), c));
    
    dots.push_back(Dot(vec2(284, 204),   vec2(540, 591), c));
    dots.push_back(Dot(vec2(352, 304),  vec2(540, bottom), c));
    
    // right under
    dots.push_back(Dot(vec2(500, 507),  vec2(right, bottom), c));
    
    // right top
    dots.push_back(Dot(vec2(670, 107),  vec2(right, top), c));
    
    // middle
    dots.push_back(Dot(vec2(660, 107),  vec2(446, 478), c));
    
    
    // left top
    dots.push_back(Dot(vec2(520, 107),  vec2(left, top), c));
    
    // left bottom
    dots.push_back(Dot(vec2(550, 107),  vec2(left, bottom), c));
    
    
    //OUTER M
    
    left = 182;
    top = 178;
    right = 711;
    // left bottom
    dots2.push_back(Dot(vec2(200, 107),  vec2(left, bottom), c));
    // left top
    dots2.push_back(Dot(vec2(524, 100),  vec2(left, top), c));
    // middle
    dots2.push_back(Dot(vec2(660, 107),  vec2(446, 444), c));
    // right top
    dots2.push_back(Dot(vec2(670, 107),  vec2(right, top), c));
    // right under
    dots2.push_back(Dot(vec2(500, 507),  vec2(right, bottom), c));

    
    
    
}



void TestSet::setNewData(EmitterData e){
    emmitterData = e;
}


vec2 vLerp(vec2 p1,vec2 p2,float v){
    
    vec2 r;
    r.x = lerp(p1.x,p2.x,v);
    r.y = lerp(p1.y,p2.y,v);
    
    return r;
    
}



void TestSet::start(){
    
    isRunning = true;
    isHitAllTargets = false;
    lifeTime = 0;
    recordDistance = 10000;
    
    for (auto& d : dots){
        d.resetForces();
        d.resetPosition();
    }
    
    for (auto& d : dots2){
        d.resetForces();
        d.resetPosition();
    }

    
    for(auto&e : emmitters){
        e.reset();
    }
    
    
    
}



void TestSet::stop(){
    isRunning = false;
    calculateFitness();
}


void TestSet::updateEmitters(bool cooldown){
    for(int i = 0; i < emmitters.size();++i){
        
        Emitter* e = &emmitters[i];
        e->mTargetForce = cooldown ?  0 : emmitterData.data[lifeTime][i];
        e->update();
    }
}


float TestSet::updateDots(std::vector<Dot>& dots,const ci::vec2& gravity){
    float combinedDistance = 0;

    for(auto& d : dots){
        checkBounderies(d);
        applyForces(d,0);
        
        
        float distance = glm::distance(d.mPosition, d.mTargetPosition);
        combinedDistance += distance;
        
        d.isHitTarget = distance < GS()->lockLimit;
        
        // snap when in replay
        if(d.isHitTarget && GS()->isReplay){
            d.mPosition = d.mTargetPosition;
        }
        
        isHitAllTargets *= d.isHitTarget;
        limitSpeed(d);
        
        d.mVelocity = vLerp(d.mVelocity, vec2(0,0), GS()->lerpBallVelocity);
        d.mDirection = vLerp(d.mDirection, gravity, 0.1);
        
        d.update();
    }
    
    
    return combinedDistance;

}



void TestSet::update(vec2& gravity){
    
    
    if(!isRunning) return;
    
    
    isHitAllTargets = true;
    float combinedDistance = 0;
    
    
    combinedDistance += updateDots(dots, gravity);
    combinedDistance += updateDots(dots2, gravity);
    
    if (combinedDistance < recordDistance) recordDistance = combinedDistance;
    
    if(isHitAllTargets) stop();
    
    
    ++lifeTime;
    
    
}



void TestSet::limitSpeed(Dot& dot){
    if(length(dot.mVelocity) > GS()->maxSpeed){
        vec2 n = normalize(dot.mVelocity);
        dot.mVelocity = n *  GS()->maxSpeed;
    }
}


bool TestSet::checkTarget(Dot& dot){
    return 0;
}


void TestSet::checkBounderies(Dot& dot){
    
    // hit the floor
    if(dot.mPosition.y > GS()->mScreen.getHeight() || dot.mPosition.y < 0){
        dot.mDirection.y = -(dot.mDirection.y);
        dot.mVelocity.y = -(dot.mVelocity.y);
    }
    
    
    // moved outside on the left or right
    if(dot.mPosition.x > GS()->mScreen.getWidth() || dot.mPosition.x < 0){
        // stop();
        dot.mDirection.x = -dot.mDirection.x;
        dot.mVelocity.x = -dot.mVelocity.x;
    }
}




void TestSet::applyForces(Dot& dot,int dataIndex){
    
    for(int i = 0; i < emmitters.size();++i){
        
        Emitter* e = &emmitters[i];
        // e->mForce = emmitterData.data[dataIndex][i];
        
        vec2 distance(e->mPosition - dot.mPosition);
        float length  = glm::length(distance);
        
        
        float forceFactor = lmap<float>(length, 0, e->mForce, 2.0, 1);
        forceFactor = fmax(forceFactor,0);
        
        forceFactor = pow(forceFactor, 1.45);
        //  forceFactor *= 1.6;
        // cout << forceFactor << endl;
        
        dot.mDirection -= normalize(distance) * forceFactor;
    }
    
}


void TestSet::drawEmitters(std::shared_ptr<ci::nvg::Context> nvgContext){
    
    auto& vg = *nvgContext;
    
    //draw emmitters
    for(auto& e : emmitters){
        
        
      //  bool toggle = true;
        for(float f=0; f < e.mForce; f+= 26.0){
            vg.beginPath();
            
//            if(toggle){
//                vg.strokeColor(ColorAf{.0f, .8f, .9f});
//            }else{
//                vg.strokeColor(ColorAf{.0f, 7.0f, 1.f});
//            }
            
            
            
            float c = lmap<float>(f, 0, e.mForce, 0.5, 0.1);
            if(c < 0) c =0;

            vg.strokeColor(ColorAf{CM_HSV, c, .7f, 1.0f});

//            toggle = !toggle;
            
            float s = lmap<float>(f, 0, e.mForce, 26, 1);
            if(s < 0) s =1;
            vg.strokeWidth(s);
            
            vg.arc(e.mPosition , f,0, (M_PI * 2), NVG_CW);
            //   vg.closePath();
            vg.stroke();
            
            
            
        }
    }
}



void TestSet::drawConnections(std::shared_ptr<ci::nvg::Context> nvgContext,vector<Dot>& dots, float width){
    
    auto& vg = *nvgContext;
    
    
    for(int i = 0; i <= dots.size(); ++i){
        
        int pos1 = i % dots.size();
        int pos2 = (i+1) % dots.size();
        
        vg.strokeColor(ColorA(1,1,1,1));
        
        vg.beginPath();
        vg.strokeWidth(4);
        
        
        vec2 div = dots[pos2].mPosition - dots[pos1].mPosition;
        vec2 norm = glm::normalize(div);
        float length = glm::length(div);
        
        
        for (float i = 24; i < length-24; i+=8) {
            
            float c = lmap<float>(i, 0, length, 1.0, 0.1);
            if(c < 0) c =0;
            
            vg.strokeColor(ColorAf{CM_HSV, 0.2, c, .8f});

            vg.moveTo(dots[pos1].mPosition + (norm * i));
            vg.lineTo(dots[pos1].mPosition + (norm * (i+4)));
            
        }
        
        vg.stroke();
        
    }
}





void TestSet::drawDots(std::shared_ptr<ci::nvg::Context> nvgContext,vector<Dot>& dots,float radius){
    auto& vg = *nvgContext;
    
    float hue = 0;
    
    Color c;
    for(auto& d : dots){
        
        hue += 0.04;
        c = ci::hsvToRgb(vec3(hue,.8,1));
        // float s = lmap<float>(d.getSpeed(), 0, 6, 0, 20);
        vg.beginPath();
        vg.strokeWidth(4);
        
        vg.fillColor(c);
        vg.strokeColor(c);
        vg.fillColor(ColorAf{CM_HSV, hue, .4f, 1.0f});
        
        vg.arc(d.mPosition , 6, -M_PI * 0.5f,  M_PI * 2.0f, NVG_CW);
        vg.closePath();
        vg.stroke();
        
        
        vg.beginPath();
        
        vg.arc(d.mTargetPosition , 6, -M_PI * 0.5f,  M_PI * 2.0f, NVG_CW);
        vg.closePath();
        vg.stroke();
    }
}




void TestSet::randomize(int frames,int rndIndex){
    
    
    ci::Perlin perlin = Perlin( 4,  clock() & 65535 );
    ;
    int const eSize = emmitters.size();// emmitters.size();
    
    for (float i=0; i< frames; ++i) {
        
        vector<float> emitterForces;
        emitterForces.reserve(eSize);
        
        for(float j=0; j< eSize;++j){
            float n = fabs(perlin.noise(i * 0.01f, j + (frames*1000) ));
            
            emitterForces.push_back(fabs(n) * 260.0f);
        }
        
        emmitterData.data.push_back(emitterForces);
    }
}




long double TestSet::calculateFitness(){
    fitness = (1.0f/(recordDistance*lifeTime));
    
    // Make the function exponential
    fitness = pow(fitness, 4);
    if (isHitAllTargets) fitness *= 2.0; // twice the fitness for finishing!
    
    return fitness;
}


void TestSet::readData(std::string fileName){
    ifstream dataFile;
    dataFile.open (getAssetPath("").string() + "/" + fileName);
    
    string line;
    int const eSize = emmitters.size();// emmitters.size();
    
    if (dataFile.is_open())
    {
        while ( getline (dataFile,line) )
        {
            
            vector<float> emitterForces;
            emitterForces.reserve(eSize);
            
            vector<string> splitEmitters = split(line, ';');
            for(auto& s : splitEmitters){
                if(s!= "") emitterForces.push_back(stof(s));
            }
            
            emmitterData.data.push_back(emitterForces);
            
            
        }
        dataFile.close();
    }
    
}


void TestSet::dumpData(std::string fileName){
    ofstream dataFile;
    dataFile.open (getAssetPath("").string() + "/" + fileName);
    
    for(auto& timeFrame : emmitterData.data){
        string row;
        for(float emmiterForce : timeFrame){
            row += toString(emmiterForce) + ";";
        }
        
        dataFile << row << "\n";
        
    }
    
    dataFile.close();
}


