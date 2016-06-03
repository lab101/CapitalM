//
//  SettingManager.h
//
//  Created by Kris Meeusen on 17/09/15.
//
//
//  Usage
//
//
//      Setting<float>  globalVolume = Setting<float>("global audio volume", 0.1);
//      addSetting(&globalVolume);
//
//      std::cout << globalVolume.mValue;


#ifndef __VideoPlayer__SettingManager__
#define __VideoPlayer__SettingManager__

#include <stdio.h>
#include "cinder/Utilities.h"
#include "cinder/Filesystem.h"
#include "cinder/log.h"
#include "cinder/app/App.h"
#include "cinder/Signals.h"
#include "cinder/Json.h"




class SettingBase{
	std::string mKey;
	bool isTweakable;

protected:
public:


	std::string getKey(){
		return mKey;
	}

	void setKey(std::string key){
		mKey = key;
	}

    
    virtual ci::JsonTree toJson(){ return ci::JsonTree("data",""); };
    virtual void setFromJson(ci::JsonTree json){  };
	
    
    
    
    // needed for tweakable settings
    // should find a better way to remove them here.
    virtual std::string getTweakableSettings(){ return ""; };
    virtual int  getSortOrder(){ return 0; };
    virtual std::string getCategory(){ return "";};


};




//template <typename T>
//T toNumber(std::string const & text)
//{
//	T value;
//	std::stringstream ss(text);
//	ss >> value;
//	return value;
//}
//

// SETTING TEMPLATED
template <typename T>
class Setting : public SettingBase{

public:

    T mValue;

	Setting<T>(){
	}

    Setting<T>(std::string key, T defaultValue){
       mValue = defaultValue;
       SettingBase::setKey(key);
    }
    

	T& value(){
		return mValue;
	}

    void setFromJson(ci::JsonTree value) override{
        mValue = value.getValue<T>();
	}

   
    ci::JsonTree toJson() override{
        ci::JsonTree json = ci::JsonTree(getKey(), mValue);
        return json;
    }


};



// SETTING tweakable
template <typename T>
class TweakableSetting : public Setting<T>{
    
    T mTweakableMin;
    T mTweakableMax;
    T mTweakableSteps;

    int mSortOrder = 0;
    std::string mCategory = "default";

    
public:
    
    TweakableSetting<T>(){}

    TweakableSetting<T>(std::string key, T defaultValue){
        Setting<T>::mValue = defaultValue;
        mTweakableMin = 0;
        mTweakableMax = defaultValue * 2;
        mTweakableSteps = 1;
        
        SettingBase::setKey(key);
    }
    
    TweakableSetting<T> setMin(T value){
        mTweakableMin = value;
        return *this;
    }
    
    TweakableSetting<T> setMax(T value){
        mTweakableMax = value;
        return *this;
    }
    
    TweakableSetting<T> setSteps(T value){
        mTweakableSteps = value;
        return *this;
    }
    
    TweakableSetting<T> setCategorie(std::string value){
        mCategory = value;
        return *this;
    }
    
    TweakableSetting<T> setSortOrder(int value){
        mSortOrder = value;
        return *this;
    }
    
    int  getSortOrder(){
        return mSortOrder;
    }
    
    std::string getCategory(){
        return mCategory;
    }
    

    virtual std::string getTweakableSettings() override{
        
        
        return "{\"min\" : " + ci::toString(mTweakableMin) + ",\"max\" : " + ci::toString(mTweakableMax) + ",\"steps\" : " + ci::toString(mTweakableSteps) + ",\"category\" : \"" + mCategory + "\"}";
    }

};









// VEC3

class SettingVec3 : public SettingBase{
    
public:
    ci::vec3 mValue;
    
    SettingVec3(){}
    
    SettingVec3(std::string key, ci::vec3 defaultValue){
        mValue = defaultValue;
        SettingBase::setKey(key);
    }
    
    

    
    void setFromJson(ci::JsonTree value) override{
        
        if(value.getNumChildren() == 3){
            mValue.x = value.getChild("x").getValue<float>();
            mValue.y = value.getChild("y").getValue<float>();
            mValue.z = value.getChild("z").getValue<float>();
        }
        
    }

    
 
    ci::JsonTree toJson() override{
        ci::JsonTree data = ci::JsonTree::makeArray(getKey());
        data.addChild(ci::JsonTree("x",mValue.x));
        data.addChild(ci::JsonTree("y",mValue.y));
        data.addChild(ci::JsonTree("z",mValue.z));
        return data;
    }
    
};








class SettingManager{
    std::vector<SettingBase*> settings;
	//map with setting from the settings file form disk.
    ci::JsonTree storedSettingsJson;

	std::string const getSettingPath(){
		return cinder::app::getAssetPath("//").string() + "settings.json";
	}
	static SettingManager* instance;




public:

	static SettingManager* Instance();

//	ci::signals::Signal<void(std::pair<std::string, std::string>)> onSettingsUpdate;


	std::vector<SettingBase*> getSettings(){
		return settings;
	}

	SettingManager(){
		readSettings();
	}

  
//	void setSettingByString(std::string key, std::string value){
//
//		for (SettingBase* s : settings){
//			if (s->getKey() == key){
//				//FIX//s->setValueFromString(value);
//				onSettingsUpdate.emit(std::pair<std::string, std::string>(key, value));
//
//				break;
//			}
//		}
//	}

    void setSettingByJSON(ci::JsonTree data);
        



    void addSetting(SettingBase* setting);
    
	std::string getSettingStringDataByKey(std::string key)
	{

		for (SettingBase* s : settings){
			if (s->getKey() == key){
				 //FIX//return s->valueToString();
				break;
			}
		}

		CI_LOG_E("SETTING NOT FOUND " + key);
		return "";
	}


    void readSettings();
    void writeSettings();


};




inline void addSetting(SettingBase* setting){
	SettingManager::Instance()->addSetting(setting);
}






#endif /* defined(__VideoPlayer__SettingManager__) */
