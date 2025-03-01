
#include<stdio.h>
#include<SDL.h>
#include<SDL_Image.h>
#include<string>
//#include<SDL_ttf.h>

const int SCREEN_HEIGHT=480;
const int SCREEN_WIDTH=640;

const int BUTTON_WIDTH=300;
const int BUTTON_HEIGHT=200;
const  int TOTAL_BUTTONS=4;

enum LButtonSprite {
BUTTON_SPRITE_MOUSE_OUT,
BUTTON_SPRITE_MOUSE_OVER_MOTION,
BUTTON_SPRITE_MOUSE_DOWN,
BUTTON_SPRITE_MOUSE_UP,
BUTTON_SPRITE_TOTAL,
};

class LTexture{
private:
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
public:
    LTexture();
    ~LTexture();
    void free();
    bool loadFromFile(std:: string path);
    #if defined(SDL_TTF_MAJOR_VERSION)
    bool loadFromRenderedText(std::string textureText,SDL_Color textColor);
    #endif;
    void setColor(Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode(SDL_BlendMode blending);
    void setAlpha(Uint8 alpha);
    void render (int x,int y,SDL_Rect* clip=NULL,double angle=0,SDL_Point* center=NULL,SDL_RendererFlip flip=SDL_FLIP_NONE);
    int getWidth();
    int getHeight();

};

class LButton{
private:
    LButtonSprite mCurrentSprite;
    SDL_Point mPosition;
public:
    LButton();
    void setPosition(int x,int y);
    void handleEvent(SDL_Event *e);
    void render();

};

bool init();
bool loadMedia();
void close();

SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
LTexture gButtonSpriteSheetTexture;
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
LButton gButtons[TOTAL_BUTTONS];

LTexture::LTexture(){
mTexture=NULL;
mWidth=0;
mHeight=0;
}

LTexture::~LTexture(){
free();
}

bool LTexture::loadFromFile(std::string path){
free();
SDL_Texture* newTexture=NULL;
SDL_Surface* loadedSurface=IMG_Load(path.c_str());
if(loadedSurface==NULL)printf("Unable to load image %s! SDL_Image Error:%s\n",path.c_str(),IMG_GetError());
else{
    SDL_SetColorKey(loadedSurface,SDL_TRUE,SDL_MapRGB(loadedSurface->format,0,255,255));
    newTexture=SDL_CreateTextureFromSurface(gRenderer,loadedSurface);
    if(newTexture==NULL)printf("Unable to creat texture from surface%s \n",SDL_GetError());
    else{
        mWidth=loadedSurface->w;
        mHeight=loadedSurface->h;

    }
    SDL_FreeSurface(loadedSurface);
}
mTexture=newTexture;
return mTexture!=NULL;

}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color colorText){
free();
SDL_Surface* textSurface=TTF_RenderText_Solid(gFront,texttureText.c_str(),colorText);
if(textSurface==NULL)printf("Unable to render text surface! %s \n",TTF_GetError());
else{
    mTexture=SDL_CreateTextureFromSurface(gRenderer,textSurface);
    if(mTexture==NULL)printf("Unable to create texture from text %s \n",SDL_GetError());
    else{
        mWidth=textSurface->w;
        mHeight=textSurface->h;
    }
    SDL_FreeSurface(textSurface);
}
return mTexture!=NULL;
}
#endif
void LTexture::free(){
if(mTexture!=NULL){
    SDL_DestroyTexture(mTexture);
    mTexture=NULL;
    mWidth=0;
    mHeight=0;
}

}

