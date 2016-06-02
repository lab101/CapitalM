#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"

#include "EmmiterData.hpp"

#include "TextRender.hpp"
#include "TestSet.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class BabyMApp : public App {
  
public:
    
	void setup() override;
    void mouseDown( MouseEvent event ) override;
    void keyDown( KeyEvent event ) override;
    
	void update() override;
	void draw() override;
    
    void start();
    
    int getTotalHits();
    float getMaxFitness();
	long double getTotalFitness();
	int getRecordDistance();
	void  newSelection();
	bool lock;
    vec2 arrivalPoint;
    vec2 gravity;
    long double totalFitness;
	int recordDistance = 10000;
    float mutation = 0.04;
    
    int frames;
    int maxFrames = 300;
    int generations=0;
    int testSetsAmount=50;
    int totalHits=0;
    
    bool isRunning = false;
    vector<TestSet> testSets;
	TestSet* bestSet;
    
    
    vector<EmitterData> matingPool;
};

void BabyMApp::setup()
{
    
    
    arrivalPoint = vec2(240,40);
    gravity = vec2(0,0.1);
	lock = true;
    
    testSets.reserve(testSetsAmount);
    
    
    for(int i=0; i < testSetsAmount;++i){
        testSets.push_back(TestSet());
        testSets[i].setup();
        testSets[i].randomize(3, maxFrames);
    }
    

	start();

}


void BabyMApp::newSelection(){
    float maxFitness = getMaxFitness();
    
    totalFitness = getTotalFitness();
    totalHits = getTotalHits();
	recordDistance = getRecordDistance();

    matingPool.clear();
    
    //for (int i = 0; i < testSets.length; i++) {
    for(auto& t : testSets){
        float fitnessNormal = lmap<float>(t.fitness,0,maxFitness,0,1);
        int n = (int) (fitnessNormal * 100);  // Arbitrary multiplier
        for (int j = 0; j < n; j++) {
//            EmitterData emData;
//            emData.data = t.mData;
            matingPool.push_back(t.emmitterData);
        }
    }
    
    
    //cout << matingPool.size() << endl;
    
    
    
    
    // Refill the population with children from the mating pool
    for (int i = 0; i < testSets.size(); i++) {
        // Sping the wheel of fortune to pick two parents
        int m = int(randInt(matingPool.size()));
        int d = int(randInt(matingPool.size()));
        // Pick two parents
        EmitterData mom = matingPool.at(m);
        EmitterData dad = matingPool.at(d);

        // Mate their genes
        EmitterData child = mom.crossover(dad);
        child.mutate(mutation);
        
        
        testSets[i].setNewData(child);
        // Mutate their genes
       // child.mutate(mutationRate);
        // Fill the new population with the new child
        //PVector location = new PVector(width/2,height+20);
        //population[i] = new Rocket(location, child,population.length);
    }
    generations++;


}

void BabyMApp::start(){
    frames = 0;
    isRunning =true;

    for(auto& t : testSets){
        t.start();
    }

	bestSet = &testSets[0];

  
}


void BabyMApp::mouseDown( MouseEvent event )
{
//    dot.mPosition = event.getPos();
//    dot.mVelocity = ci::vec2(0,0);
//    dot.mDirection = ci::vec2(0,0);
}


void BabyMApp::keyDown( KeyEvent event ){
    if(event.getCode() == event.KEY_UP){
        mutation += 0.005;
    }
	if (event.getCode() == event.KEY_DOWN){
		mutation -= 0.005;
	}
	if (event.getCode() == event.KEY_l){
		lock = !lock;

	}

    if(event.getCode() == event.KEY_SPACE){
        newSelection();
        start();

    }
}




void BabyMApp::update()
{

    if(isRunning){


		int lowestDistance = 10000;
		for (auto& t : testSets){
			t.update(gravity, arrivalPoint);
			if (t.recordDistance < lowestDistance){
				lowestDistance = t.recordDistance;
				bestSet = &t;
			}
		}

        if(++frames == maxFrames){
            for(auto& t : testSets){
                t.stop();
            }
            


            if(getTotalHits() > 0 && lock){
				isRunning = false;
            }else{
				newSelection();
				start();
               
            }
 
        }
        
    }
}


float BabyMApp::getMaxFitness(){
    float max = 0;
    for(auto& t : testSets){
        if(max < t.fitness){
            max = t.fitness;
        }
    }
    
    return max;
}

long double BabyMApp::getTotalFitness(){
    long double fit = 0;
    for(auto& t : testSets){
        fit += t.fitness;
    }
    
    return fit;
}

int BabyMApp::getTotalHits(){
    int fit = 0;
    for(auto& t : testSets){
        fit += t.isHitTarget;
    }
    
    return fit;
}

int BabyMApp::getRecordDistance(){
	int fit = 100000;
	for (auto& t : testSets){
		if (t.recordDistance < fit) fit = t.recordDistance;
	}

	return fit;
}




void BabyMApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );

    
    int textOffset=0;
    for(auto& t : testSets){
      //  t.draw(textOffset);
       // textOffset+=10;
    }

	if (bestSet != nullptr){
		bestSet->draw(0);
	}
    
    
    int f = totalFitness * 10000000000000;
    gl::drawString("total fitness " + toString(f), vec2(500,20));
	gl::drawString("pool size " + toString(matingPool.size()), vec2(500, 30));
    gl::drawString("mutation " + toString(mutation), vec2(500,40));
  //  gl::drawString("pool size " + toString(matingPool.size()), vec2(500,50));

	TextRenderSingleton::Instance()->renderText("HITS #" + toString(totalHits), vec2(500, 80));
	TextRenderSingleton::Instance()->renderText("GEN #" + toString(generations), vec2(500, 100));
	TextRenderSingleton::Instance()->renderText("DIST #" + toString(recordDistance), vec2(500, 120));
}

CINDER_APP( BabyMApp, RendererGl )
