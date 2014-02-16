//
//  GameScene.cpp
//  CrystalCraze
//
//  Created by 信徒 on 14-2-12.
//
//

#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "MainScene.h"
#include "Gem.h"

USING_NS_CC;
using namespace cocosbuilder;
using namespace CocosDenshion;

int kBoardWidth = 8;
int kBoardHeight = 10;
int kNumTotalGems = kBoardWidth * kBoardHeight;
int kTimeBetweenGemAdds = 8;
int kTotalGameTime = 1000*60;
int kIntroTime = 1800;
int kNumRemovalFrames = 8;
int kDelayBeforeHint = 3000;
int kMaxTimeBetweenConsecutiveMoves = 1000;

float kGameOverGemSpeed = 0.1;
float kGameOverGemAcceleration = 0.005;

int kBoardTypeGem0 = 0;
int kBoardTypeGem1 = 1;
int kBoardTypeGem2 = 2;
int kBoardTypeGem3 = 3;
int kBoardTypeGem4 = 4;
int kBoardTypePup0 = 5;


GameScene::GameScene()
: mAnimationManager(NULL)
, mIsOver(false)
, mTimer(NULL)
, mLabelScore(NULL)
, mGameLayer(NULL)
, mSprtHeader(NULL)

, mFallingGems(NULL)
, mBoard(NULL)
, mBoardSprites(NULL)
, mNumGemsInColumn(NULL)
, mTimeSinceAddInColumn(NULL)

, mScore(0)
, mLastTakenGemTime(0)
, mNumConsecutiveGems(0)
, mIsPowerPlay(false)
, mPowerPlayParticles(NULL)
, mPowerPlayLayer(NULL)

, mParticleLayer(NULL)
, mHintLayer(NULL)
, mShimmerLayer(NULL)
, mEffectsLayer(NULL)

, mProgressTimer(NULL)

, mTimeCurrent(0)
, mLastMoveTime(0)
, mIsDisplayingHint(false)

, mBoardChangedSinceEvaluation(false)
, mPossibleMove(0)

, mGameOverGems(NULL)
, mEndTimerStarted(false)
{
    auto listenerTouch = EventListenerTouchAllAtOnce::create();
    listenerTouch->onTouchesBegan = CC_CALLBACK_2(GameScene::onTouchesBegan, this);
    auto listenerMouse = EventListenerMouse::create();
    listenerMouse->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenerMouse, this);
}

GameScene::~GameScene()
{
    CC_SAFE_RELEASE_NULL(mAnimationManager);
    CC_SAFE_RELEASE_NULL(mTimer);
    CC_SAFE_RELEASE_NULL(mLabelScore);
    CC_SAFE_RELEASE_NULL(mGameLayer);
    CC_SAFE_RELEASE_NULL(mSprtHeader);
    
    CC_SAFE_DELETE_ARRAY(mFallingGems);
    CC_SAFE_DELETE_ARRAY(mBoard);
    CC_SAFE_DELETE_ARRAY(mNumGemsInColumn);
    CC_SAFE_DELETE_ARRAY(mTimeSinceAddInColumn);
    
    CC_SAFE_RELEASE_NULL(mPowerPlayParticles);
    CC_SAFE_RELEASE_NULL(mPowerPlayLayer);
    
    CC_SAFE_RELEASE_NULL(mParticleLayer);
    CC_SAFE_RELEASE_NULL(mHintLayer);
    
    CC_SAFE_RELEASE_NULL(mShimmerLayer);
    CC_SAFE_RELEASE_NULL(mEffectsLayer);
    
    CC_SAFE_RELEASE_NULL(mProgressTimer);
    
    mBoardSprites.clear();
    _eventDispatcher->removeAllEventListeners();
}

void GameScene::onEnter()
{
    Layer::onEnter();
}

