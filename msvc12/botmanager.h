#pragma once


#include "extension.h"

#include <eiface.h>
#include <iplayerinfo.h>

#include <utlvector.h>


extern IBotManager* botmanager;
extern IPlayerInfoManager* playermanager;

class CBot;

class CBotManager
	: public IClientListener
{

public:
	void Init();
	void Shutdown();

	CBot* CreateBot(const char* botName);
	bool RemoveBot(int client);
	void RemoveAll();
	CBot* GetBot(const char* botName);
	CBot* GetBot(int client);
	CBot* BotOfEdict(const edict_t* pEdict);

	void Think();
	void PostThink();

private:
	// IClientListener
	void OnClientDisconnecting(int client);

	


private:
	CUtlVector<CBot*> m_Bots;

};

extern CBotManager g_BotManager;
inline CBotManager& GBotManager()
{
	return g_BotManager;
}