#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

#include <Geode/ui/Button.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

template <string::ConstexprString S, typename T>
static const T& getSetting() {
    static T value = (
        listenForSettingChanges<T>(S.data(), [](T v) {
            value = v;
        }),
        Mod::get()->getSettingValue<T>(S.data())
    );

    return value;
}

class $modify(EditorUI) {
    
    bool objectCheck(GameObject* obj) {
        return obj->getScaleX() >= getSetting<"threshold", float>() || obj->getScaleY() >= getSetting<"threshold", float>();  
    }

    void updateProObjects() {
        if (!getSetting<"enabled", bool>()) {
            return;
        }

        if (m_selectedObject) {
            m_editorLayer->updateExtendedCollision(m_selectedObject, objectCheck(m_selectedObject));
        }

        if (m_selectedObjects) {
            for (auto obj : CCArrayExt<GameObject*>(m_selectedObjects)) {
                m_editorLayer->updateExtendedCollision(obj, objectCheck(obj));
            }
        }
    }

    void transformObjectsActive() {
        EditorUI::transformObjectsActive();
        updateProObjects();
    }

    void scaleXChanged(float p0, bool p1) {
        EditorUI::scaleXChanged(p0, p1);
        updateProObjects();
    }

    void scaleYChanged(float p0, bool p1) {
        EditorUI::scaleYChanged(p0, p1);
        updateProObjects();
    }
    
    void scaleXYChanged(float p0, float p1, bool p2) {
        EditorUI::scaleXYChanged(p0, p1, p2);
        updateProObjects();
    }

};

class $modify(ProEditorPauseLayer, EditorPauseLayer) {

    struct Fields {
        CCMenuItemToggler* toggle = nullptr;
    };

    static void onModify(auto& self) {
        (void)self.setHookPriorityAfterPost("EditorPauseLayer::init", "undefined0.object_glow_in_editor");
    }

    void onToggle(CCObject*) {
        Mod::get()->setSavedValue("enabled", !getSetting<"enabled", bool>());
    }

    bool init(LevelEditorLayer* p0) {
        if (!EditorPauseLayer::init(p0)) {
            return false;
        }

        auto menu = getChildByID("options-menu");

        if (!menu) {
            return true;
        }

        auto objGlow = Loader::get()->isModLoaded("undefined0.object_glow_in_editor");
        auto imp = Loader::get()->isModLoaded("razoom.improved_playtest");

        // if (!objGlow && !imp) {
        //     menu->setScale(.95f);
        //     menu->setPositionY(menu->getPositionY() - 10.f);
        //     menu->setPositionX(menu->getPositionX() - 8.f);
        // }

        // auto toggler = GameToolbox::createToggleButton(
        //     "Auto Extended Collision",
        //     menu_selector(ProEditorPauseLayer::onToggle),
        //     getSetting<"enabled", bool>(),
        //     static_cast<CCMenu*>(menu), { imp ? 23.5f : 19.159f, imp ? 267.962f : 270.962f },
        //     this, this,
        //     .55f, .42f, 85.f,
        //     { imp ? 2.f : 5.f, 0.f },
        //     "bigFont.fnt",
        //     false, 0, nullptr
        // );

        // toggler->setID("toggler"_spr);

        // if (objGlow) {
        //     toggler->setPositionY(toggler->getPositionY() - 21);
        // }

        // if (auto lbl = getChildByType<CCLabelBMFont*>(-1)) {
        //     lbl->removeFromParent();
        //     lbl->setAnchorPoint({0.f, 0.5f});
        //     lbl->setID("label"_spr);
        //     lbl->setPosition(toggler->getPosition() + CCPoint{15, 0});

        //     menu->addChild(lbl);

        //     m_fields->toggle = toggler;

        //     auto btn = Button::createWithSpriteFrameName("GJ_optionsBtn_001.png", [](Button*) {
        //         openSettingsPopup(Mod::get(), false);
        //     });
        //     btn->setPosition(lbl->getPosition() + CCPoint{lbl->getScaledContentWidth() + 15, 0});
        //     btn->setScale(0.385f);

        //     menu->addChild(btn);
        // }

        if (!objGlow) {
            auto infoMenu = getChildByID("info-menu");
            if (!infoMenu) return true;

            auto lengthLabel = infoMenu->getChildByID("length-label");
            if (!lengthLabel) return true;

            lengthLabel->setPositionY(lengthLabel->getPositionY() + 3.f);
        }

        // if (objGlow || imp) {
        //     auto countTotal = 0;
        //     auto first = true;

        //     for (auto node : menu->getChildrenExt()) {
        //         node->setPositionY(node->getPositionY() - ((11 + static_cast<int>(imp)) - countTotal) * 1.65f);

        //         if (imp && first) {
        //             countTotal++;
        //             first = false;
        //         }

        //         if (typeinfo_cast<CCLabelBMFont*>(node)) {
        //             countTotal++;
        //         }
        //     }
        // }

        return true;
    }

};

$on_mod(Loaded) {
    listenForKeybindSettingPresses("toggle", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down && !repeat) {
            Mod::get()->setSettingValue("enabled", !getSetting<"enabled", bool>());

            if (auto lel = LevelEditorLayer::get()) {
                if (auto pauseLayer = lel->getChildByType<EditorPauseLayer*>(0)) {
                    if (auto toggle = static_cast<ProEditorPauseLayer*>(pauseLayer)->m_fields->toggle) {
                        toggle->toggle(getSetting<"enabled", bool>());
                    }
                }
            }
        }
    });
}