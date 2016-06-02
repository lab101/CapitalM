//
//  TextRender.hpp
//  ClientUI
//
//  Created by lab101 on 04/05/16.
//
//

#ifndef TextRender_hpp
#define TextRender_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "Singleton.hpp"

class TextRender{
    
    ci::gl::TextureFontRef      mTextureFont;
    ci::Font                    mFontSmall;
    ci::Font                    mFontBig;


public:
    TextRender();
    void renderText(std::string txt,ci::ivec2 position,float maxWidht = 2000);
    void setup();
    
    ci::gl::TextureRef renderTextToTextureRef(std::string txt,float maxWidht = 2000);
    
};

typedef Singleton<TextRender> TextRenderSingleton;


#endif /* TextRender_hpp */
