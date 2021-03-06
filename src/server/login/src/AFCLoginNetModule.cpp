/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "login/include/AFCLoginNetModule.hpp"

namespace ark {

bool AFCLoginNetModule::Init()
{
    m_pLogModule = FindModule<AFILogModule>();
    m_pBusModule = FindModule<AFIBusModule>();
    m_pNetServiceManagerModule = FindModule<AFINetServiceManagerModule>();

    return true;
}

bool AFCLoginNetModule::PostInit()
{
    StartServer();
    return true;
}

int AFCLoginNetModule::StartServer()
{
    auto ret = m_pNetServiceManagerModule->CreateServer();
    ret.Then([=](const std::pair<bool, std::string>& resp) {
        if (!resp.first)
        {
            ARK_LOG_ERROR(
                "Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), resp.second);
            ARK_ASSERT_NO_EFFECT(0);
            exit(0);
        }

        m_pNetServer = m_pNetServiceManagerModule->GetSelfNetServer();
        if (m_pNetServer == nullptr)
        {
            ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->GetSelfBusName());
            exit(0);
        }

        // m_pNetServer->AddNetEventCallBack(this, &AFCLoginNetServerModule::OnSocketClientEvent);
    });

    return 0;
}

//bool AFCLoginNetModule::PreUpdate()
//{
//    int ret = StartClient();
//    return (ret == 0);
//}

//    pSessionData->mnClientID = xClientID;
//    mmClientSessionData.AddElement(xClientID, pSessionData);
//}

//void AFCLoginNetServerModule::OnClientDisconnect(const AFGUID& xClientID)
//{
//    mmClientSessionData.RemoveElement(xClientID);
//}

//void AFCLoginNetServerModule::OnSocketClientEvent(const NetEventType event, const AFGUID& conn_id, const
//std::string& i     p, int bus_id)
//{
//    switch (event)
//    {
//    case CONNECTED:
//        {
//            ARK_LOG_INFO("Connected success, id = {}", conn_id.ToString());
//            OnClientConnected(conn_id);
//        }
//        break;
//    case DISCONNECTED:
//        {
//            ARK_LOG_INFO("Connection closed, id = {}", conn_id.ToString());
//            OnClientDisconnect(conn_id);
//        }
//        break;
//    default:
//        break;
//    }
//}

} // namespace ark
