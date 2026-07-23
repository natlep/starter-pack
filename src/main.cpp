#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/ui/MDTextArea.hpp>

using namespace geode::prelude;

class MyMdTextera : public FLAlertLayer {
public:
    async::TaskHolder<web::WebResponse> m_request;
    MDTextArea* m_mdArea = nullptr;
    CCLabelBMFont* m_loadingLabel = nullptr;

    static MyMdTextera* create() {
        auto ret = new MyMdTextera();
        if (ret && ret->init(150)) {ret->autorelease(); return ret;}
        delete ret;
        return nullptr;
    }

    bool init(int opacity) {
        if (!FLAlertLayer::init(opacity)) return false;
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        const float Width = 400.f;
        const float Height = 280.f;

        m_mainLayer->setContentSize({Width, Height}),
        m_mainLayer->setPosition(winSize / 2);
        m_mainLayer->ignoreAnchorPointForPosition(false);

        auto cornerBL = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        cornerBL->setPosition({0.f, 0.f});
        cornerBL->setAnchorPoint({0.f, 0.f});
        m_mainLayer->addChild(cornerBL, 1);
        
        auto cornerBR = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        cornerBR->setPosition({Width, 0.f});
        cornerBR->setAnchorPoint({1.f, 0.f});
        cornerBR->setFlipX(true);
        m_mainLayer->addChild(cornerBR, 1);

        auto cornerTL = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        cornerTL->setPosition({0.f, Height});
        cornerTL->setAnchorPoint({0.f, 1.f});
        cornerTL->setFlipY(true);
        m_mainLayer->addChild(cornerTL, 1);

        auto cornerTR = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        cornerTR->setPosition({Width, Height});
        cornerTR->setAnchorPoint({1.f, 1.f});
        cornerTR->setFlipX(true);
        cornerTR->setFlipY(true);
        m_mainLayer->addChild(cornerTR, 1);

        auto bg = CCScale9Sprite::create("GJ_square04.png", {0, 0, 80, 80});
        bg->setContentSize({Width, Height});
        bg->setPosition({Width / 2, Height / 2});
        m_mainLayer->addChild(bg, -1);

        auto title = CCLabelBMFont::create("Starter Pack", "goldFont.fnt");
        title->setScale(0.8f);
        title->setPosition({Width / 2, Height - 20.f});
        m_mainLayer->addChild(title, 1);

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this, menu_selector(MyMdTextera::onClose)
        );
        closeBtn->setPosition({0.f, Height - 10.f});
        menu->addChild(closeBtn);

        auto refreshBtnSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        auto refreshBtn = CCMenuItemSpriteExtra::create(
            refreshBtnSprite,
            this, menu_selector(MyMdTextera::onRefresh)
        );
        refreshBtn->setPosition({Width, Height - 10.f});
        menu->addChild(refreshBtn);

        m_mainLayer->addChild(menu, 3);
        m_loadingLabel = CCLabelBMFont::create("chargement...", "goldFont.fnt");
        m_loadingLabel->setScale(0.6f);
        m_loadingLabel->setPosition({Width / 2, Height / 2 - 10.f});
        m_loadingLabel->setVisible(false);
        m_mainLayer->addChild(m_loadingLabel);

        this->loadData();
        this->show();
        return true;

    }
    void onRefresh(CCObject*) {
        this->loadData();

    }

    void loadData() {
        const float Width = 400.f;
        const float Height = 280.f;
        const float mdWidth = Width - 40.f;
        const float mdHeight = Height - 60.f;
        m_request.cancel();

        if (m_mdArea) {
            m_mdArea->removeFromParent();
            m_mdArea = nullptr;
        }

        if (m_loadingLabel) {
            m_loadingLabel->setVisible(true);
        }

        auto req = web::WebRequest();

        std::string apiUrl = "https://5imcrj45k438p74gyfe7.c.websim.com/starterpack.md";

        m_request.spawn(
            req.get(apiUrl),
            [this, mdWidth, mdHeight, Width, Height](web::WebResponse res) {
                if (m_loadingLabel) {
                    m_loadingLabel->setVisible(false);

                }
                std::string markdownText;
                if (res.ok()) {
                    markdownText = res.string().unwrapOr("error");

                } else {
                    markdownText= fmt::format("# error\nImpossible de charger les infos.\n\nCode HTTP : {}", res.code());

                }
                m_mdArea = MDTextArea::create(markdownText, {mdWidth, mdHeight});
                m_mdArea->setPosition({Width / 2, Height / 2 - 15.f});
                this->m_mainLayer->addChild(m_mdArea);
            }
        );
    }
    void onClose(CCObject*) {
        this->removeFromParentAndCleanup(true);

    }
};

class $modify(MyMenuLayer, MenuLayer) {
public:
    bool init() {
        if (!MenuLayer::init()) return false;
        auto bottomMenu = this->getChildByID("bottom-menu");
        auto button = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_gkBtn_001.png"),
            this,
            menu_selector(MyMenuLayer::onButtonClick)
        );
        bottomMenu->addChild(button);
        return true;
    }
    void onButtonClick(CCObject*) {
        MyMdTextera::create();
    }
};