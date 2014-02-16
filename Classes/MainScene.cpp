//
//  MainScene.cpp
//  CrystalCraze
//
//  Created by 信徒 on 14-2-12.
//
//

#include "MainScene.h"
#include "AboutScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "Gem.h"

USING_NS_CC;
using namespace cocosbuilder;
using namespace CocosDenshion;

MainScene::MainScene()
: mMenuType(MENU_NONE)
, mSpriteStarNode(NULL)
, mMenu(NULL)
, mLastScore(NULL)
, mFallingGems(NULL)
, mNodeFallingLayer(NULL)
, mAnimationManager(NULL)
{
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(mSpriteStarNode);
    CC_SAFE_RELEASE_NULL(mMenu);
    CC_SAFE_RELEASE_NULL(mLastScore);
    CC_SAFE_RELEASE_NULL(mFallingGems);
    CC_SAFE_RELEASE_NULL(mNodeFallingLayer);
    CC_SAFE_RELEASE_NULL(mAnimationManager);
}

void MainScene::update(float delta)
{
    int percent = rand() % 100;
	if (percent < 2)
	{
		int type = rand() % 5;
		Sprite *sprt = Sprite::createWithSpriteFrameName(
            String::createWithFormat("crystals/%d.png",type)->getCString());
        int width = 100 * mNodeFallingLayer->getContentSize().width;
		float x = (float)(rand() % width) / 100;
		float y = mNodeFallingLayer->getContentSize().height + kGemSize/2;
		float scale = 0.2 + 0.8 * (rand() % 100) / 100;
        
		float speed = 2*scale*kGemSize/40;
        
		sprt->setPosition(x,y);
		sprt->setScale(scale);
        
        Gem *gem = new Gem();
        gem->speed = speed;
        gem->sprt = sprt;
        mFallingGems->addObject(gem);
		mNodeFallingLayer->addChild(sprt);
	}
    
    Object *it = NULL;
    CCARRAY_FOREACH(mFallingGems, it)
    {
        auto gem = dynamic_cast<Gem *>(it);
        auto pos = gem->sprt->getPosition();
		pos.y -= gem->speed;
		gem->sprt->setPosition(pos);
        
		if (pos.y < -kGemSize/2)
		{
            mNodeFallingLayer->removeChild(gem->sprt, true);
			mFallingGems->removeObject(gem);
		}
    }
}

void MainScene::onNodeLoaded(cocos2d::Node *pNode, cocosbuilder::NodeLoader *pNodeLoader)
{
    if (mSpriteStarNode)
    {
        Node *node = cocos2d::ParticleSystemQuad::create("particles/bg-stars.plist");
        mSpriteStarNode->addChild(node);
    }
    
    if (mLastScore)
    {
        mLastScore->setString(String::createWithFormat("%d", gLastScore)->getCString());
    }
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile ("crystals.plist");
    
    mFallingGems = new Array();
    mFallingGems->init();

    scheduleUpdate();
    
    
    SimpleAudioEngine::getInstance()->playBackgroundMusic("sounds/loop.mp3");
    
    mMenuType = MENU_NONE;
}

bool MainScene::onAssignCCBMemberVariable(cocos2d::Object* pTarget, const char* pMemberVariableName, cocos2d::Node* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "starNode", cocos2d::Sprite *, mSpriteStarNode);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "menu", cocos2d::Menu *, mMenu);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "lblLastScore", cocos2d::LabelBMFont *, mLastScore);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "fallingGemsLayer", cocos2d::Node *, mNodeFallingLayer);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "mAnimationManager", cocosbuilder::CCBAnimationManager *, mAnimationManager);
    return false;
}

cocos2d::SEL_MenuHandler MainScene::onResolveCCBCCMenuItemSelector(cocos2d::Object * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressPlay", MainScene::onPressPlay);
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPressAbout", MainScene::onPressAbout);
    return  NULL;
}

cocos2d::extension::Control::Handler MainScene::onResolveCCBCCControlSelector(cocos2d::Object * pTarget, const char* pSelectorName)
{
    return NULL;
}

void MainScene::onAnimationComplete()
{
    if (mMenuType == MENU_PLAY)
    {
        Scene *scene = GameScene::createScene();
        Director::getInstance()->replaceScene(scene);
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    }
}

void MainScene::onPressPlay(cocos2d::Object *sender)
{
    mMenuType = MENU_PLAY;
    if (mAnimationManager)
    {
        mAnimationManager->runAnimationsForSequenceNamed("Outro");
    }
    
    SimpleAudioEngine::getInstance()->playEffect("sounds/click.wav");
    
    Object *it = NULL;
    CCARRAY_FOREACH(mFallingGems, it)
    {
        auto gem = dynamic_cast<Gem *>(it);
		gem->sprt->runAction(FadeOut::create(0.5));
    }
}

void MainScene::onPressAbout(cocos2d::Object *sender)
{
    mMenuType = MENU_ABOUT;
    
    SimpleAudioEngine::getInstance()->playEffect("sounds/click.wav");
    mMenu->setEnabled(false);

    Node *node = AboutScene::createLayer();
    
	addChild(node, 10);
}

void MainScene::onRemoveMenuLayer(cocos2d::Node *layer)
{
    mMenu->setEnabled(true);
    removeChild(layer);
}

void MainScene::setAnimationManager(cocosbuilder::CCBAnimationManager *animationManager)
{
    CC_SAFE_RELEASE_NULL(mAnimationManager);
    mAnimationManager = animationManager;
    CC_SAFE_RETAIN(mAnimationManager);
    mAnimationManager->setAnimationCompletedCallback(this,
         callfunc_selector(MainScene::onAnimationComplete));
}

Scene *MainScene::createScene()
{
    NodeLoaderLibrary *library = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();
    library->registerNodeLoader("MainScene", MainSceneLoader::loader());
    
    CCBReader *reader = new CCBReader(library);
    reader->autorelease();
    Node *node = reader->readNodeGraphFromFile("MainScene.ccbi");
    ((MainScene *)node)->setAnimationManager(reader->getAnimationManager());
    Scene *scene = Scene::create();
    scene->addChild(node);
    return scene;
}
