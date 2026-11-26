#include "APUtils.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Archipelago.h>
#include <set>

using namespace geode::prelude;

class $modify(APGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        std::set<int> sentCoins; // each coin is sent once per level session
    };

    void processItems() {
        GJBaseGameLayer::processItems();
        auto playLayer = PlayLayer::get();
        if (!playLayer || !playLayer->m_level) return; // e.g. in the level editor
        auto levelObj = playLayer->m_level;
        if (APUtils::coinsEnabled && AP_GetConnectionStatus() == AP_ConnectionStatus::Authenticated) {
            auto levelID = levelObj->m_levelID.value();
            levelID = APUtils::checkIfTower(levelID);
            if (levelID >= 1 && levelID <= static_cast<int>(APUtils::levels.size())) {
                for (int i = 0; i < 3; i++) {
                    auto key = levelObj->getCoinKey(i+1);
                    auto hasCoin = GameStatsManager::sharedState()->hasSecretCoin(key);
                    if (hasCoin) {
                        auto itemID = (levelID * 1000) + (i+1) + 130820130;
                        if (m_fields->sentCoins.insert(itemID).second) {
                            APUtils::sendItem(itemID);
                            geode::log::info("sent coin item with id {}", itemID);
                        }
                    } else {
                        if (!key) {
                            geode::log::warn("couldnt find key for coin {}, level {}, id {}", i+1, levelID, levelObj->m_levelID.value());
                        }
                    }
                }
            }
        }
    }
};
