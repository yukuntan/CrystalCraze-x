#include "AppDelegate.h"
#include "MainScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto eglView = EGLView::getInstance();

    director->setOpenGLView(eglView);
    director->setProjection(cocos2d::Director::Projection::_2D);
    
    auto screenSize = EGLView::getInstance()->getFrameSize();
    auto designSize = Size(320, 480);
    auto resourceSize = Size(320, 480);
    
    std::vector<std::string> searchPaths = FileUtils::getInstance()->getSearchPaths();
    std::vector<std::string> resDirOrders;
    
    Application::Platform platform = Application::getInstance()->getTargetPlatform();
	if (platform == Application::Platform::OS_IPHONE ||
        platform == Application::Platform::OS_IPAD ||
        platform == Application::Platform::OS_MAC)
    {
        searchPaths.push_back("Published-iOS");
        FileUtils::getInstance()->setSearchPaths(searchPaths);
        
        if (screenSize.height > 480)
        {
            resourceSize = Size(640, 960);
            resDirOrders.push_back("resources-iphonehd");
        }
        else
        {
            resDirOrders.push_back("resources-iphone");
        }
        FileUtils::getInstance()->setSearchResolutionsOrder(resDirOrders);
    }
    else if (platform == Application::Platform::OS_ANDROID ||
             platform == Application::Platform::OS_WINDOWS)
    {
        if (screenSize.height > 960)
        {
            resourceSize = Size(640, 960);
            resDirOrders.push_back("resources-large");
            resDirOrders.push_back("resources-medium");
            resDirOrders.push_back("resources-small");
        }
        else if (screenSize.height > 480)
        {
            resourceSize = Size(480, 720);
            resDirOrders.push_back("resources-medium");
            resDirOrders.push_back("resources-small");
        }
        else
        {
            resourceSize = Size(320, 568);
            resDirOrders.push_back("resources-small");
        }
        FileUtils::getInstance()->setSearchResolutionsOrder(resDirOrders);
    }
    director->setContentScaleFactor(resourceSize.width / designSize.width);
    EGLView::getInstance()->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::SHOW_ALL);
    
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = MainScene::createScene();

    FileUtils::getInstance()->loadFilenameLookupDictionaryFromFile("fileLookup.plist");
    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
