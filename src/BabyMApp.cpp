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
	void draw() override;
    
    void start();
    
    int getTotalHits();
    float getMaxFitness();
	long double getTotalFitness();
	int getRecordDistance();
	void  newSelection();
	bool lock;

    long double totalFitness;
	int recordDistance = 10000;
    
    int frames;
    int maxFrames       = 1050;
    int generations     = 0;
    int testSetsAmount;
    int totalHits       = 0;
    
    bool isRunning = false;
    vector<TestSet> testSets;
	TestSet* bestSet;
    params::InterfaceGlRef	mParams;

    vector<int> shadows;
    
    gl::TextureRef  building;
    
    vector<EmitterData> matingPool;
    
    ci::vec2 mousePos;
    

};

void BabyMApp::setup()
{
    
    //string replayFile = "emmitterData8066.csv";
    string replayFile = "run1/emmitterData34096.csv";
    
    
    mNanoVG = std::make_shared<nvg::Context>(nvg::createContextGL());

    if(GS()->isReplay){
        testSetsAmount = 10;
        lock = true;
     
    }else{
        testSetsAmount = 1050;
        lock = true;
        
        int offset = testSetsAmount /10;
        for (int i =0; i < testSetsAmount; i+=offset) {
            shadows.push_back(i);
        }
        

    }
    
    
    
    setWindowPos(10, 10);
    
    building = gl::Texture::create(loadImage(loadAsset("building.png")));
    
    
    testSets.reserve(testSetsAmount);
    
    GS()->mScreen.set(0, 0, building->getWidth(), building->getHeight());
    // GS()->mScreen.set(0, 0, 800, 600);
    setWindowSize(GS()->mScreen.getWidth() + 300, GS()->mScreen.getHeight());

    for(int i=0; i < testSetsAmount;++i){
        testSets.push_back(TestSet());
        testSets[i].setup();
        
        if(GS()->isReplay){
            testSets.back().readData("run1/_s"+ toString(i)+ "_emmitterData34096.csv");
            shadows.push_back(i);
        }else{
            testSets[i].randomize(maxFrames);
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
    
    while(isRunning){
        update();
    }

    
}


void BabyMApp::newSelection(){
    float maxFitness = getMaxFitness();
    
    totalFitness = getTotalFitness();
    totalHits = getTotalHits();
	recordDistance = getRecordDistance();

    matingPool.clear();
    

    for(auto& t : testSets){
        float fitnessNormal = lmap<float>(t.fitness,0,maxFitness,0,1);
        int n = (int) (fitnessNormal * 100);  // Arbitrary multiplier

        for (int j = 0; j < n; j++) {
            matingPool.push_back(t.emmitterData);
        }
    }
    
    
    
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
       // newSelection();
        start();

    }
}




void BabyMApp::update()
{

    if(isRunning){

        
        if(++frames == maxFrames){
            std::cout << "GEN:" << generations << "\tDIST: " << recordDistance << "\tSELECTION pool size:" << matingPool.size() << endl;

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


		int lowestDistance = 10000;
		for (auto& t : testSets){
			t.update(GS()->gravity);
            
			if (t.recordDistance < lowestDistance){
				lowestDistance = t.recordDistance;
				bestSet = &t;
                
			}
            
            
            if(t.isHitTarget && lock && !GS()->isReplay){

                if(!GS()->isReplay){
                    
                    std::cout << "FOUND ONE AT gen " << generations << " : distance" << recordDistance << endl;

                    int i=0;
                    
                    string fileName = "_s" + toString(i) + "_emmitterData" + toString(getElapsedFrames()) + ".csv";
                    t.dumpData(fileName );
                    
                    for(int s : shadows){
                        string fileName = "_s" + toString(++i) + "_emmitterData" + toString(getElapsedFrames()) + ".csv";
                        testSets[s].dumpData( fileName + ".csv");
                    }
                    
                    isRunning = false;
                    return;

                }else{
                   // isRunning = false;
                    return;
                }
                
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

    
    gl::color(0.5, 0.5, 0.5);
    gl::draw(building);
    gl::color(1, 1, 1);
    
    if(!GS()->isBackgroundDrawingOff){


    // Store a reference so we can use dot-notation.
    auto& vg = *mNanoVG;

    for(int s : shadows){
            
        vg.strokeColor(Color{.8f, .8f, .8f});
        testSets[s].drawConnections(mNanoVG,0.8);

        vg.strokeWidth(2);
        vg.strokeColor(Color(1,1,1));

        testSets[s].drawDots(mNanoVG, 4);
    }
    
    
    

	if (bestSet != nullptr){
        

        vg.strokeColor(ColorAf{244  / 255.0f, 1.f, .0f});

        bestSet->drawConnections(mNanoVG,4);
        bestSet->drawEmitters(mNanoVG);

        for(auto& d : bestSet->dots){
            
           // float s = lmap<float>(d.getSpeed(), 0, 6, 0, 20);

            vg.beginPath();
            vg.strokeWidth(8);

            vg.fillColor(Color(1,0,0));
            vg.strokeColor(ColorAf{204  / 255.0f, 1.f, .0f});

            vg.arc(d.mPosition , 10, -M_PI * 0.5f,  M_PI * 2.0f, NVG_CW);
            vg.closePath();
            vg.stroke();
            


        }
	}
    
    
    gl::color(1, 1, 1);

    vg.beginFrame(getWindowSize(), getWindowContentScale());
    vg.endFrame();
    
    
    gl::color(0, 0, 0);
    gl::drawSolidRect(Rectf(GS()->mScreen.x2,0,getWindowWidth(),getWindowHeight()));
    
    }
    
    int offs = GS()->mScreen.x2 + 60;
    //int f = totalFitness * 10000000000;
    //gl::drawString("total fitness \t" + toString(f), vec2(900,20));
	gl::drawString("pool size \t" + toString(matingPool.size()), vec2(offs, 35));
    gl::drawString("mutation \t" + toString(GS()->mutation), vec2(offs,50));
    gl::drawString("population \t" + toString(testSetsAmount), vec2(offs,65));
  //  gl::drawString("pool size " + toString(matingPool.size()), vec2(500,50));
    gl::drawString("mouse\t" + toString(mousePos), vec2(offs, 80));
 
	TextRenderSingleton::Instance()->renderText("HITS #" + toString(totalHits), vec2(offs , 110));
	TextRenderSingleton::Instance()->renderText("GEN #" + toString(generations), vec2(offs , 140));
    TextRenderSingleton::Instance()->renderText("DIST #" + toString(recordDistance), vec2(offs , 170));
    TextRenderSingleton::Instance()->renderText("FRAME #" + toString(bestSet->lifeTime), vec2(offs , 200));
    
    
    // Draw the interface
    mParams->draw();


}

CINDER_APP(BabyMApp, RendererGl(RendererGl::Options().stencil().msaa(0)),
           [](App::Settings *settings) { settings->setHighDensityDisplayEnabled(); })


//CINDER_APP( BabyMApp, RendererGl )
