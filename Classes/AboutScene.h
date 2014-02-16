//
//  AboutScene.h
//  CrystalCraze
//
//  Created by 信徒 on 14-2-12.
//
//

#ifndef __CrystalCraze__AboutScene__
#define __CrystalCraze__AboutScene__

#include "cocos2d.h"
#include "cocosbuilder/cocosbuilder.h"

class AboutScene
: public cocos2d::Layer
, public cocosbuilder::NodeLoaderListener
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::CCBSelectorResolver
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(AboutScene, create);
    
    AboutScene();
    virtual ~AboutScene();
    
    virtual void onNodeLoaded(cocos2d::Node * pNode, cocosbuilder::NodeLoader * pNodeLoader);
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Object* target, const char* memberVariableName, cocos2d::Node* node);
    
    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::Object * pTarget, const char* pSelectorName);
    
    virtual cocos2d::extension::Control::Handler onResolveCCBCCControlSelector(cocos2d::Object * pTarget, const char* pSelectorName);
    
    void onPressDone(cocos2d::Object *sender);
    
    void onAnimationComplete();
    
    void setAnimationManager(cocosbuilder::CCBAnimationManager *animationManager);
    
    static Node *createLayer();
    
protected:
    cocosbuilder::CCBAnimationManager *mAnimationManager;
    bool mPressDown;
};

class AboutSceneLoader
: public cocosbuilder::LayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(AboutSceneLoader, loader);
    
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(AboutScene);
};


#endif /* defined(__CrystalCraze__AboutScene__) */
