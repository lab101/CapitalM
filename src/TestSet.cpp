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
    dots.push_back(Dot(vec2(500, 507),  vec2(687, bottom), c));
    
    // right top
    dots.push_back(Dot(vec2(670, 107),  vec2(687, top), c));
    
    // middle
    dots.push_back(Dot(vec2(660, 107),  vec2(446, 478), c));


    // left top
    dots.push_back(Dot(vec2(520, 107),  vec2(left, top), c));
    
    // left bottom
    dots.push_back(Dot(vec2(550, 107),  vec2(left, bottom), c));

    
    //    dots.push_back(Dot(vec2(352, 304),  vec2(520, 748), c));
    //    dots.push_back(Dot(vec2(84, 504),   vec2(520, 611), c));
    //    dots.push_back(Dot(vec2(84, 504),   vec2(542, 591), c));

   // dots.push_back(Dot(vec2(194, 604),  vec2(358, bottom), c));
    
    
//    //left under
//    dots.push_back(Dot(vec2(182, 704),  vec2(182, bottom), c));
//    
//    //top left
//    dots.push_back(Dot(vec2(304,704),   vec2(208,238), c));
//    
//    // middle
//    dots.push_back(Dot(vec2(420,704),   vec2(446, 478), c));
//    
//    // top right
//    dots.push_back(Dot(vec2(637, 704),  vec2(686, 240), c));
//    
//    // top right
//    dots.push_back(Dot(vec2(690, 704),  vec2(691, 872), c));
//    
//    
//    //estra flap right

    
//
//    // left under2
//    Dot d4;
//    d4.setup(vec2(84, 604), vec2(358, 748), ci::Color(1.0, 0.2, 0));
//    dots.push_back(d4);
//    
//
//    
//    // left underup1
//    Dot d42;
//    d42.setup(vec2(194, 604), vec2(358, 870), ci::Color(1.0, 0.2, 0));
//    dots.push_back(d42);
//
//
//    // left under
//    Dot d41;
//    d41.setup(vec2(184, 704), vec2(206, 868), ci::Color(1.0, 0.2, 0));
//    dots.push_back(d41);
//
//    // top left
//    Dot d1;
//    d1.setup(vec2(304,704), vec2(208,238), ci::Color(0, 1, 1));
//    dots.push_back(d1);
//
//    // middle
//    Dot d3;
//    d3.setup(vec2(420,704), vec2(446, 478), ci::Color(0.5, 1, 0));
//    dots.push_back(d3);
//    
//    //top right
//	Dot d2;
//	d2.setup(vec2(637, 704), vec2(686, 240), ci::Color(0, 0, 1));
//	dots.push_back(d2);
//
//    // bottom right
//	Dot d5;
//	d5.setup(vec2(690, 704), vec2(691, 872), ci::Color(1.0, 0.5, 0.3));
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
    
    for(auto&e : emmitters){
        e.reset();
    }
         
         

}



void TestSet::stop(){
    isRunning = false;
    calcuclateFitnessScore();
}




void TestSet::update(vec2& gravity){
    
    
    if(!isRunning) return;
    
    
    
    isHitTarget = true;
	float combinedDistance = 0;

    
    for(int i = 0; i < emmitters.size();++i){
        
        Emitter* e = &emmitters[i];
        e->mTargetForce = emmitterData.data[lifeTime][i];
        e->update();
    }
    

    for(auto& d : dots){
        checkBounderies(d);
        applyForces(d,0);


		float distance = glm::distance(d.mPosition, d.mTargetPosition);
		combinedDistance += distance;


        isHitTarget *= distance < 10;
        limitSpeed(d);
        
        d.mVelocity = vLerp(d.mVelocity, vec2(0,0), GS()->lerpBallVelocity);
        d.mDirection = vLerp(d.mDirection, gravity, 0.1);

        d.update();
    }
    
	if (combinedDistance < recordDistance) recordDistance = combinedDistance;

    if(isHitTarget) stop();
    
    
    ++lifeTime;

    
}



void TestSet::limitSpeed(Dot& dot){
    if(length(dot.mVelocity) > GS()->maxSpeed){
        vec2 n = normalize(dot.mVelocity);
        dot.mVelocity = n *  GS()->maxSpeed;
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
        
        forceFactor = pow(forceFactor, 1.4);
        //  forceFactor *= 1.6;
        // cout << forceFactor << endl;
        
        dot.mDirection -= normalize(distance) * forceFactor;
    }

}


void TestSet::drawEmitters(std::shared_ptr<ci::nvg::Context> nvgContext){
    
    auto& vg = *nvgContext;

    //draw emmitters
    for(auto& e : emmitters){
        
        
        for(float f=0; f < e.mForce; f+= 26.0){
            vg.beginPath();

            vg.strokeColor(ColorAf{.0f, .8f, .9f});

            float s = lmap<float>(f, 0, e.mForce, 40, 1);
            if(s < 0) s =1;
            vg.strokeWidth(s);

            vg.arc(e.mPosition , f,0, (M_PI * 2), NVG_CW);
         //   vg.closePath();
            vg.stroke();


        }
    }
}



void TestSet::drawConnections(std::shared_ptr<ci::nvg::Context> nvgContext,float width){
    
    auto& vg = *nvgContext;
    vg.strokeWidth(width);

    vg.beginPath();
    
    for(int i = 1; i < dots.size(); ++i){
        vec2 norm = glm::normalize(dots[i].mPosition - dots[i-1].mPosition);
        vg.moveTo(dots[i-1].mPosition + (norm * 20.0f));
        vg.lineTo(dots[i].mPosition - (norm * 20.0f));
    }
    
    vg.stroke();

    
}





void TestSet::drawDots(std::shared_ptr<ci::nvg::Context> nvgContext,float radius){
    auto& vg = *nvgContext;

        //draw dots
        for(int i = 0; i < dots.size(); ++i){
            
            vg.beginPath();
            vg.arc(dots[i].mPosition , 4, -M_PI * 0.5f,  M_PI * 2.0f, NVG_CW);
            vg.closePath();
            vg.stroke();
        }
}




void TestSet::randomize(int frames){
    
    
    ci::Perlin perlin = Perlin( 4,  clock() & 65535 );
;
    int const eSize = emmitters.size();// emmitters.size();
    
    for (float i=0; i< frames; ++i) {
        
        vector<float> emitterForces;
        emitterForces.reserve(eSize);
        
        for(float j=0; j< eSize;++j){
            float n = fabs(perlin.noise(i * 0.01f, j ));
            
            emitterForces.push_back(fabs(n) * 260.0f);
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


