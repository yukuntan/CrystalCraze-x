//
//  GameScene.h
//  CrystalCraze
//
//  Created by 信徒 on 14-2-12.
//
//

#ifndef __CrystalCraze__GameScene__
#define __CrystalCraze__GameScene__

#include <vector>
#include "cocos2d.h"
#include "cocosbuilder/cocosbuilder.h"

class GameScene
: public cocos2d::Layer
, public cocosbuilder::NodeLoaderListener
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::CCBSelectorResolver
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_WITH_INIT_METHOD(GameScene, create);
    
    GameScene();
    virtual ~GameScene();
    
    virtual void onEnter();
    
    virtual void update(float delta);


    virtual void onNodeLoaded(cocos2d::Node * pNode, cocosbuilder::NodeLoader * pNodeLoader);
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Object* target, const char* memberVariableName, cocos2d::Node* node);
    
    virtual cocos2d::SEL_MenuHandler onResolveCCBCCMenuItemSelector(cocos2d::Object * pTarget, const char* pSelectorName);
    
    virtual cocos2d::extension::Control::Handler onResolveCCBCCControlSelector(cocos2d::Object * pTarget, const char* pSelectorName);
    
    void onPressPause(cocos2d::Object *sender);
    
    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches,
                        cocos2d::Event *unused_event);
    
    void onMouseDown(cocos2d::Event* event);
    
    void onRemoveFromParent(cocos2d::Node *node);
    
    void onAnimationComplete();
    
    void setAnimationManager(cocosbuilder::CCBAnimationManager *animationManager);
    
    static cocos2d::Scene *createScene();
    
protected:
    void processClick(const cocos2d::Point &location);
    
    
    void setBoard();
    
    void findConnectedGems(int x, int y, std::vector<int> &arr);
    
    void findConnectedGems_(int x, int y,
                            std::vector<int> &arr, int gemType);
    
    bool removeConnectedGems(int x, int y);
    
    bool activatePowerUp(int x, int y);
    
    void removeMarkedGems();
    
    int getGemType(int x, int y);
    
    void setGemType(int x, int y, int newType);
    
    int findMove();
    
    void createRandomMove();
    
    void createGameOver();
    
    void updateGameOver();
    
    void displayHint();
    
    void debugPrintBoard();
    
    void setupShimmer();
    
    void removeShimmer();
    
    void updateSparkle();
    
    void updatePowerPlay();
    
    void addScore(int score);
    
    
protected:
    cocosbuilder::CCBAnimationManager *mAnimationManager;
    bool mIsOver;
    
    cocos2d::Sprite *mTimer;
    cocos2d::LabelBMFont *mLabelScore;
    cocos2d::Layer *mGameLayer;
    cocos2d::Sprite *mSprtHeader;
    
    typedef struct Gem
    {
        int gemType;
        cocos2d::Sprite *gemSprite;
        float yPos;
        float ySpeed;
        
    }Gem;
    
    typedef struct GameOverGem
    {
        cocos2d::Sprite *sprite;
        float xPos;
        float yPos;
        float xSpeed;
        float ySpeed;
    }GameOverGem;

    
    typedef std::vector<Gem> Gems;
    
    Gems *mFallingGems;
    int *mBoard;
    std::vector<cocos2d::Sprite *> mBoardSprites;
    int *mNumGemsInColumn;
    float *mTimeSinceAddInColumn;
    
    int mScore;
    int mLastTakenGemTime;
    int mNumConsecutiveGems;
    bool mIsPowerPlay;
    cocos2d::ParticleSystemQuad *mPowerPlayParticles;
    cocos2d::Layer *mPowerPlayLayer;
    
    cocos2d::Node *mParticleLayer;
    cocos2d::Node *mHintLayer;
    cocos2d::Node *mShimmerLayer;
    cocos2d::Node *mEffectsLayer;
    
    cocos2d::ProgressTimer *mProgressTimer;
    
    float mTimeCurrent;
    float mLastMoveTime;
    bool mIsDisplayingHint;
    
    bool mBoardChangedSinceEvaluation;
    int mPossibleMove;
    
    std::vector<GameOverGem> mGameOverGems;
    bool mEndTimerStarted;
};

class GameSceneLoader
: public cocosbuilder::LayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(GameSceneLoader, loader);
    
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(GameScene);
};


#endif /* defined(__CrystalCraze__GameScene__) */
