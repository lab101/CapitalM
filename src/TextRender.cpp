//
//  TextRender.cpp
//  ClientUI
//
//  Created by lab101 on 04/05/16.
//
//

#include "TextRender.hpp"
#include "cinder/app/app.h"

using namespace ci;
using namespace ci::app;


TextRender::TextRender(){
    
	mFontBig = Font("Arial", 28);
//	mFontSmall = Font("Pixel Operator 8", 14);
	//mFontSmall = Font("Pixel Operator 8", 14);
	mFontSmall = Font("Gotham Book", 14);
	//mFontBig = Font("Arial", 18);
//	mFontSmall = Font("Hello Type", 16);

    mTextureFont = gl::TextureFont::create( mFontBig );

}


void TextRender::setup(){
}


ci::gl::TextureRef TextRender::renderTextToTextureRef(std::string txt,float maxWidht){
    
	vec2 size = TextBox().alignment(TextBox::LEFT).font(mFontSmall).text(txt).measure();
	
    TextBox tbox = TextBox().alignment( TextBox::LEFT ).font( mFontSmall ).size( ivec2(std::min(size.x +2, maxWidht), TextBox::GROW ) ).text( txt );
    tbox.setColor( Color( 1.0f, 1.0f, 1.0f ) );
    tbox.setBackgroundColor( ColorA( 0, 0, 0, 1 ) );
    tbox.setPremultiplied(true);
    return gl::Texture2d::create( tbox.render() );

}



void TextRender::renderText(std::string txt, ci::ivec2 position,float maxWidth){
  //  ci::gl::enableAlphaBlending();
    
  //  gl::color( ColorA( 1, 0.5f, 0.25f, 1.0f ) );

    ci::gl::color(1, 1, 1);
    mTextureFont->drawString(txt, position);

}