void GameScene::update(float delta)
{
    if (!mIsOver)
	{
		removeMarkedGems();
        
		int x = 0;
        auto point = Point(0,0);
		
		// Add falling gems
		for (x = 0; x < kBoardWidth; x++)
		{
			if (mNumGemsInColumn[x] + mFallingGems[x].size() < kBoardHeight &&
				mTimeSinceAddInColumn[x] >= kTimeBetweenGemAdds)
			{
				// A gem should be added to this column!
				int gemType = rand()%5;
				auto gemSprite = Sprite::createWithSpriteFrameName(String::createWithFormat("crystals/%d.png",gemType)->getCString());
				gemSprite->setPosition(x * kGemSize, kBoardHeight * kGemSize);
				gemSprite->setAnchorPoint(point);
                Gem gem;
                gem.gemType = gemType;
                gem.gemSprite = gemSprite;
                gem.yPos = kBoardHeight;
                gem.ySpeed = 0;
				mFallingGems[x].push_back(gem);
                
				mGameLayer->addChild(gemSprite);
                
				mTimeSinceAddInColumn[x] = 0;
			}
            
			mTimeSinceAddInColumn[x]++;
		}
        
		// Move falling gems
		bool gemLanded = false;
		for (x = 0; x < kBoardWidth; x++)
		{
			Gems &column = mFallingGems[x];
			int numFallingGems = mFallingGems[x].size();
			for (int i = numFallingGems-1; i >= 0; i--)
			{
				Gem &gem = column[i];
                
				gem.ySpeed += 0.06;
				gem.ySpeed *= 0.99;
				gem.yPos -= gem.ySpeed;
                
				if (gem.yPos <= mNumGemsInColumn[x])
				{
					// The gem hit the ground or a fixed gem
					if (!gemLanded)
					{
                        SimpleAudioEngine::getInstance()->playEffect(String::createWithFormat("sounds/tap-%d.wav",
                            (rand()%4))->getCString());
						gemLanded = true;
					}
                    
					// Insert into board
					int y = mNumGemsInColumn[x];
                    
					if (mBoard[x + y*kBoardWidth] != -1)
					{
						CCLOG(String::createWithFormat("Warning! Overwriting board idx: %d type: %d", x + y*kBoardWidth,
                           mBoard[x + y*kBoardWidth])->getCString());
					}
                    
					mBoard[x + y*kBoardWidth] = gem.gemType;
					mBoardSprites[x + y*kBoardWidth] = gem.gemSprite;
                    
					// Update fixed position
					gem.gemSprite->setPosition(x*kGemSize, y*kGemSize);
					mNumGemsInColumn[x] ++;
                    
					mBoardChangedSinceEvaluation = true;
                    column.erase(column.begin() + i);
				}
				else
				{
					// Update the falling gems position
					gem.gemSprite->setPosition(x*kGemSize, gem.yPos*kGemSize);
				}
			}
		}
        
		// Check if there are possible moves and no gems falling
		bool isFallingGems = false;
		for (x = 0; x < kBoardWidth; x++)
		{
			if (mNumGemsInColumn[x] != kBoardHeight)
			{
				isFallingGems = true;
				break;
			}
		}
        
		if (!isFallingGems)
		{
			int possibleMove = findMove();
			if (possibleMove == -1)
			{
				// Create a possible move
				createRandomMove();
			}
		}
        mTimeCurrent += delta * 1000;
		// Update timer
		float currentTime = mTimeCurrent - kIntroTime;
        if (currentTime < 0) currentTime = 0;
		float timeLeft = (kTotalGameTime - currentTime) * 100.0/kTotalGameTime;
		if (timeLeft < 0) timeLeft = 0;
		if (timeLeft > 99.9) timeLeft = 99.9;
        
		mProgressTimer->setPercentage(timeLeft);
        
		// Update consecutive moves / powerplay
		if (currentTime - mLastMoveTime > kMaxTimeBetweenConsecutiveMoves)
		{
			mNumConsecutiveGems = 0;
		}
		updatePowerPlay();
        
		// Update sparkles
		updateSparkle();
        
		// Check if timer sound should be played
		if (timeLeft < 6.6 && !mEndTimerStarted)
		{
            SimpleAudioEngine::getInstance()->playEffect("sounds/timer.wav");
			mEndTimerStarted = true;
		}
        
		// Check for game over
		if (timeLeft == 0)
		{
			createGameOver();
			mAnimationManager->runAnimationsForSequenceNamed("Outro");
            mIsOver = true;
			//gAudioEngine.stopAllEffects();
			CCLOG("stopAllEffects not working!");
            SimpleAudioEngine::getInstance()->playEffect("sounds/endgame.wav");
			gLastScore = mScore;
		}
		else if (currentTime - mLastMoveTime > kDelayBeforeHint && !mIsDisplayingHint)
		{
			displayHint();
		}
	}
	else
	{
		// It's game over
		updateGameOver();
	}

}