void LTexture::setColor(Uint8 red,Uint8 green,Uint8 blue){
SDL_SetTextureColorMod(mTexture,red,green,blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending){
SDL_SetTextureBlendMode(mTexture,blending);
}

void LTexture::setAlpha(Uint8 alpha){
SDL_SetTextureAlphaMod(mTexture,alpha);
}

void LTexture::render(int x,int y,SDL_Rect* clip,double angle,SDL_Point* center,SDL_RendererFlip flip){
SDL_Rect renderQuad={x,y,mWidth,mHeight};
if(clip!=NULL){
    renderQuad.w=clip->w;
    renderQuad.h=clip->h;
}
SDL_RenderCopyEx(gRenderer,mTexture,clip,&renderQuad,angle,center,flip);
}

int LTexture::getWidth(){return mWidth;}
int LTexture::getHeight(){return mHeight;}

LButton::LButton(){
    mCurrentSprite=BUTTON_SPRITE_MOUSE_OUT;
    mPosition.x=0;
    mPosition.y=0;

}

void LButton::setPosition(int x,int y){
mPosition.x=x;
mPosition.y=y;
}

void LButton::handleEvent(SDL_Event* e){
    if(e->type==SDL_MOUSEMOTION || e->type==SDL_MOUSEBUTTONDOWN || e->type==SDL_MOUSEBUTTONUP){
        int x;int y;
        SDL_GetMouseState(&x,&y);
        bool inside=true;
        if(x<mPosition.x)inside=false;
        else if(x>mPosition.x+BUTTON_WIDTH)inside=false;
        else if(y<mPosition.y)inside=false;
        else if(y>mPosition.y+BUTTON_HEIGHT)inside=false;

        if(inside==false)mCurrentSprite=BUTTON_SPRITE_MOUSE_OUT;
        else{
            switch(e->type){
            case SDL_MOUSEBUTTONDOWN :mCurrentSprite=BUTTON_SPRITE_MOUSE_DOWN;break;
            case SDL_MOUSEBUTTONUP: mCurrentSprite=BUTTON_SPRITE_MOUSE_UP;break;
            case SDL_MOUSEMOTION: mCurrentSprite=BUTTON_SPRITE_MOUSE_OVER_MOTION;break;
            }

        }
    }
}

void LButton::render(){
gButtonSpriteSheetTexture.render(mPosition.x,mPosition.y,&gSpriteClips[mCurrentSprite]);
}

bool init(){
   bool success=true;
if(SDL_Init(SDL_INIT_VIDEO)!=0){printf("cant create SDL %s \n",SDL_GetError());success=false;}
else{
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1")){printf("Linear textue filtering not enabled!\n");success=false;}

    gWindow=SDL_CreateWindow("SDL_Tutorial",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if(gWindow==NULL){printf("cant create window %s \n",SDL_GetError());success=false;}
    else{
        gRenderer=SDL_CreateRenderer(gWindow,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
        if(gRenderer==NULL){printf("cant create renderer %s \n",SDL_GetError());success=false;}
        else{
                SDL_SetRenderDrawColor(gRenderer,255,255,255,255);
            if(!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)){printf("cant init IMG %s \n",IMG_GetError());success=false;}
        }
    }

}
return success;

}

bool loadMedia(){
bool success=true;
if(!gButtonSpriteSheetTexture.loadFromFile("image/button.png")){
    printf("Failed to load button sprite texture \n");
    success=false;
}
else{
    for( int i=0;i<BUTTON_SPRITE_TOTAL;i++){
        gSpriteClips[ i ].x = 0;
			gSpriteClips[ i ].y = i * 200;
			gSpriteClips[ i ].w = BUTTON_WIDTH;
			gSpriteClips[ i ].h = BUTTON_HEIGHT;
    }
        gButtons[ 0 ].setPosition( 0, 0 );
		gButtons[ 1 ].setPosition( SCREEN_WIDTH - BUTTON_WIDTH, 0 );
		gButtons[ 2 ].setPosition( 0, SCREEN_HEIGHT - BUTTON_HEIGHT );
		gButtons[ 3 ].setPosition( SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT );
}
return success;
}

void close(){
gButtonSpriteSheetTexture.free();

SDL_DestroyRenderer(gRenderer);gRenderer=NULL;
SDL_DestroyWindow(gWindow);gWindow=NULL;


SDL_Quit();
IMG_Quit();
//TTF_Quit();
}

int main(int argc,char* argv[]){
if(!init())printf("Failed to initialize\n");
else{
    if(!loadMedia())printf("failed to load media\n");
    else{
        bool quit=false;
        SDL_Event e;
        while(!quit){
            while(SDL_PollEvent(&e)!=0){
                if(e.type==SDL_QUIT)quit=true;
                for( int i=0;i<TOTAL_BUTTONS;i++){
                    gButtons[i].handleEvent(&e);
                }
            }
            SDL_SetRenderDrawColor(gRenderer,255,255,255,255);
            SDL_RenderClear(gRenderer);

            for( int i=0;i<TOTAL_BUTTONS;i++){
                gButtons[i].render();
            }
            SDL_RenderPresent(gRenderer);
        }
    }

}
close();
return 0;

}



