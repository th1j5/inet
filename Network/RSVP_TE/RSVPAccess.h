//
// (C) 2005 Vojtech Janota
//
// This library is free software, you can redistribute it
// and/or modify
// it under  the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation;
// either version 2 of the License, or any later version.
// The library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//

#ifndef __RSVPACCESS_H__
#define __RSVPACCESS_H__

#include <omnetpp.h>
#include "ModuleAccess.h"
#include "RSVP.h"

/**
 * FIXME missing documentation
 */
class RSVPAccess : public ModuleAccess<RSVP>
{
    public:
        RSVPAccess() : ModuleAccess<RSVP>("rsvp") {}
};

#endif