void GameScene::onNodeLoaded(cocos2d::Node *pNode, cocosbuilder::NodeLoader *pNodeLoader)
{
    mTimeCurrent = 0;
   
	// Setup board
	setBoard();
    
	mIsOver = false;
	mIsDisplayingHint = false;
    
    
    // Setup timer
    mTimer->setVisible(false);
    mProgressTimer = ProgressTimer::create(Sprite::create("gamescene/timer.png"));
    mProgressTimer->setPosition(mTimer->getPosition());
    mProgressTimer->setPercentage(100);
    mProgressTimer->setType(ProgressTimerType::BAR);
    mProgressTimer->setMidpoint(Point(0, 0.5));
    mProgressTimer->setBarChangeRate(Point(1, 0));
    mSprtHeader->addChild(mProgressTimer);
    
    mLastMoveTime = mTimeCurrent;
    mTimeCurrent += kIntroTime;
    
    mNumConsecutiveGems = 0;
    mIsPowerPlay = false;
    mEndTimerStarted = false;
    
    mScore = 0;
    
    // Schedule callback
     scheduleUpdate();
    
    // TODO: Make into batch node
    
    mParticleLayer = ParticleBatchNode::create("particles/taken-gem.png", 250);
    Node *gameLayer = SpriteBatchNode::create("crystals.pvr.ccz");
    gameLayer->setContentSize(mGameLayer->getContentSize());
    
    mHintLayer = Node::create();
    mShimmerLayer = Node::create();
    mEffectsLayer = Node::create();
    
    mGameLayer->addChild(mShimmerLayer, -1);
    mGameLayer->addChild(mParticleLayer, 1);
    mGameLayer->addChild(gameLayer, 0);
    mGameLayer->addChild(mHintLayer, 3);
    mGameLayer->addChild(mEffectsLayer, 2);
    
	setupShimmer();
}

bool GameScene::onAssignCCBMemberVariable(cocos2d::Object* pTarget, const char* pMemberVariableName, cocos2d::Node* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "sprtTimer", Sprite *, mTimer);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "lblScore", LabelBMFont *, mLabelScore);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "gameLayer", Layer *, mGameLayer);
    CCB_MEMBERVARIABLEASSIGNER_GLUE(this, "sprtHeader", Sprite *, mSprtHeader);
    return false;
}

cocos2d::SEL_MenuHandler GameScene::onResolveCCBCCMenuItemSelector(cocos2d::Object * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCMENUITEM_GLUE(this, "onPauseClicked", GameScene::onPressPause);
    return  NULL;
}

cocos2d::extension::Control::Handler GameScene::onResolveCCBCCControlSelector(cocos2d::Object * pTarget, const char* pSelectorName)
{
    return NULL;
}

void GameScene::onTouchesBegan(const std::vector<cocos2d::Touch *> &touches,
                               cocos2d::Event *unused_event)
{
    processClick(touches[0]->getLocation());
}

void GameScene::onMouseDown(cocos2d::Event *event)
{
    EventMouse *eventMouse = (EventMouse*)event;
    Point position(eventMouse->getCursorX(), eventMouse->getCursorY());
    processClick(position);
}

void GameScene::onRemoveFromParent(cocos2d::Node *node)
{
    node->getParent()->removeChild(node);
}

void GameScene::processClick(const cocos2d::Point &location)
{
    Point loc = location-mGameLayer->getPosition();
    
	int x = floor((float)loc.x/kGemSize);
	int y = floor((float)loc.y/kGemSize);
    
	if (!mIsOver)
	{
		mHintLayer->removeAllChildren();
		mIsDisplayingHint = false;
        
		if (activatePowerUp(x,y) ||
			removeConnectedGems(x,y))
		{
			// Player did a valid move
			int sound = mNumConsecutiveGems;
			if (sound > 4) sound = 4;
            SimpleAudioEngine::getInstance()->playEffect(
              String::createWithFormat("sounds/gem-%d.wav",sound)->getCString());
            
			mNumConsecutiveGems++;
		}
		else
		{
			mNumConsecutiveGems = 0;
		}
        
		mLastMoveTime = mTimeCurrent;
	}

}

void GameScene::onPressPause(cocos2d::Object *sender)
{
    createGameOver();
    SimpleAudioEngine::getInstance()->playEffect("sounds/endgame.wav");
    mAnimationManager->runAnimationsForSequenceNamed("Outro");
    mIsOver = true;
}

void GameScene::onAnimationComplete()
{
    if (mIsOver)
    {
        Scene *scene = MainScene::createScene();
        Director::getInstance()->replaceScene(scene);
    }
}

void GameScene::setAnimationManager(cocosbuilder::CCBAnimationManager *animationManager)
{
    CC_SAFE_RELEASE_NULL(mAnimationManager);
    mAnimationManager = animationManager;
    CC_SAFE_RETAIN(mAnimationManager);
    mAnimationManager->setAnimationCompletedCallback(this,
       callfunc_selector(GameScene::onAnimationComplete));
}

Scene *GameScene::createScene()
{
    NodeLoaderLibrary *library = NodeLoaderLibrary::newDefaultNodeLoaderLibrary();
    library->registerNodeLoader("GameScene", GameSceneLoader::loader());
    
    CCBReader *reader = new CCBReader(library);
    reader->autorelease();
    Node *node = reader->readNodeGraphFromFile("GameScene.ccbi");
    ((GameScene *)node)->setAnimationManager(reader->getAnimationManager());
    Scene *scene = Scene::create();
    scene->addChild(node);
    return scene;
}

