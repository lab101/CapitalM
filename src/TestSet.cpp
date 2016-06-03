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



void TestSet::setup(Rectf screen){
    
    float f = 1;
    mScreen = screen;

    
    for(int i=0; i < 3; ++i){
        emmitters.push_back(Emitter(vec2(50,300*i),f));
        emmitters.push_back(Emitter(vec2(350,300*i),f));
        emmitters.push_back(Emitter(vec2(700,300*i),f));

    }
    
    
    
//    emmitters.push_back(Emitter(vec2(100,mScreen.getHeight()-10),f));
//    emmitters.push_back(Emitter(vec2(500,mScreen.getHeight()-10),f));
//    emmitters.push_back(Emitter(vec2(50,10),f));
//    emmitters.push_back(Emitter(vec2(450,10),f));
//    emmitters.push_back(Emitter(getWindowCenter(),f));
    
    
    
    
    Dot d1;
    d1.setup(vec2(20,screen.getHeight()-300), vec2(240,140), ci::Color(0, 1, 1));
    dots.push_back(d1);
    
	Dot d2;
	d2.setup(vec2(120, screen.getHeight() - 300), vec2(540, 140), ci::Color(0, 0, 1));
	dots.push_back(d2);
	
	Dot d3;
	d3.setup(vec2(420, mScreen.getHeight() - 30), vec2(440, 240), ci::Color(0.5, 1, 0));
	dots.push_back(d3);
//
//	Dot d4;
//	d4.setup(vec2(420, mScreen.getHeight(); - 80), vec2(440, 100), ci::Color(1.0, 0.2, 0));
//	dots.push_back(d4);
//
//
//	Dot d5;
//	d5.setup(vec2(20, mScreen.getHeight(); - 280), vec2(140, 400), ci::Color(1.0, 0.5, 0.3));
//	dots.push_back(d5);

    
    

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
    
    
    isHitTarget = true;
	float combinedDistance = 0;

    
    for(int i = 0; i < emmitters.size();++i){
        
        Emitter* e = &emmitters[i];
        int dataIndex = getElapsedFrames() % emmitterData.data.size();
        e->mTargetForce = emmitterData.data[dataIndex][i];
        e->update();
    }
    

    for(auto& d : dots){
        checkBounderies(d);
        applyForces(d,0);


		float distance = glm::distance(d.mPosition, d.mTargetPosition);
		combinedDistance += distance;


        isHitTarget *= distance < 10;
        limitSpeed(d);
        
        d.mVelocity = vLerp(d.mVelocity, vec2(0,0), 0.09);
        d.mDirection = vLerp(d.mDirection, gravity, 0.1);

        d.update();
    }
    
	if (combinedDistance < recordDistance) recordDistance = combinedDistance;

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
    if(dot.mPosition.y > mScreen.getHeight() || dot.mPosition.y < 0){
        dot.mDirection.y = -(dot.mDirection.y);
        dot.mVelocity.y = -(dot.mVelocity.y);
    }
    
    
    // moved outside on the left or right
    if(dot.mPosition.x > mScreen.getWidth() || dot.mPosition.x < 0){
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
        
        forceFactor = pow(forceFactor, 2);
        //  forceFactor *= 1.6;
        // cout << forceFactor << endl;
        
        dot.mDirection -= normalize(distance) * forceFactor;
    }

}



void TestSet::draw(int textOffset,bool background){

    
    //lmap<float>(recordDistance, 400, 0, 0.1, 3);
	
	//if (lineWidth < 0) lineWidth = 0.1;


	//float alpha = lmap<float>(recordDistance, 2000, 0, 0.1, 1);
	//gl::lineWidth(14);

    if(!background){
        gl::color(1, 1, 1, 0.1);

        for(auto& e : emmitters){
            gl::drawSolidCircle(e.mPosition, fmax(e.mForce,4));
        }
    }
    
    for(int i = 0; i < dots.size(); ++i){
        
        if(i > 0 ){
            gl::color(1, 1, 1, background ? 0.4 : 1);
            gl::drawLine(dots[i-1].mPosition, dots[i].mPosition);
        }
        
		dots[i].draw(!background);
    }


    
	if (recordDistance< 250) gl::drawString("l:" + to_string((int) recordDistance), vec2(800, 10 + textOffset));
 //   gl::drawString("d:" + to_string(recordDistance), vec2(50,10+textOffset));
    
    int f = fitness*10000000000000;
    if(f>0) gl::drawString(to_string(f), vec2(900,10+textOffset));
    
    
    gl::drawStrokedRect(mScreen, 1);
}




void TestSet::randomize(int emmitterAmount,int frames){
    
    
    ci::Perlin perlin = Perlin( 4,  clock() & 65535 );
;
    int const eSize = emmitters.size();// emmitters.size();
    
    for (float i=0; i< frames; ++i) {
        
        vector<float> emitterForces;
        emitterForces.reserve(eSize);
        
        for(float j=0; j< eSize;++j){
            float n = fabs(perlin.noise(i * 0.01f, j ));
            
            emitterForces.push_back(fabs(n) * 320.0f);
        }
        
        emmitterData.data.push_back(emitterForces);
    }
}




long double TestSet::calcuclateFitnessScore(){
    fitness= 0;
   // for(auto&d :dots){
    //    if (d.recordDistance < 1) d.recordDistance = 1;
        
        // Reward finishing faster and getting close
		//lifeTime *= 0.1;
        fitness = (1.0f/(recordDistance*lifeTime));
        
        
        //if (hitObstacle) fitness *= 0.1; // lose 90% of fitness hitting an obstacle
  //  }

    // Make the function exponential
    fitness = pow(fitness, 4);

	//fitness *= 0.01;

    if (isHitTarget) fitness *= 2.0; // twice the fitness for finishing!

    
    return fitness;

}

