#include "ModProblemList.hpp"
#include "GUI/CCControlExtension/CCScale9Sprite.h"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "ui/mods/list/ModProblemItem.hpp"

#include <Geode/cocos/platform/CCPlatformMacros.h>
#include <Geode/cocos/base_nodes/Layout.hpp>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/loader/Loader.hpp>

bool ModProblemList::init(
    std::vector<LoadProblem> problems,
    CCSize const& size
) {
    if (!CCNode::init()) {
        return false;
    }
    m_problems = problems;
    this->setContentSize(size);
    this->setAnchorPoint({ 0.5f, 0.5f });

    CCScale9Sprite* bg = CCScale9Sprite::create("square02b_001.png");
    bg->setColor({ 0, 0, 0 });
    bg->setOpacity(75);
    bg->setScale(.3f);
    bg->setContentSize(size / bg->getScale());
    m_bg = bg;
    this->addChildAtPosition(
        m_bg,
        Anchor::Center
    );

    // mfw fod created a scrolllayer with layouts
    m_list = ScrollLayer::create({ size.width - 10.f, size.height - 10.f });
    m_list->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(size.height)
            ->setGap(5.0f)
    );
    this->addChildAtPosition(
        m_list,
        Anchor::Center,
        -m_list->getScaledContentSize() / 2
    );

    CCSize itemSize = {
        m_list->getScaledContentWidth(),
        50.f
    };

    for (LoadProblem problem : m_problems) {
        m_list->m_contentLayer->addChild(ModProblemItem::create(problem, itemSize));
        m_list->m_contentLayer->addChild(ModProblemItem::create(problem, itemSize));
        m_list->m_contentLayer->addChild(ModProblemItem::create(problem, itemSize));
        m_list->m_contentLayer->addChild(ModProblemItem::create(problem, itemSize));
        m_list->m_contentLayer->addChild(ModProblemItem::create(problem, itemSize));
    }
    m_list->m_contentLayer->updateLayout();
    // Scroll list to top
    auto listTopScrollPos = -m_list->m_contentLayer->getContentHeight() + m_list->getContentHeight();
    m_list->m_contentLayer->setPositionY(listTopScrollPos);
    return true;
}

ModProblemList* ModProblemList::create(
    std::vector<LoadProblem> problems,
    CCSize const& size
) {
    auto ret = new ModProblemList();
    if (!ret || !ret->init(problems, size)) {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    ret->autorelease();
    return ret;
}