void GameScene::setBoard()
{
    mBoard = new int[kNumTotalGems];
	for (int i = 0; i < kNumTotalGems; i++)
	{
		mBoard[i] = -1;
        mBoardSprites.push_back(nullptr);
	}
    
	mNumGemsInColumn = new int[kBoardWidth];
	mTimeSinceAddInColumn = new float[kBoardWidth];
	
	int x = 0;
	
	for (x = 0; x < kBoardWidth; x++)
	{
		mNumGemsInColumn[x] = 0;
		mTimeSinceAddInColumn[x] = 0;
	}
    
	// Setup falling pieces
	mFallingGems = new Gems[kBoardWidth];
    
	mBoardChangedSinceEvaluation = true;
	mPossibleMove = -1;
}

void GameScene::findConnectedGems(int x, int y, std::vector<int> &arr)
{
	if (mBoard[x + y*kBoardWidth] <= -1) return;
    
	findConnectedGems_(x, y, arr, mBoard[x + y*kBoardWidth]);
}

void GameScene::findConnectedGems_(int x, int y, std::vector<int> &arr, int gemType)
{
    // Check for bounds
	if (x < 0 || x >= kBoardWidth) return;
	if (y < 0 || y >= kBoardHeight) return;
    
	int idx = x + y*kBoardWidth;
    
	// Make sure that the gems type match
	if (mBoard[idx] != gemType) return;
    
    
	// Check if idx is already visited
	for (int i = 0; i < arr.size(); i++)
	{
		if (arr[i] == idx) return;
	}
    
	// Add idx to array
	arr.push_back(idx);
    
	// Visit neighbours
	findConnectedGems_(x+1, y, arr, gemType);
	findConnectedGems_(x-1, y, arr, gemType);
	findConnectedGems_(x, y+1, arr, gemType);
	findConnectedGems_(x, y-1, arr, gemType);
}

bool GameScene::removeConnectedGems(int x, int y)
{
    bool removedGems = false;
	// Check for bounds
	if (x < 0 || x >= kBoardWidth) return removedGems;
	if (y < 0 || y >= kBoardHeight) return removedGems;
    
    std::vector<int> connected;
	findConnectedGems(x,y, connected);
	
    
	if (connected.size() >= 3)
	{
		mBoardChangedSinceEvaluation = true;
		removedGems = true;
        
		addScore(100 * connected.size());
        
		int idxPup = -1;
		int pupX = 0;
		int pupY = 0;
		if (connected.size() >= 6)
		{
			// Add power-up
			idxPup = connected[rand() % connected.size()];
			pupX = idxPup % kBoardWidth;
			pupY = floor((float)idxPup/kBoardWidth);
		}
        
		for (int i = 0; i < connected.size(); i++)
		{
			int idx = connected[i];
			int gemX = idx % kBoardWidth;
			int gemY = floor((float)idx/kBoardWidth);
            
			mBoard[idx] = -kNumRemovalFrames;
			mGameLayer->removeChild(mBoardSprites[idx]);
			mBoardSprites[idx] = NULL;
            
			// Add particle effect
			auto particle = ParticleSystemQuad::create("particles/taken-gem.plist");
			particle->setPosition(gemX * kGemSize+kGemSize/2, gemY*kGemSize+kGemSize/2);
			particle->setAutoRemoveOnFinish(true);
			mParticleLayer->addChild(particle);
            
			// Add power-up
			if (idx == idxPup)
			{
				mBoard[idx] = kBoardTypePup0;
                auto zeroPoint = Point(0, 0);
				auto sprt = Sprite::createWithSpriteFrameName("crystals/bomb.png");
				sprt->setPosition(gemX*kGemSize, gemY*kGemSize);
				sprt->setAnchorPoint(zeroPoint);
				sprt->setOpacity(0);
				sprt->runAction(FadeIn::create(0.4));
                
				auto sprtGlow = Sprite::createWithSpriteFrameName("crystals/bomb-hi.png");
				sprtGlow->setAnchorPoint(zeroPoint);
				sprtGlow->setOpacity(0);
				sprtGlow->runAction(RepeatForever::create(
                    Sequence::createWithTwoActions(FadeIn::create(0.4),FadeOut::create(0.4))));
				sprt->addChild(sprtGlow);
                
				mBoardSprites[idx] = sprt;
				mGameLayer->addChild(sprt);
			}
			else if (idxPup != -1)
			{
				// Animate effect for power-up
				auto sprtLight = Sprite::createWithSpriteFrameName("crystals/bomb-light.png");
				sprtLight->setPosition(gemX*kGemSize+kGemSize/2, gemY*kGemSize+kGemSize/2);
				BlendFunc bf = {GL_SRC_ALPHA, GL_ONE};
                sprtLight->setBlendFunc(bf);
				mEffectsLayer->addChild(sprtLight);
                
				auto movAction = MoveTo::create(0.2, Point(pupX*kGemSize+kGemSize/2, pupY*kGemSize+kGemSize/2));
				auto seqAction = Sequence::createWithTwoActions(movAction, CallFunc::create(
                    CC_CALLBACK_0(GameScene::onRemoveFromParent, this, sprtLight)));
                
				sprtLight->runAction(seqAction);
			}
		}
	}
	else
	{
        SimpleAudioEngine::getInstance()->playEffect("sounds/miss.wav");
	}
    
	mLastMoveTime = mTimeCurrent;
    
	return removedGems;
}

