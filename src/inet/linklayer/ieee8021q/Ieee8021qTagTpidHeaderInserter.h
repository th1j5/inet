//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __INET_IEEE8021QTAGTPIDHEADERINSERTER_H
#define __INET_IEEE8021QTAGTPIDHEADERINSERTER_H

#include "inet/common/Protocol.h"
#include "inet/protocolelement/base/ProtocolHeaderInserterBase.h"

namespace inet {

class INET_API Ieee8021qTagTpidHeaderInserter : public ProtocolHeaderInserterBase
{
  protected:
    int tpid = -1;
    const Protocol *nextProtocol = nullptr;
    int defaultVlanId = -1;
    int defaultPcp = -1;
    int defaultUserPriority = -1;
    int defaultDropEligible = -1;

  protected:
    virtual void initialize(int stage) override;
    virtual void processPacket(Packet *packet) override;
};

} // namespace inet

#endif

