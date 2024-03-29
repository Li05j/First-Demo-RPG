#ifndef _ENEMY_H_de55fadc_40fc_4c8b_a94b_4b5d363df06a_
#define _ENEMY_H_de55fadc_40fc_4c8b_a94b_4b5d363df06a_

#include "./src/blueprint/combatUnits/combatUnits.h"

class Enemy : public CombatUnits {
public:
	Enemy(UnitType type);
	virtual ~Enemy();

	// debug use
	virtual void debugPrintUnitInfo() override;
};

#endif