bool GameScene::activatePowerUp(int x, int y)
{
    bool removedGems = false;
    // Check for bounds
	if (x < 0 || x >= kBoardWidth) return removedGems;
	if (y < 0 || y >= kBoardHeight) return removedGems;
    
	
    
	int idx = x + y * kBoardWidth;
	if (mBoard[idx] == kBoardTypePup0)
	{
		// Activate bomb
        SimpleAudioEngine::getInstance()->playEffect("sounds/powerup.wav");
        
		removedGems = true;
        
		addScore(2000);
        
		mBoard[idx] = -kNumRemovalFrames;
		mGameLayer->removeChild(mBoardSprites[idx]);
		mBoardSprites[idx] = NULL;
		
		// Remove a horizontal line
		int idxRemove;
		for (int xRemove = 0; xRemove < kBoardWidth; xRemove++)
		{
			idxRemove = xRemove + y * kBoardWidth;
			if (mBoard[idxRemove] >= 0 && mBoard[idxRemove] < 5)
			{
				mBoard[idxRemove] = -kNumRemovalFrames;
				mGameLayer->removeChild(mBoardSprites[idxRemove]);
				mBoardSprites[idxRemove] = NULL;
			}
		}
        
		// Remove a vertical line
		for (int yRemove = 0; yRemove < kBoardHeight; yRemove++)
		{
			idxRemove = x + yRemove * kBoardWidth;
			if (mBoard[idxRemove] >= 0 && mBoard[idxRemove] < 5)
			{
				mBoard[idxRemove] = -kNumRemovalFrames;
				mGameLayer->removeChild(mBoardSprites[idxRemove]);
				mBoardSprites[idxRemove] = NULL;
			}
		}
        
		// Add particle effects
		auto hp = ParticleSystemQuad::create("particles/taken-hrow.plist");
		hp->setPosition(kBoardWidth/2*kGemSize+kGemSize/2, y*kGemSize+kGemSize/2);
		hp->setAutoRemoveOnFinish(true);
		mParticleLayer->addChild(hp);
        
		auto vp = ParticleSystemQuad::create("particles/taken-vrow.plist");
		vp->setPosition(x*kGemSize+kGemSize/2, kBoardHeight/2*kGemSize+kGemSize/2);
		vp->setAutoRemoveOnFinish(true);
		mParticleLayer->addChild(vp);
        
		// Add explo anim
		auto center = Point(x*kGemSize+kGemSize/2, y*kGemSize+kGemSize/2);
        
		// Horizontal
		auto sprtH0 = Sprite::createWithSpriteFrameName("crystals/bomb-explo.png");
		sprtH0->setBlendFunc(BlendFunc::ADDITIVE);
		sprtH0->setPosition(center);
		sprtH0->setScaleX(5);
		sprtH0->runAction(ScaleTo::create(0.5, 30, 1));
		sprtH0->runAction(Sequence::createWithTwoActions(FadeOut::create(0.5), CallFunc::create(
             CC_CALLBACK_0(GameScene::onRemoveFromParent, this, sprtH0))));
		mEffectsLayer->addChild(sprtH0);
        
		// Vertical
		auto sprtV0 = Sprite::createWithSpriteFrameName("crystals/bomb-explo.png");
		sprtV0->setBlendFunc(BlendFunc::ADDITIVE);
		sprtV0->setPosition(center);
		sprtV0->setScaleY(5);
		sprtV0->runAction(ScaleTo::create(0.5, 1, 30));
		sprtV0->runAction(Sequence::createWithTwoActions(FadeOut::create(0.5), CallFunc::create(
            CC_CALLBACK_0(GameScene::onRemoveFromParent, this, sprtV0))));
		mEffectsLayer->addChild(sprtV0);
        
		// Horizontal
		auto sprtH1 = Sprite::createWithSpriteFrameName("crystals/bomb-explo-inner.png");
		sprtH1->setBlendFunc(BlendFunc::ADDITIVE);
		sprtH1->setPosition(center);
		sprtH1->setScaleX(0.5);
		sprtH1->runAction(ScaleTo::create(0.5, 8, 1));
		sprtH1->runAction(Sequence::createWithTwoActions(FadeOut::create(0.5), CallFunc::create(
            CC_CALLBACK_0(GameScene::onRemoveFromParent, this, sprtH1))));
		mEffectsLayer->addChild(sprtH1);
        
		// Vertical
		auto sprtV1 = Sprite::createWithSpriteFrameName("crystals/bomb-explo-inner.png");
		sprtV1->setRotation(90);
		sprtV1->setBlendFunc(BlendFunc::ADDITIVE);
		sprtV1->setPosition(center);
		sprtV1->setScaleY(0.5);
		sprtV1->runAction(ScaleTo::create(0.5, 8, 1));
		sprtV1->runAction(Sequence::createWithTwoActions(FadeOut::create(0.5), CallFunc::create(
            CC_CALLBACK_0(GameScene::onRemoveFromParent, this, sprtV1))));
		mEffectsLayer->addChild(sprtV1);
	}
    
	return removedGems;

}

