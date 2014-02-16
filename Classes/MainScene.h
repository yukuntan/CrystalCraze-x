//
//  MainScene.h
//  CrystalCraze
//
//  Created by 信徒 on 14-2-12.
//
//

#ifndef __CrystalCraze__MainScene__
#define __CrystalCraze__MainScene__

#include "cocos2d.h"
#include "cocosbuilder/cocosbuilder.h"

class MainScene
: public cocos2d::Layer
, public cocosbuilder::NodeLoaderListener
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::CCBSelectorResolver
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(MainScene, create);
    
    MainScene();
    virtual ~MainScene();
    
    virtual void update(float delta);
    
    virtual void onNodeLoaded(cocos2d::Node * pNode, cocosbuilder::NodeLoader * pNodeLoader);
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Object* target, const char* memberVariableName, cocos2d::Node* node);
    
    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::Object * pTarget, const char* pSelectorName);
    
    virtual cocos2d::extension::Control::Handler onResolveCCBCCControlSelector(cocos2d::Object * pTarget, const char* pSelectorName);
    
    void onAnimationComplete();
    
    void onPressPlay(cocos2d::Object *sender);
    
    void onPressAbout(cocos2d::Object *sender);
    
    void onRemoveMenuLayer(cocos2d::Node *layer);
    
    void setAnimationManager(cocosbuilder::CCBAnimationManager *animationManager);
    
    static cocos2d::Scene *createScene();
    
protected:
    enum MENU_TYPE
    {
        MENU_NONE,
        MENU_PLAY,
        MENU_ABOUT
    };
    
    class Gem : public cocos2d::Object
    {
    public:
        Gem()
        : sprt(NULL)
        , speed(0.0)
        {}
        
        virtual ~Gem()
        {
            CC_SAFE_RELEASE_NULL(sprt);
        }
        
        cocos2d::Sprite *sprt;
        float speed;
    };
    
protected:
    MENU_TYPE mMenuType;
    cocos2d::Array *mFallingGems;
    
    cocos2d::Sprite *mSpriteStarNode;
    cocos2d::Menu *mMenu;
    cocos2d::LabelBMFont *mLastScore;
    cocos2d::Node *mNodeFallingLayer;
    cocosbuilder::CCBAnimationManager *mAnimationManager;

};

class MainSceneLoader
: public cocosbuilder::LayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(MainSceneLoader, loader);
    
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(MainScene);
};


#endif /* defined(__CrystalCraze__MainScene__) */
