#pragma once


#include "extension.h"

#include <edict.h>
#include <iplayerinfo.h>


class CBot
{

public:
	CBot(edict_t* pEdict);

public:
	void Think();
	void PostThink();
	edict_t* GetEdict() const { return m_pEdict; }
	int GetIndex() const { return gamehelpers->IndexOfEdict(m_pEdict); }
	void ChangeTeam(int iTeamNum);
	const char* GetName();
	void runCommand(CBotCmd* ucmd) const { m_pBotController->RunPlayerMove(ucmd); }
private:
	edict_t* m_pEdict;
	IBotController* m_pBotController;
	IPlayerInfo* m_pPlayerInfo;
};