void GameScene::removeMarkedGems()
{
    // Iterate through the board
	for (int x = 0; x < kBoardWidth; x++)
	{
		for (int y = 0; y < kBoardHeight; y++)
		{
			int i = x + y * kBoardWidth;
            
			if (mBoard[i] < -1)
			{
				// Increase the count for negative crystal types
				mBoard[i]++;
				if (mBoard[i] == -1)
				{
					mNumGemsInColumn[x]--;
					mBoardChangedSinceEvaluation = true;
                    
					// Transform any gem above this to a falling gem
					for (int yAbove = y+1; yAbove < kBoardHeight; yAbove++)
					{
						int idxAbove = x + yAbove*kBoardWidth;
                        
						if (mBoard[idxAbove] < -1)
						{
							mNumGemsInColumn[x]--;
							mBoard[idxAbove] = -1;
						}
						if (mBoard[idxAbove] == -1) continue;
                        
						// The gem is not connected, make it into a falling gem
						int gemType = mBoard[idxAbove];
						auto gemSprite = mBoardSprites[idxAbove];
                        GameScene::Gem gem;
                        gem.gemType = gemType;
                        gem.gemSprite = gemSprite;
                        gem.yPos = yAbove;
                        gem.ySpeed = 0;
						mFallingGems[x].push_back(gem);
                        
						// Remove from board
						mBoard[idxAbove] = -1;
						mBoardSprites[idxAbove] = NULL;
                        
						mNumGemsInColumn[x]--;
					}
                    
				}
			}
		}
	}
}

int GameScene::getGemType(int x, int y)
{
    if (x < 0 || x >= kBoardWidth) return -1;
	if (y < 0 || y >= kBoardHeight) return -1;
    
	return mBoard[x+y*kBoardWidth];
}

void GameScene::setGemType(int x, int y, int newType)
{
    // Check bounds
	if (x < 0 || x >= kBoardWidth) return;
	if (y < 0 || y >= kBoardHeight) return;
    
	// Get the type of the gem
	int idx = x + y*kBoardWidth;
	int gemType = mBoard[idx];
    
	// Make sure that it is a gem
	if (gemType < 0 || gemType >= 5) return;
    
	mBoard[idx] = newType;
    
	// Remove old gem and insert a new one
	mGameLayer->removeChild(mBoardSprites[idx]);
    
	auto gemSprite = Sprite::createWithSpriteFrameName(
        String::createWithFormat("crystals/%d.png", newType)->getCString());
	gemSprite->setPosition(x * kGemSize, y * kGemSize);
	gemSprite->setAnchorPoint(Point(0,0));
    
	mGameLayer->addChild(gemSprite);
	mBoardSprites[idx] = gemSprite;
    
	mBoardChangedSinceEvaluation = true;
}

