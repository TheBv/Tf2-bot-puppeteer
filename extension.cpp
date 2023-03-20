/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "./msvc12/bot.h"
#include "./msvc12/botmanager.h"
#include <string>
 /**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

BotPuppeteer g_BotPuppeteer;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_BotPuppeteer);

IForward* g_pCreateBot = NULL;



bool BotPuppeteer::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	GBotManager().Init();

	g_pShareSys->RegisterLibrary(myself, "botpuppeteer");
	extern sp_nativeinfo_t BotPuppeteer_Natives[];
	g_pShareSys->AddNatives(myself, BotPuppeteer_Natives);
	
	//g_pCreateBot = forwards->CreateForward("CreatePupper", ET_Ignore, 0);

	return true;
}
void BotPuppeteer::SDK_OnUnload()
{
	GBotManager().Shutdown();
}
#if defined SMEXT_CONF_METAMOD
bool BotPuppeteer::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetServerFactory, botmanager, IBotManager, INTERFACEVERSION_PLAYERBOTMANAGER);
	GET_V_IFACE_CURRENT(GetServerFactory, playermanager, IPlayerInfoManager, INTERFACEVERSION_PLAYERINFOMANAGER);
	return true;
}

bool BotPuppeteer::SDK_OnMetamodUnload(char* error, size_t maxlength)
{
	return true;
}
#endif
//Command_Bot(char[] name,int tick_count,float forwardmove, float sidemove, float upmove, int weaponselect, int weaponsubtype,int mousedx, int mousedy)
//(int client, int& buttons, int& impulse, float vel[3], float angles[3], int& weapon, int& subtype, int& cmdnum, int& tickcount, int& seed, int mouse[2]){
static cell_t Command_Bot(IPluginContext* pContext, const cell_t* params)
{
	CBot* pBot = GBotManager().GetBot(params[1]);
	if (!pBot)
	{
		return pContext->ThrowNativeError("Unable to find bot with id: ", params[1]);
	}
	CBotCmd botCmd;
	botCmd.Reset();
	cell_t *vel, *angles, *mouse;
	
	botCmd.buttons = params[2];
	botCmd.impulse = params[3];
	pContext->LocalToPhysAddr(params[4], &vel);
	botCmd.forwardmove = sp_ctof(vel[0]);
	botCmd.sidemove = sp_ctof(vel[1]);
	botCmd.upmove = sp_ctof(vel[2]);
	pContext->LocalToPhysAddr(params[5], &angles);
	botCmd.viewangles.x = sp_ctof(angles[0]);
	botCmd.viewangles.y = sp_ctof(angles[1]);
	botCmd.viewangles.z = sp_ctof(angles[2]);
	botCmd.weaponselect = params[6];
	botCmd.weaponsubtype = params[7];
	botCmd.command_number = params[8];
	botCmd.tick_count = params[9];
	botCmd.random_seed = params[10];
	pContext->LocalToPhysAddr(params[11], &mouse);
	botCmd.mousedx = mouse[0];
	botCmd.mousedy = mouse[1];
	pBot->runCommand(&botCmd);
	return 1;
}

static cell_t Create_Bot(IPluginContext* pContext, const cell_t* params)
{
	char* botName = NULL;
	pContext->LocalToString(params[1], &botName);

	CBot* pBot = GBotManager().CreateBot(botName);

	if (!pBot)
	{
		return pContext->ThrowNativeError("Unable to create bot");
	}

	return pBot->GetIndex();
}

static cell_t Remove_Bot(IPluginContext* pContext, const cell_t* params)
{
	bool success = GBotManager().RemoveBot(params[1]);
	return success;
}

static cell_t Remove_All(IPluginContext* pContext, const cell_t* params)
{
	GBotManager().RemoveAll();
	return 1;
}

static cell_t BotChangeTeam(IPluginContext* pContext, const cell_t* params)
{
	char* botName = NULL;
	pContext->LocalToString(params[1], &botName);
	CBot* pBot = GBotManager().GetBot(botName);
	if (!pBot)
	{
		return pContext->ThrowNativeError("Unable to find bot with name: ", std::string(botName));
	}
	pBot->ChangeTeam(params[2]);
	return pBot->GetIndex();
	
}

sp_nativeinfo_t BotPuppeteer_Natives[] =
{
	{"BotPuppeteer_CreateBot",Create_Bot},
	{"BotPuppeteer_RemoveBot",Remove_Bot},
	{"BotPuppeteer_RemoveAll",Remove_All},
	//{"BotPuppeteer_BotChangeTeam",BotChangeTeam},
	{"BotPuppeteer_CommandBot",Command_Bot},
	{ NULL, NULL}
};
