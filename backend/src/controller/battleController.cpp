#include "battleController.h"

#include <algorithm>

#include "./src/blueprint/combatUnits/enemy/enemy.h"
#include "./src/blueprint/combatUnits/ally/ally.h"

#include "./src/model/allyModel.h"
#include "./src/model/battleModel.h"
#include "./src/model/battleLoggerModel.h"
#include "./src/model/enemyModel.h"
#include "./src/model/playerInfoModel.h"

#include "./src/utils/debugUtils.h"
#include "./src/utils/helperUtils.h"
#include "./src/utils/playerUtils.h"

BattleController::BattleController(
	BattleModel& battleM,
	AllyModel& allyM,
	EnemyModel& enemyM,
	PlayerInfoModel& playerM,
	BattleLoggerModel& battleLogM) :
	m_battleM(battleM), m_allyM(allyM), m_enemyM(enemyM), m_playerM(playerM), m_battleLogM(battleLogM)
{}

BattleController::~BattleController() {}

BattleController& BattleController::getInstance(
	BattleModel& battleM,
	AllyModel& allyM,
	EnemyModel& enemyM,
	PlayerInfoModel& playerM,
	BattleLoggerModel& battleLogM)
{
	static BattleController instance(battleM, allyM, enemyM, playerM, battleLogM);
	return instance;
}

void BattleController::prepareBattle() {
	populateAllyBattle();
	populateEnemyBattle();
	m_battleM.resetbattleVictory();
	m_battleM.resetMoveOrder();
	m_battleM.resetTurnCounter();
	startBattle();
}

void BattleController::startBattle()
{
	while (!m_battleM.isBattleEnd()) {
		m_battleM.incrementTurnCounter();
		preTurn();
		// DEBUG(DB_GENERAL, "preturn owo\n");
		duringTurn();
		// DEBUG(DB_GENERAL, "duringturn owo\n");
		postTurn();
		// DEBUG(DB_GENERAL, "postturn owo\n");
	}
	endBattle();
}

void BattleController::populateAllyBattle() {
	const std::vector<int>& playerParty = m_playerM.getPlayerParty();
	int allyCount = 0;
	for (int i = 0; i < TOTAL_ALLY; i++) {
		if (playerParty.at(i)) {
			m_battleM.pushAllyForBattle(m_allyM.getAlly(i));
			allyCount++;
		}
	}

	if (allyCount > MAX_PARTY_SIZE) {
		DEBUG(DB_GENERAL, "ERROR -- populateEnemyBattle(): too many allies, ally count: %d", allyCount);
	}
}

void BattleController::populateEnemyBattle() {
	// TODO: random multiple enemies
	// TODO: will need to check level/location etc to get appropriate enemies

	int randomEnemy = generateRandomNumber(1, 2);
	m_battleM.pushEnemyForBattle(m_enemyM.getEnemy(randomEnemy));
}

void BattleController::preTurn()
{
	m_battleM.resetMoveOrder();
	// determineMoveOrder();
	// wait(1);
}

void BattleController::duringTurn()
{
	// attack
	auto& moveOrder = m_battleM.getMoveOrder();
	for (auto unit : moveOrder) {
		auto& allyBattleUnits = m_battleM.getAllyBattle();
		auto& enemyBattleUnits = m_battleM.getEnemyBattle();

		// since there is only one ally and one enemy, subject and target are all 0.
		int subjectIdx = 0;
		int targetIdx = 0;

		auto& ally = allyBattleUnits.at(subjectIdx);
		auto& enemy = enemyBattleUnits.at(targetIdx);

		if (unit->getUnitType() == ALLY_UNIT) {
			// TODO: below is dummy damage calculation. need to replace it to a meaningful one.
			useDefaultAttack(ally.getName(), enemy.getName(), enemy.getUnitType(), targetIdx, 20);
		}
		else if (unit->getUnitType() == ENEMY_UNIT) {
			// TODO: below is dummy damage calculation. need to replace it to a meaningful one.
			useDefaultAttack(enemy.getName(), ally.getName(), ally.getUnitType(), targetIdx, 20);
		}
		else {
			// should'nt end up here
			DEBUG(DB_GENERAL, "ERROR - duringTurn(): unknown unit type: %d.\n", unit->getId());
		}
		checkEnemyPartyWipe();
		checkAllyPartyWipe();
	}
}

void BattleController::postTurn()
{
	// do nothing for now;
	// wait(1);
}

void BattleController::checkAllyPartyWipe()
{
	auto& allyBattle = m_battleM.getAllyBattle();
	for (auto& unit : allyBattle) {
		if (unit.getOneBParam(B_CURRHP) > 0) {
			return;
		}
	}
	m_battleM.gameOver();
	m_battleM.resetMoveOrder();
}

void BattleController::checkEnemyPartyWipe()
{
	auto& m_enemyBattle = m_battleM.getEnemyBattle();
	for (auto& unit : m_enemyBattle) {
		if (unit.getOneBParam(B_CURRHP) > 0) {
			return;
		}
	}
	m_battleM.battleVictory();
	m_battleM.resetMoveOrder();
}

// bool BattleController::isEndBattle()
// {
// 	// DEBUG(DB_GENERAL, "Note -- isEndBattle(): gameover?: %d, victory?: %d\n", m_gameover, m_victory);
// 	if (m_gameover || m_victory) {
// 		return true;
// 	}
// 	return false;
// }

// void BattleController::determineMoveOrder()
// {
// 	auto& allyBattle = m_battleM.getAllyBattle();
// 	auto& enemyBattle = m_battleM.getEnemyBattle();
// 	auto& moveOrder = m_battleM.getMoveOrder();

// 	for (auto& unit : allyBattle) {
// 		m_move_order.push_back(&unit);
// 	}
// 	for (auto& unit : enemyBattle) {
// 		m_move_order.push_back(&unit);
// 	}

// 	std::sort(m_move_order.begin(), m_move_order.end(), [](CombatUnits* a, CombatUnits* b) {
// 		return a->getOneBParam(BasicParamType::B_SPD) > b->getOneBParam(BasicParamType::B_SPD);
// 		});
// }

void BattleController::useDefaultAttack(std::string subjectName, std::string targetName, UnitType targetUnitType, int targetIdx, int damage)
{
	m_battleLogM.pushBattleLog(damageLog(subjectName, targetName, 20)); // dummy damage number
	m_battleM.changeUnitParam(targetUnitType, targetIdx, -damage);
}

void BattleController::endBattle() {
	m_battleM.resetAllyBattle();
	m_battleM.resetEnemyBattle();
}

std::string BattleController::damageLog(std::string subject, std::string object, int damage, bool crit)
{
	std::string log = std::string();
	if (!crit) {
		log = subject + " dealt " + std::to_string(damage) + " damage to " + object + " !";
	}
	return log;
}

// const int BattleController::getTurn() const
// {
// 	return m_battleM.getTurn();
// }

// const std::vector<Ally>& BattleController::getAllyBattle() const {
// 	return m_battleM.getAllyBattle();
// }

// const std::vector<Enemy>& BattleController::getEnemyBattle() const {
// 	return m_battleM.getEnemyBattle();
// }

const std::deque<std::string>& BattleController::getBattleLogs() const
{
	return m_battleLogM.getBattleLogs();
}