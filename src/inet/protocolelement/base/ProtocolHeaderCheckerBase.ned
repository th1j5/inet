//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package inet.protocolelement.base;

import inet.queueing.base.PacketFilterBase;

//
// This is a base module for various packet header checker modules.
//
simple ProtocolHeaderCheckerBase extends PacketFilterBase
{
    parameters:
        @class(ProtocolHeaderCheckerBase);
}
