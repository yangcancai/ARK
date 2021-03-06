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

#pragma once

#include "interface/AFIModule.hpp"
#include "net/interface/AFINetServerService.hpp"

namespace ark {

class AFIGameNetModule : public AFIModule
{
public:
    //class GateBaseInfo
    //{
    //public:
    //    GateBaseInfo() {}

    //    GateBaseInfo(const int gateID, const AFGUID xIdent)
    //        : nGateID(gateID)
    //        , xClientID(xIdent)
    //    {
    //    }

    //    int nActorID{0};
    //    int nGateID{0};
    //    AFGUID xClientID{0};
    //};

    //class GateServerInfo
    //{
    //public:
    //    AFServerData xServerData;
    //    //actor id -> gate fd
    //    std::map<AFGUID, AFGUID> xRoleInfo;
    //};

    virtual std::shared_ptr<AFINetServerService> GetNetServerService() = 0;
    virtual void SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self) = 0;
    virtual void SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self) = 0;
    virtual bool AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID) = 0;
    virtual bool RemovePlayerGateInfo(const AFGUID& nRoleID) = 0;
    // virtual std::shared_ptr<GateBaseInfo> GetPlayerGateInfo(const AFGUID& nRoleID) = 0;
};

} // namespace ark
