#include "bot.h"
#include "botmanager.h"


// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>



CBot::CBot(edict_t* pEdict)
	: m_pEdict(pEdict)
{
	Assert(pEdict);
	Assert(botmanager);

	m_pBotController = botmanager->GetBotController(pEdict);
	m_pPlayerInfo = playermanager->GetPlayerInfo(pEdict);
}

void CBot::ChangeTeam(int iTeamNum) {
	m_pPlayerInfo->ChangeTeam(iTeamNum);
}
const char* CBot::GetName() {
	return m_pPlayerInfo->GetName();
}
void CBot::Think()
{
	CBotCmd botCmd;
	V_memset(&botCmd, 0, sizeof(botCmd));
	// make the server run a usercmd for us
	// sourcemod will hook this and allow us to change the usercmd from plugin land
	//m_pBotController->RunPlayerMove(&botCmd);
}
void CBot::PostThink() {}
