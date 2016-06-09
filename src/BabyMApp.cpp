#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"

#include "EmmiterData.hpp"

#include "TextRender.hpp"
#include "TestSet.hpp"

#include "cinder/params/Params.h"
#include "SettingManager.h"


#include "ci_nanovg_gl.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class BabyMApp : public App {
  
    
    std::shared_ptr<nvg::Context> mNanoVG;

public:
    
	void setup() override;
    void mouseDown( MouseEvent event ) override;
    void keyDown( KeyEvent event ) override;
    
	void update() override;
    
    void updateForGeneration();
    void updateForReplay();
    
	void draw() override;
    
    void start();
    void stop();
    
    int getTotalHits();
    float getMaxFitness();
	int getRecordDistance();
	void  newSelection();
	bool lock;

	int recordDistance = 10000;
    
    int frames          = 0;
    int generations     = 0;
    int testSetsAmount;
    int totalHits       = 0;
    
    bool isRunning = false;
    vector<TestSet> testSets;
	TestSet* bestSet;
    params::InterfaceGlRef	mParams;

    vector<int>             shadows;
    
    gl::TextureRef          building;
    
    vector<EmitterData>     emmiterDataCopy;
    vector<EmitterData*>    matingPool;
    
    ci::vec2 mousePos;
    

};

void BabyMApp::setup()
{
    
    //string replayFile = "emmitterData8066.csv";
    
    
    mNanoVG = std::make_shared<nvg::Context>(nvg::createContextGL());
    testSetsAmount = GS()->testSetMax;

   lock = true;
        
//        int offset = testSetsAmount /10;
//        for (int i =0; i < testSetsAmount; i+=offset) {
//            shadows.push_back(i);
//        }
    
    
    setWindowPos(10, 10);
    
    building = gl::Texture::create(loadImage(loadAsset("building2.png")));
    
    
    testSets.reserve(testSetsAmount);
    emmiterDataCopy.resize(testSetsAmount);
    emmiterDataCopy.assign(testSetsAmount, EmitterData());
    
    GS()->mScreen.set(0, 0, building->getWidth(), building->getHeight());
    setWindowSize(GS()->mScreen.getWidth() + 300, GS()->mScreen.getHeight());

    for(int i=0; i < testSetsAmount;++i){
        testSets.push_back(TestSet());
        testSets[i].setup();
        
        if(GS()->isReplay){
            testSets.back().readData(GS()->replayFile);
          //  shadows.push_back(i);
        }else{
            testSets[i].randomize(GS()->maxFrames,i);
        }
    }

    

    
    
    // Create the interface and give it a name.
    mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 300 ) ) );
    mParams->setPosition(vec2(GS()->mScreen.x2 +250,300));
    
    
    // Setup some basic parameters.
    

    mParams->addParam( "lerpTargetForce", &(GS()->lerpTargetForce)).min( 0.001f ).max( 0.9f ).precision( 10 ).step( 0.0001f );
    mParams->addParam( "lerpFalloffForce", &(GS()->lerpFalloffForce)).min( 0.000001f ).max( 0.9f ).precision( 10 ).step( 0.00001f );
    mParams->addParam( "maxSpeed", &(GS()->maxSpeed)).min( 0.1f ).max( 20.0f ).precision( 4 ).step( 0.1f );
    
    mParams->addParam( "lerpBallVelocity", &(GS()->lerpBallVelocity)).min( 0.0001f ).max( 10.0f ).precision( 6 ).step( 0.001f );
    mParams->addParam( "isBackgroundDrawingOff", &(GS()->isBackgroundDrawingOff));
    
    
    start();
    
    while(isRunning && GS()->noDraw && recordDistance > 40){
        update();
    }

    
}


void BabyMApp::newSelection(){
    float maxFitness = getMaxFitness();
    
    matingPool.clear();
    
    for(int i =0; i < testSetsAmount; ++i)
    {
        
        emmiterDataCopy[i] = (testSets[i].emmitterData);

        float fitnessNormal = lmap<float>(testSets[i].fitness,0,maxFitness,0,1);
        int n = (int) (fitnessNormal * 100);  // Arbitrary multiplier

        for (int j = 0; j < n; j++) {
            matingPool.push_back(&(emmiterDataCopy[i]));
        }
    }
    
    
    
    for (int i = 0; i < testSets.size(); i++) {
        int m = int(randInt(matingPool.size()));
        int d = int(randInt(matingPool.size()));

        EmitterData mom = *matingPool.at(m);
        EmitterData dad = *matingPool.at(d);

        // Mate their genes
        EmitterData child = mom.crossover(dad);
        child.mutate(GS()->mutation);
        
        testSets[i].setNewData(child);
    }
    
    generations++;
}



void BabyMApp::start(){
    frames = 0;
    isRunning = true;

    for(auto& t : testSets){
        t.start();
    }

	bestSet = &testSets[0];
}

void BabyMApp::stop(){
    
    for(auto& t : testSets){
        t.stop();
    }

    totalHits = getTotalHits();
    recordDistance = getRecordDistance();
    isRunning = false;
}


void BabyMApp::mouseDown( MouseEvent event )
{
    mousePos = event.getPos();
}




