//
//  TestSet.cpp
//  BabyM
//
//  Created by Kris Meeusen on 31/05/16.
//
//

#include "TestSet.hpp"
#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace std;
using namespace ci;
using namespace ci::app;

void TestSet::setup(){
    float f = 1;

    emmitters.push_back(Emitter(vec2(100,getWindowHeight()-10),f));
    emmitters.push_back(Emitter(vec2(500,getWindowHeight()-10),f));
    emmitters.push_back(Emitter(vec2(50,10),f));
    emmitters.push_back(Emitter(vec2(450,10),f));
    emmitters.push_back(Emitter(getWindowCenter(),f));
    
    
    
    
    Dot d1;
    d1.setup(vec2(20,getWindowHeight()-300), vec2(240,40), ci::Color(0, 1, 1));
    dots.push_back(d1);
    
	Dot d2;
	d2.setup(vec2(120, getWindowHeight() - 300), vec2(340, 40), ci::Color(0, 0, 1));
	dots.push_back(d2);
	
	Dot d3;
	d3.setup(vec2(420, getWindowHeight() - 30), vec2(440, 240), ci::Color(0.5, 1, 0));
	dots.push_back(d3);
	
	Dot d4;
	d4.setup(vec2(420, getWindowHeight() - 80), vec2(440, 100), ci::Color(1.0, 0.2, 0));
	dots.push_back(d4);


	Dot d5;
	d5.setup(vec2(20, getWindowHeight() - 280), vec2(140, 400), ci::Color(1.0, 0.5, 0.3));
	dots.push_back(d5);

    
    

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
    isHitTarget = false;
    lifeTime = 0;
	recordDistance = 10000;
    for (auto& d : dots){
        d.resetForces();
        d.resetPosition();
    }
         
         

}



void TestSet::stop(){
    isRunning = false;
    calcuclateFitnessScore();
}




void TestSet::update(vec2& gravity,vec2& target){
    
    
    if(!isRunning) return;
    
    ++lifeTime;
    
    
  //  isHitTarget = true;
	float combinedDistance = 0;

    for(auto& d : dots){
        int dataIndex = getElapsedFrames() % emmitterData.data.size();
        checkBounderies(d);
        applyForces(d,dataIndex);


		float distance = glm::distance(d.mPosition, d.mTargetPosition);
		combinedDistance += distance;


       // isHitTarget *= checkTarget(d);
        limitSpeed(d);
        
        d.mVelocity = vLerp(d.mVelocity, vec2(0,0), 0.09);
        d.mDirection = vLerp(d.mDirection, gravity, 0.1);

        d.update();
    }
    
	if (combinedDistance < recordDistance) recordDistance = combinedDistance;
	if (combinedDistance < 50) isHitTarget = true;

    if(isHitTarget) stop();
    
}



void TestSet::limitSpeed(Dot& dot){
    if(length(dot.mVelocity) > 20){
        vec2 n = normalize(dot.mVelocity);
        dot.mVelocity = n * 20.0f;
    }
}


bool TestSet::checkTarget(Dot& dot){

	/*  float distance = glm::distance(dot.mPosition, dot.mTargetPosition);

	  if(distance < dot.recordDistance){
	  dot.recordDistance = distance;
	  }


	  return (distance < 20);    */
	return 0;
}


void TestSet::checkBounderies(Dot& dot){
    
    // hit the floor
    if(dot.mPosition.y > getWindowHeight() || dot.mPosition.y < 0){
        dot.mDirection.y = -(dot.mDirection.y);
        dot.mVelocity.y = -(dot.mVelocity.y);
    }
    
    
    // moved outside on the left or right
    if(dot.mPosition.x > getWindowWidth() || dot.mPosition.x < 0){
        // stop();
        dot.mDirection.x = -dot.mDirection.x;
        dot.mVelocity.x = -dot.mVelocity.x;
    }
}




void TestSet::applyForces(Dot& dot,int dataIndex){
    
    for(int i = 0; i < emmitters.size();++i){
        
        Emitter* e = &emmitters[i];
        e->mForce = emmitterData.data[dataIndex][i];
        
        vec2 distance(e->mPosition - dot.mPosition);
        float length  = glm::length(distance);
        
        
        float forceFactor = lmap<float>(length, 0, e->mForce, 2.0, 1);
        forceFactor = fmax(forceFactor,0);
        
        forceFactor = pow(forceFactor, 2);
        //  forceFactor *= 1.6;
        // cout << forceFactor << endl;
        
        dot.mDirection -= normalize(distance) * forceFactor;
    }

}



void TestSet::draw(int textOffset){

    gl::color(0, 1, 1,0.08);

	float lineWidth = lmap<float>(recordDistance, 400, 0, 0.1, 3);
	
	if (lineWidth < 0) lineWidth = 0.1;


	float alpha = lmap<float>(recordDistance, 2000, 0, 0.1, 1);
	gl::lineWidth(lineWidth);

    for(auto& e : emmitters){
        gl::drawStrokedCircle(e.mPosition, fmax(e.mForce,4));
    }
    
    for(int i = 0; i < dots.size(); ++i){
        
        if(i>0){
			gl::color(0, 1, 1, alpha*0.5);
            gl::drawLine(dots[i-1].mPosition, dots[i].mPosition);
        }
        
		dots[i].draw(alpha);
    }


    
	if (recordDistance< 250) gl::drawString("l:" + to_string((int) recordDistance), vec2(10, 10 + textOffset));
 //   gl::drawString("d:" + to_string(recordDistance), vec2(50,10+textOffset));
    
    int f = fitness*10000000000000;
    if(f>0) gl::drawString(to_string(f), vec2(50,10+textOffset));
}




void TestSet::randomize(int emmitterAmount,int frames){
    
    int const eSize = emmitters.size();// emmitters.size();
    for (int i=0; i< frames; ++i) {
        
        vector<float> emitterForces;
        emitterForces.reserve(eSize);
        
        for(int j=0; j< eSize;++j){
            emitterForces.push_back(ci::randFloat(0, 180));
        }
        
        emmitterData.data.push_back(emitterForces);
    }
}




long double TestSet::calcuclateFitnessScore(){
    fitness= 0;
   // for(auto&d :dots){
    //    if (d.recordDistance < 1) d.recordDistance = 1;
        
        // Reward finishing faster and getting close
		lifeTime *= 0.1;
        fitness = (1.0f/(recordDistance));
        
        
        //if (hitObstacle) fitness *= 0.1; // lose 90% of fitness hitting an obstacle
  //  }

    // Make the function exponential
    fitness = pow(fitness, 4);

	fitness *= 0.01;

    if (isHitTarget) fitness *= 2.0; // twice the fitness for finishing!

    
    return fitness;

}

