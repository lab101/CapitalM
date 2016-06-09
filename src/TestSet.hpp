//
//  TestSet.hpp
//  BabyM
//
//  Created by Kris Meeusen on 31/05/16.
//
//

#ifndef TestSet_hpp
#define TestSet_hpp

#include <stdio.h>
#include <vector>

#include "Emitter.hpp"
#include "Dot.hpp"
#include "EmmiterData.hpp"
#include "cinder/Rect.h"
#include "ci_nanovg_gl.hpp"

class TestSet{
    
public:
    //std::vector<std::vector<float> > mData;
    EmitterData emmitterData;
    
    bool isRunning;
    bool isHitAllTargets;
    
    std::vector<Emitter> emmitters;
    std::vector<Dot> dots;
    std::vector<Dot> dots2;
    
    void randomize(int frames,int rndIndex);
    void setup();
    void update(cinder::vec2& gravity);
    
    void updateEmitters(bool cooldown=false);
    float updateDots(std::vector<Dot>& dots,const ci::vec2& gravity);
    
    
    void drawDots(std::shared_ptr<ci::nvg::Context> nvgContext,std::vector<Dot>& dots,float radius);
    void drawEmitters(std::shared_ptr<ci::nvg::Context> nvgContext);
    void drawConnections(std::shared_ptr<ci::nvg::Context> nvgContext,std::vector<Dot>& dots,float width);
    
    void setNewData(EmitterData e);
    void applyForces(Dot& d,int dataIndex);
    void checkBounderies(Dot& d);
    bool checkTarget(Dot& d);
    void limitSpeed(Dot& d);
    void addDot(ci::vec2 start,ci::vec2 target);
    
    void start();
    void stop();
    
    int lifeTime;
    
    long double fitness = 0;
    float recordDistance = 1000;
    
    long double calculateFitness();
    
    void dumpData(std::string fileName);
    void readData(std::string fileName);
    
    
};

#endif /* TestSet_hpp */