void BabyMApp::keyDown( KeyEvent event ){
    if(event.getCode() == event.KEY_UP){
        GS()->mutation += 0.005;
    }
	if (event.getCode() == event.KEY_DOWN){
		GS()->mutation -= 0.005;
	}
	if (event.getCode() == event.KEY_l){
		lock = !lock;

	}

    if(event.getCode() == event.KEY_SPACE){
        newSelection();
        start();

    }
}

void BabyMApp::updateForGeneration(){
    
    if(isRunning){
        
        ++frames;
        
        // LAST FRAME STOP THE TEST.
        if(frames == GS()->maxFrames){
            
            stop();
            
            cout << "\nGEN:" << generations;
            cout << "\t\tDIST: " << recordDistance;
            cout << "\t\tSELECTION pool size:" << matingPool.size();
            cout << "\t\tframes: " << GS()->maxFrames;
            cout << "\t\tpopulation \t" << testSetsAmount <<  endl;
            
            newSelection();
            start();
        }
        
        // UPDATE AND SELECT FITTEST
        int lowestDistance = 10000;
        for (auto& t : testSets){
            
            // update test sets
            t.updateEmitters(false);
            t.update(GS()->gravity);
            
            // check for the set with the smallest distance.
            if (t.recordDistance < lowestDistance){
                lowestDistance = t.recordDistance;
                bestSet = &t;
            }
            
            
            if(t.isHitAllTargets && lock ){
                
                std::cout << "\nFOUND ONE AT gen " << generations << " : distance" << recordDistance << endl;
                
                
                string fileName = "_s" + toString(generations) + "_emmitterData" + toString(getElapsedFrames()) + ".csv";
                t.dumpData(fileName );
                
                
                stop();
                return;
                
            }
        }
    }
}


void BabyMApp::updateForReplay(){
    if(isRunning){
        
        ++frames;
        
        // LAST FRAME STOP THE TEST.
        if(frames == GS()->maxFrames){
            stop();
        }
        
        
        for (auto& t : testSets){
            t.updateEmitters(false);
            t.update(GS()->gravity);
            
            bestSet = &t;
        }

    }else{
        GS()->lerpTargetForce=0.004;
        GS()->lerpFalloffForce=0.005;
        bestSet->updateEmitters(true);
    }

}



void BabyMApp::update()
{
    if(GS()->isReplay) updateForReplay();
    else updateForGeneration();
  
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




int BabyMApp::getTotalHits(){
    int fit = 0;
    for(auto& t : testSets){
        fit += t.isHitAllTargets;
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
    
    gl::color(0.1, 0.1, 0.1);
    gl::draw(building);
    gl::color(1, 1, 1);
    
    if(!GS()->isBackgroundDrawingOff){


    // Store a reference so we can use dot-notation.
    auto& vg = *mNanoVG;

	if (bestSet != nullptr){
        
        bestSet->drawConnections(mNanoVG,bestSet->dots,4);
        bestSet->drawConnections(mNanoVG,bestSet->dots2,4);
       
        bestSet->drawDots(mNanoVG,bestSet->dots,4);
        bestSet->drawDots(mNanoVG,bestSet->dots2,4);
        
        bestSet->drawEmitters(mNanoVG);
   	}
    
    gl::color(1, 1, 1);

    vg.beginFrame(getWindowSize(), getWindowContentScale());
    vg.endFrame();
    
    
    gl::color(0, 0, 0);
    gl::drawSolidRect(Rectf(GS()->mScreen.x2,0,getWindowWidth(),getWindowHeight()));
    
    }
    
    int offs = GS()->mScreen.x2 + 60;
    
//    gl::disableAlphaBlending();
//    gl::disableBlending();
   // gl::enableAlphaBlendingPremult();
    gl::enableAlphaBlending();
    gl::enableAlphaBlendingPremult();
    
    gl::drawString("pool size \t" + toString(matingPool.size()), vec2(offs, 35));
    gl::drawString("mutation \t" + toString(GS()->mutation), vec2(offs,50));
    gl::drawString("population \t" + toString(testSetsAmount), vec2(offs,65));
  //  gl::drawString("pool size " + toString(matingPool.size()), vec2(500,50));
    gl::drawString("mouse\t" + toString(mousePos), vec2(offs, 80));
 
	TextRenderSingleton::Instance()->renderText("HITS #" + toString(totalHits), vec2(offs , 110));
	TextRenderSingleton::Instance()->renderText("GEN #" + toString(generations), vec2(offs , 140));
    TextRenderSingleton::Instance()->renderText("DIST #" + toString(recordDistance), vec2(offs , 170));
    TextRenderSingleton::Instance()->renderText("FRAME #" + toString(bestSet->lifeTime), vec2(offs , 200));
    TextRenderSingleton::Instance()->renderText("TIME #" + toString(ci::app::getElapsedSeconds()), vec2(offs , 230));
    
    
    // Draw the interface
   // mParams->draw();


}

CINDER_APP(BabyMApp, RendererGl(RendererGl::Options().stencil().msaa(0)),
           [](App::Settings *settings) { settings->setHighDensityDisplayEnabled(); })


//CINDER_APP( BabyMApp, RendererGl )