int GameScene::findMove()
{
    if (!mBoardChangedSinceEvaluation)
	{
		return mPossibleMove;
	}
    
	// Iterate through all places on the board
	for (int y = 0; y < kBoardHeight; y++)
	{
		for (int x = 0; x < kBoardWidth; x++)
		{
			int idx = x + y*kBoardWidth;
			int gemType = mBoard[idx];
            
			// Make sure that it is a gem
			if (gemType < 0 || gemType >= 5) continue;
            
			// Check surrounding tiles
			int numSimilar = 0;
            
			if (getGemType(x-1, y) == gemType) numSimilar++;
			if (getGemType(x+1, y) == gemType) numSimilar++;
			if (getGemType(x, y-1) == gemType) numSimilar++;
			if (getGemType(x, y+1) == gemType) numSimilar++;
            
			if (numSimilar >= 2)
			{
				mPossibleMove = idx;
				return idx;
			}
		}
	}
	mBoardChangedSinceEvaluation = false;
	mPossibleMove = -1;
	return -1;

}

void GameScene::createRandomMove()
{
    // Find a random place in the lower part of the board
	int x = (rand() % kBoardWidth);
	int y = (rand() % (kBoardHeight/2));
    
	// Make sure it is a gem that we found
	int gemType = mBoard[x+y*kBoardWidth];
	if (gemType < 0 || gemType >= 5) return;
    
	// Change the color of two surrounding gems
	setGemType(x+1, y, gemType);
	setGemType(x, y+1, gemType);
    
	mBoardChangedSinceEvaluation = true;

}

void GameScene::createGameOver()
{
	for (int x = 0; x < kBoardWidth; x++)
	{
		Gems &column = mFallingGems[x];
		for (int i = 0; i < column.size(); i++)
		{
			Gem &gem = column[i];
            
			int ySpeed = (rand()%200-100) * kGameOverGemSpeed / 100;
			int xSpeed = (rand()%200-100) * kGameOverGemSpeed / 100;
            
            GameOverGem gameOverGem;
            gameOverGem.sprite = gem.gemSprite;
            gameOverGem.xPos = x;
            gameOverGem.yPos = gem.yPos;
            gameOverGem.ySpeed = ySpeed;
            gameOverGem.xSpeed = xSpeed;
			mGameOverGems.push_back(gameOverGem);
		}
        
		for (int y = 0; y < kBoardHeight; y++)
		{
			int i1 = x + y * kBoardWidth;
			if (mBoardSprites[i1])
			{
				float ySpeed1 = (float)(rand()%200-100)*kGameOverGemSpeed / 100;
				float xSpeed1 = (float)(rand()%200-100)*kGameOverGemSpeed / 100;
                
                GameOverGem gameOverGem;
                gameOverGem.sprite = mBoardSprites[i1];
                gameOverGem.xPos = x;
                gameOverGem.yPos = y;
                gameOverGem.ySpeed = ySpeed1;
                gameOverGem.xSpeed = xSpeed1;
                mGameOverGems.push_back(gameOverGem);
			}
		}
	}
    
	mHintLayer->removeAllChildren();
    
	removeShimmer();
}

void GameScene::updateGameOver()
{
    for (int i = 0; i < mGameOverGems.size(); i++)
	{
		GameOverGem &gem = mGameOverGems[i];
        
		gem.xPos = gem.xPos + gem.xSpeed;
		gem.yPos = gem.yPos + gem.ySpeed;
		gem.ySpeed -= kGameOverGemAcceleration;
        
		gem.sprite->setPosition(gem.xPos*kGemSize, gem.yPos*kGemSize);
	}
}

void GameScene::displayHint()
{
    mIsDisplayingHint = true;
    
	int idx = findMove();
	int x = idx % kBoardWidth;
	int y = floor((float)idx/kBoardWidth);
    
    std::vector<int> connected;
	findConnectedGems(x,y,connected);
    
	for (int i = 0; i < connected.size(); i++)
	{
		idx = connected[i];
		x = idx % kBoardWidth;
		y = floor((float)idx/kBoardWidth);
        
		auto actionFadeIn = FadeIn::create(0.5);
		auto actionFadeOut = FadeOut::create(0.5);
		auto actionSeq = Sequence::createWithTwoActions(actionFadeIn, actionFadeOut);
		auto action = RepeatForever::create(actionSeq);
        
		auto hintSprite = Sprite::createWithSpriteFrameName("crystals/hint.png");
		hintSprite->setOpacity(0);
		hintSprite->setPosition(x*kGemSize, y*kGemSize);
		hintSprite->setAnchorPoint(Point(0,0));
		mHintLayer->addChild(hintSprite);
		hintSprite->runAction(action);
	}
}

void GameScene::debugPrintBoard()
{
    for (int y = kBoardHeight-1; y >= 0; y--)
	{
		int i = kBoardWidth*y;
		CCLOG("%d%d%d%d%d%d%d%d",
        mBoard[i],mBoard[i+1],mBoard[i+2],mBoard[i+3],mBoard[i+4],mBoard[i+5],mBoard[i+6],mBoard[i+7]);
	}
	CCLOG("--------");
	CCLOG("%d%d%d%d%d%d%d%d",
          mNumGemsInColumn[0],mNumGemsInColumn[1],mNumGemsInColumn[2],mNumGemsInColumn[3],
    mNumGemsInColumn[4],mNumGemsInColumn[5],mNumGemsInColumn[6],mNumGemsInColumn[7]);
}

