#include "extension.h"

#include "botmanager.h"
#include "bot.h"
#include <string>
// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>


IBotManager* botmanager = NULL;
IPlayerInfoManager* playermanager = NULL;
CBotManager g_BotManager;


void BotManager_GameFrame(bool simulating)
{
	GBotManager().Think();
}

void CBotManager::Init()
{
	g_pSM->AddGameFrameHook(BotManager_GameFrame);

	playerhelpers->AddClientListener(this);
}

void CBotManager::Shutdown()
{
	playerhelpers->RemoveClientListener(this);

	g_pSM->RemoveGameFrameHook(BotManager_GameFrame);
}

CBot* CBotManager::CreateBot(const char* botName)
{
	Assert(botmanager);
	Assert(playermanager);
	edict_t* pEdict = botmanager->CreateBot(botName);

	if (!pEdict)
	{
		// server is likely full
		return NULL;
	}

	CBot* pBot = new CBot(pEdict);
	m_Bots.AddToTail(pBot);
	pBot->ChangeTeam(1);
	return pBot;
}

bool CBotManager::RemoveBot(int client) {
	IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(client);
	Assert(pPlayer);
	if (pPlayer->IsFakeClient())
	{
		pPlayer->Kick("Removed by System");
		return true;
	}
	return false;
}

void CBotManager::RemoveAll() {
	for (int i = m_Bots.Size()-1;i>= 0; i--)
	{
		edict_t* pEdict = m_Bots[i]->GetEdict();
		IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(pEdict);
		Assert(pPlayer);
		if (pPlayer->IsFakeClient()) {
			pPlayer->Kick("Removed by System");
		}
	}
}


CBot* CBotManager::GetBot(const char* botName)
{
	FOR_EACH_VEC(m_Bots, i)
	{
		//Not working also kinda bad
		if (std::string(m_Bots[i]->GetName()) == std::string(botName))
			return m_Bots[i];
	}

	return m_Bots[0];
}

CBot* CBotManager::GetBot(int client)
{
	IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(client);
	Assert(pPlayer);

	if (pPlayer->IsFakeClient())
	{
		edict_t* pEdict = pPlayer->GetEdict();

		if (!pEdict)
			return; // well...

		CBot* pBot = BotOfEdict(pEdict);

		if (pBot)
		{
			return pBot;
		}
	}
}

void CBotManager::Think()
{
	FOR_EACH_VEC(m_Bots, i)
	{
		m_Bots[i]->Think();
	}
}

void CBotManager::PostThink()
{
	FOR_EACH_VEC(m_Bots, i)
	{
		m_Bots[i]->PostThink();
	}
}

void CBotManager::OnClientDisconnecting(int client)
{
	IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(client);
	Assert(pPlayer);
	if (pPlayer->IsFakeClient())
	{
		edict_t* pEdict = pPlayer->GetEdict();

		if (!pEdict)
			return; // well...

		CBot* pBot = BotOfEdict(pEdict);

		if (pBot)
		{
			m_Bots.FindAndRemove(pBot);
			delete pBot;
		}
	}
}



CBot* CBotManager::BotOfEdict(const edict_t* pEdict)
{
	FOR_EACH_VEC(m_Bots, i)
	{
		if (m_Bots[i]->GetEdict() == pEdict)
			return m_Bots[i];
	}

	return NULL;
}