// $Header$
//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/*  ---------------------------------------------------------
    file: RoutingTableAccess.cc
    Purpose: Implementation of IPModule base class;
    gives access to the RoutingTable
    comment: RoutingTable-Module needs to be named "routingTable"
    --------------------------------------------------------- */


#include "RoutingTableAccess.h"

Define_Module( RoutingTableAccess );

void RoutingTableAccess::initialize()
{
	cObject *foundmod;
	cModule *curmod = this;

	// ProcessorAccess::initialize();

	// find Routing Table
	rt = NULL;
	for (curmod = parentModule(); curmod != NULL;
			curmod = curmod->parentModule())
	{
		if ((foundmod = curmod->findObject("routingTable", false)) != NULL)
		{
			rt = (RoutingTable *)foundmod;
			break;
		}
	}

}

