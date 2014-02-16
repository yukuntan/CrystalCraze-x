//
//  AboutScene.cpp
//  CrystalCraze
//
//  Created by 信徒 on 14-2-12.
//
//

#include "AboutScene.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace cocosbuilder;
using namespace CocosDenshion;

AboutScene::AboutScene()
: mAnimationManager(NULL)
, mPressDown(false)
{
    
}

AboutScene::~AboutScene()
{
    CC_SAFE_RELEASE_NULL(mAnimationManager);
}

void AboutScene::onNodeLoaded(cocos2d::Node *pNode, cocosbuilder::NodeLoader *pNodeLoader)
{
}

bool AboutScene::onAssignCCBMemberVariable(cocos2d::Object* pTarget, const char* pMemberVariableName, cocos2d::Node* pNode)
{
    return false;
}

cocos2d::SEL_MenuHandler AboutScene::onResolveCCBCCMenuItemSelector(cocos2d::Object * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressDone", AboutScene::onPressDone);
    return  NULL;
}

cocos2d::extension::Control::Handler AboutScene::onResolveCCBCCControlSelector(cocos2d::Object * pTarget, const char* pSelectorName)
{
    return NULL;
}

void AboutScene::onPressDone(cocos2d::Object *sender)
{
    SimpleAudioEngine::getInstance()->playEffect("sounds/click.wav");
    mAnimationManager->runAnimationsForSequenceNamed("Outro");
    mPressDown = true;
}

void AboutScene::onAnimationComplete()
{
    if (mPressDown)
    {
        Node *node = getParent();
        ((MainScene *)node)->onRemoveMenuLayer(this);
    }
}

void AboutScene::setAnimationManager(cocosbuilder::CCBAnimationManager *animationManager)
{
    CC_SAFE_RELEASE_NULL(mAnimationManager);
    mAnimationManager = animationManager;
    CC_SAFE_RETAIN(mAnimationManager);
    mAnimationManager->setAnimationCompletedCallback(this,
        callfunc_selector(AboutScene::onAnimationComplete));
}

Node *AboutScene::createLayer()
{
    NodeLoaderLibrary *library = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();
    library->registerNodeLoader("AboutScene", AboutSceneLoader::loader());
    
    CCBReader *reader = new CCBReader(library);
    reader->autorelease();
    Node *node = reader->readNodeGraphFromFile("AboutScene.ccbi");
    ((AboutScene *)node)->setAnimationManager(reader->getAnimationManager());
    return node;
}