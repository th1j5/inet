//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __INET_PROTOCOLHEADERCHECKERBASE_H
#define __INET_PROTOCOLHEADERCHECKERBASE_H

#include "inet/queueing/base/PacketFilterBase.h"

namespace inet {

class INET_API ProtocolHeaderCheckerBase : public queueing::PacketFilterBase
{
  public:
    virtual bool canPushSomePacket(cGate *gate) const override { return true; }
    virtual bool canPushPacket(Packet *packet, cGate *gate) const override { return true; }
};

} // namespace inet

#endif

