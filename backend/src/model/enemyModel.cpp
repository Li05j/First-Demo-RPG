#include "enemyModel.h"

#include <fstream>
// #include <limits>

#include "./src/blueprint/combatUnits/enemy/enemy.h"
#include "./src/utils/combatUnitsUtils.h"
#include "./src/utils/debugUtils.h"

EnemyModel::EnemyModel() : m_dataFileName("./src/data/enemy_data1.txt") {
    populateEnemyData();
}

EnemyModel::~EnemyModel() {}

EnemyModel& EnemyModel::getInstance() {
    static EnemyModel instance;
    return instance;
}

void EnemyModel::populateEnemyData() {
    std::ifstream file(m_dataFileName);

    std::string line;
    Enemy currentEnemy(ENEMY_UNIT);

    while (std::getline(file, line)) {
        // DEBUG(DB_GENERAL, "populateEnemyData(): %s\n", line.c_str());
        if (line.empty()) {
            m_enemyData.push_back(currentEnemy);
            continue;
        }

        // + 2 for strings to skip the space after the colon
        if (line.find("ID:") != std::string::npos) {
            currentEnemy.setId(std::stoi(line.substr(line.find(":") + 1)));
        }
        else if (line.find("Name:") != std::string::npos) {
            currentEnemy.setName(line.substr(line.find(":") + 2));
        }
        else if (line.find("HP:") != std::string::npos) {
            currentEnemy.setBPGrowth(B_MAXHP, std::stoi(line.substr(line.find(":") + 1)));
        }
        else if (line.find("ATK:") != std::string::npos) {
            currentEnemy.setBPGrowth(B_ATK, std::stoi(line.substr(line.find(":") + 1)));
        }
        else if (line.find("DEF:") != std::string::npos) {
            currentEnemy.setBPGrowth(B_DEF, std::stoi(line.substr(line.find(":") + 1)));
        }
        else if (line.find("SPD:") != std::string::npos) {
            currentEnemy.setBPGrowth(B_SPD, std::stoi(line.substr(line.find(":") + 1)));
        }
    }

    file.close();

    if (m_enemyData.empty()) {
        DEBUG(DB_GENERAL, "ERROR -- populateEnemyData(): No enemy is popullated.\n");
    }
}

Enemy& EnemyModel::getEnemy(int enemyId) {
    if (enemyId >= 0 && m_enemyData.size() > enemyId) {
        return m_enemyData.at(enemyId);
    }
    DEBUG(DB_GENERAL, "ERROR -- getEnemy(): enemyId OutOfRange. enemyId = %d.\n", enemyId);
    return m_enemyData.at(0);
}

// lazy loading implementation
// std::shared_ptr<Enemy> EnemyController::getEnemyData(int enemyId) {
//     DEBUG(DB_GENERAL, "getEnemyData() called %d\n", enemyId);
//     if (m_enemyData.find(enemyId) == m_enemyData.end()) {
//         // Enemy data not loaded, load from file
//         std::ifstream file("./src/blueprint/combatUnits/enemy/enemy_data1.txt");
//         std::string line;
//         Enemy currentEnemy;

//         int skip = enemyId * (ENEMY_DATA_LINES + 1);

//         // skip lines
//         for (int i = 0; i < skip; i++) {
//             file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//         }

//         // populate data
//         for (int i = 0; i < ENEMY_DATA_LINES; i++) {
//             std::getline(file, line);
//             DEBUG(DB_GENERAL, "getEnemyData(): %s\n", line.c_str());

//             if (line.find("ID:") != std::string::npos) {
//                 currentEnemy.setId(std::stoi(line.substr(line.find(":") + 1)));
//             }
//             else if (line.find("Name:") != std::string::npos) {
//                 currentEnemy.setName(line.substr(line.find(":") + 1));
//             }
//             else if (line.find("HP:") != std::string::npos) {
//                 int hp = std::stoi(line.substr(line.find(":") + 1));
//                 currentEnemy.setBasicParam(B_MAXHP, hp);
//                 currentEnemy.setBasicParam(B_CURRHP, hp);
//             }
//             else if (line.find("ATK:") != std::string::npos) {
//                 currentEnemy.setBasicParam(B_ATK, std::stoi(line.substr(line.find(":") + 1)));
//             }
//             else if (line.find("DEF:") != std::string::npos) {
//                 currentEnemy.setBasicParam(B_DEF, std::stoi(line.substr(line.find(":") + 1)));
//             }
//         }

//         if (currentEnemy.getId() == enemyId) {
//             m_enemyData[enemyId] = std::make_shared<Enemy>(currentEnemy);
//         }
//         else {
//             // should not reach;
//             DEBUG(DB_GENERAL, "Wrong enemy created! expected id: %d, actual id: %d.\n", enemyId, currentEnemy.getId());
//         }

//         file.close();
//     }

//     return m_enemyData[enemyId];
// }