void GameScene::setupShimmer()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("gamescene/shimmer.plist");
    
	for (int i = 0; i < 2; i++)
	{
		auto sprt = Sprite::createWithSpriteFrameName(String::createWithFormat(
            "gamescene/shimmer/bg-shimmer-%d.png",i)->getCString());
        
		ActionInterval *seqRot = NULL;
		ActionInterval *seqMov = NULL;
		ActionInterval *seqSca = NULL;
		
		int x = 0;
		int y = 0;
		float rot = 0;
        
		for (int j = 0; j < 10; j++)
		{
			int time = rand()%10+5;
			x = kBoardWidth*kGemSize/2;
			y = random()%kBoardHeight*kGemSize;
			float rot = rand()%180-90;
			float scale = (float)(rand()%300 + 300)/100;
            
			ActionInterval * actionRot = EaseInOut::create(RotateTo::create(time, rot), 2);
			ActionInterval * actionMov = EaseInOut::create(MoveTo::create(time, Point(x,y)), 2);
			ActionInterval * actionSca = ScaleTo::create(time, scale);
            
			if (!seqRot)
			{
				seqRot = actionRot;
				seqMov = actionMov;
				seqSca = actionSca;
			}
			else
			{
				seqRot = Sequence::createWithTwoActions(seqRot, actionRot);
				seqMov = Sequence::createWithTwoActions(seqMov, actionMov);
				seqSca = Sequence::createWithTwoActions(seqSca, actionSca);
			}
		}
        
		x = kBoardWidth*kGemSize/2;
		y = rand()*(kBoardHeight*kGemSize);
		rot = rand()*180-90;
        
		sprt->setPosition(x,y);
		sprt->setRotation(rot);
        
		sprt->setPosition(kBoardWidth*kGemSize/2, kBoardHeight*kGemSize/2);
        BlendFunc bf{GL_SRC_ALPHA, GL_ONE};
		sprt->setBlendFunc(bf);
		sprt->setScale(3);
        
		mShimmerLayer->addChild(sprt);
		sprt->setOpacity(0);
		sprt->runAction(RepeatForever::create(seqRot));
		sprt->runAction(RepeatForever::create(seqMov));
		sprt->runAction(RepeatForever::create(seqSca));
        
		sprt->runAction(FadeIn::create(2));
	}
}

void GameScene::removeShimmer()
{
    Vector<Node *> &children = mShimmerLayer->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		children.at(i)->runAction(FadeOut::create(1));
	}
}

void GameScene::updateSparkle()
{
    int percent = rand()%100;
    if (percent > 10) return;
	int idx = rand()%kNumTotalGems;
	auto gemSprite = mBoardSprites[idx];
	if (mBoard[idx] < 0 || mBoard[idx] >= 5) return;
	if (!gemSprite) return;
    Vector<Node *> &children = gemSprite->getChildren();
	if (children.size() > 0) return;
    
	auto sprite = Sprite::createWithSpriteFrameName("crystals/sparkle.png");
	sprite->runAction(RepeatForever::create(RotateBy::create(3, 360)));
    
	sprite->setOpacity(0);
    
	sprite->runAction(Sequence::create(FadeIn::create(0.5),FadeOut::create(2),
         CallFunc::create(CC_CALLBACK_0(GameScene::onRemoveFromParent, this, sprite)), NULL));
    
	sprite->setPosition(kGemSize*(2/6), kGemSize*(4/6));
    
	gemSprite->addChild(sprite);

}

void GameScene::updatePowerPlay()
{
    bool powerPlay = (mNumConsecutiveGems >= 5);
	if (powerPlay == mIsPowerPlay) return;
    
	if (powerPlay)
	{
		// Start power-play
		mPowerPlayParticles = ParticleSystemQuad::create("particles/power-play.plist");
		mPowerPlayParticles->setAutoRemoveOnFinish(true);
		mParticleLayer->addChild(mPowerPlayParticles);
	}
	else
	{
		// Stop power-play
		if (mPowerPlayParticles)
		{
			mPowerPlayParticles->stopSystem();
		}
	}
    
	mIsPowerPlay = powerPlay;
}

void GameScene::addScore(int score)
{
    if (mIsPowerPlay) score *= 3;
	mScore += score;
	mLabelScore->setString(String::createWithFormat("%d",mScore)->getCString());
}