//
//  SettingManager.cpp
//  InovationWall
//

#include "SettingManager.h"

using namespace std;
using namespace ci;


SettingManager*  SettingManager::instance = NULL;


SettingManager* SettingManager::Instance(){
	if (instance == NULL)
	{
		instance = new SettingManager();

	}

	return instance;
}




void SettingManager::setSettingByJSON(ci::JsonTree data){
    for (SettingBase* s : settings){
        
        string key = data.getKey();
        
        if (s->getKey() == key){
            s->setFromJson(data);
  //          onSettingsUpdate.emit(std::pair<std::string, std::string>(key, value));
            
            break;
        }
    }
    
}


void SettingManager::readSettings(){
    
    if (!fs::exists(getSettingPath())){
        CI_LOG_E("CAN'T FIND SETTING FILE " + getSettingPath());
        return;
    }
    
    std::ifstream input(getSettingPath().c_str());
    if (!input.is_open())
    {
        CI_LOG_E("CAN'T OPEN SETTING FILE " + getSettingPath());
        return;
    }
    else{
        
        
        string file_contents { istreambuf_iterator<char>(input), istreambuf_iterator<char>() };

        storedSettingsJson = JsonTree(file_contents);
        
        CI_LOG_I("found " << storedSettingsJson.getNumChildren() << " settings");
        
        input.close();
    }
    
    
}


void SettingManager::writeSettings(){
    
    JsonTree settingsJson = JsonTree::makeArray();
    
    for (SettingBase* s : settings){
        
       // JsonTree setting = JsonTree::makeObject();
       // setting.addChild(s->toJson());
        settingsJson.pushBack(s->toJson());
    }
    
    
    ofstream settingFile(getSettingPath(), std::ios::out);
    settingFile << settingsJson.serialize();
    settingFile.close();
    
    CI_LOG_I("settings saved to " + getSettingPath());
}



void SettingManager::addSetting(SettingBase* setting){
    
    std::string key = setting->getKey();
    
    if(storedSettingsJson.hasChild(key)){
        string value = storedSettingsJson.getChild(key).getValue<string>();
        CI_LOG_D("FOUND SETTING : " + key );
        // FIX setting->setValueFromString(value);
        setting->setFromJson(storedSettingsJson.getChild(key));
        //onSettingsUpdate.emit(std::pair<std::string, std::string>(key, value));
    }

    settings.push_back(setting);
}

