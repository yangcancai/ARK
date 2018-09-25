﻿/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "Common/AFProtoCPP.hpp"
#include "Common/AFDataDefine.hpp"
#include "AFCGameServerToWorldModule.h"

AFCGameServerToWorldModule::AFCGameServerToWorldModule(AFIPluginManager* p)
{
    pPluginManager = p;
}

bool AFCGameServerToWorldModule::Init()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pBusModule = pPluginManager->FindModule<AFIBusModule>();
    m_pGameNetServerModule = pPluginManager->FindModule<AFIGameNetServerModule>();
    m_pNetClientManagerModule = pPluginManager->FindModule<AFINetClientManagerModule>();
    m_pMsgModule = pPluginManager->FindModule<AFIMsgModule>();

    m_pKernelModule->AddClassCallBack(ARK::Player::ThisName(), this, &AFCGameServerToWorldModule::OnObjectClassEvent);

    return true;
}

bool AFCGameServerToWorldModule::PostInit()
{
    int ret = StartClient();
    return (ret == 0);
}

int AFCGameServerToWorldModule::StartClient()
{
    //创建所有与对端链接的client
    int ret = m_pNetClientManagerModule->CreateClusterClients();
    if (ret != 0)
    {
        ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
        ARK_ASSERT_NO_EFFECT(0);
        return ret;
    }

    //if need to add a member
    AFINetClientService* pNetClient = m_pNetClientManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_WORLD);
    if (pNetClient == nullptr)
    {
        return -1;
    }

    pNetClient->AddRecvCallback(this, &AFCGameServerToWorldModule::TransPBToProxy);
    pNetClient->AddEventCallBack(this, &AFCGameServerToWorldModule::OnSocketWSEvent);

    return 0;
}

void AFCGameServerToWorldModule::OnSocketWSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID)
{
    if (eEvent == CONNECTED)
    {
        ARK_LOG_INFO("Connected success, id = {}", xClientID.ToString());
        Register(nServerID);
    }
}

int AFCGameServerToWorldModule::OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
{
    if (strClassName == ARK::Player::ThisName())
    {
        if (ARK_ENTITY_EVENT::ENTITY_EVT_DESTROY == eClassEvent)
        {
            SendOffline(self);
        }
        else if (ARK_ENTITY_EVENT::ENTITY_EVT_ALL_FINISHED == eClassEvent)
        {
            SendOnline(self);
        }
    }

    return 0;
}

void AFCGameServerToWorldModule::Register(const int bus_id)
{
    AFINetClientService* pNetClient = m_pNetClientManagerModule->GetNetClientServiceByBusID(bus_id);
    if (pNetClient == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return;
    }

    const AFServerConfig* server_config = m_pBusModule->GetAppServerInfo();
    if (server_config == nullptr)
    {
        ARK_ASSERT_NO_EFFECT(0);
        return;
    }

    AFMsg::ServerInfoReportList xMsg;
    AFMsg::ServerInfoReport* pData = xMsg.add_server_list();

    pData->set_bus_id(server_config->self_id);
    pData->set_cur_online(0);
    pData->set_url(server_config->public_url);
    pData->set_max_online(server_config->max_connection);
    pData->set_logic_status(AFMsg::EST_NARMAL);

    //pNetClient->SendToServerByPB(nSeverID, AFMsg::EGameMsgID::EGMI_GTW_GAME_REGISTERED, xMsg, 0);

    m_pMsgModule->SendParticularSSMsg(bus_id, AFMsg::EGameMsgID::EGMI_GTW_GAME_REGISTERED, xMsg);
    ARK_LOG_INFO("Register self server_id = {}", pData->bus_id());
}

void AFCGameServerToWorldModule::RefreshWorldInfo()
{
    //do nothing for now
}

void AFCGameServerToWorldModule::SendOnline(const AFGUID& self)
{
    AFMsg::RoleOnlineNotify xMsg;

    const AFGUID& xGuild = m_pKernelModule->GetNodeObject(self, "GuildID");
    *xMsg.mutable_guild() = AFIMsgModule::GUIDToPB(xGuild);

    //m_pNetClientModule->SendSuitByPB(xGuild.nLow, AFMsg::EGMI_ACK_ONLINE_NOTIFY, xMsg, self);
    m_pMsgModule->SendSuitSSMsg(ARK_APP_WORLD, xGuild.nLow, AFMsg::EGMI_ACK_ONLINE_NOTIFY, xMsg, self);
}

void AFCGameServerToWorldModule::SendOffline(const AFGUID& self)
{
    AFMsg::RoleOfflineNotify xMsg;

    const AFGUID& xGuild = m_pKernelModule->GetNodeObject(self, "GuildID");
    *xMsg.mutable_guild() = AFIMsgModule::GUIDToPB(xGuild);

    //m_pNetClientModule->SendSuitByPB(xGuild.nLow, AFMsg::EGMI_ACK_OFFLINE_NOTIFY, xMsg, self);
    m_pMsgModule->SendSuitSSMsg(ARK_APP_WORLD, xGuild.nLow, AFMsg::EGMI_ACK_OFFLINE_NOTIFY, xMsg, self);
}

void AFCGameServerToWorldModule::TransPBToProxy(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    ARK_MSG_PROCESS_NO_OBJECT_STRING(xHead, msg, nLen);
    m_pGameNetServerModule->SendMsgPBToGate(nMsgID, strMsg, nPlayerID);
}