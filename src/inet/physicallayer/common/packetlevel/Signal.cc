//
// Copyright (C) OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/common/Units.h"
#include "inet/physicallayer/common/packetlevel/Signal.h"

namespace inet {
namespace physicallayer {

using namespace inet::units::values;

Register_Class(Signal);

Signal::Signal(const char *name, short kind, int64_t bitLength) :
    cPacket(name, kind, bitLength)
{
}

Signal::Signal(const Signal& other) :
    cPacket(other)
{
}

std::string Signal::str() const
{
    std::ostringstream out;
    std::string className = getClassName();
    auto index = className.rfind("::");
    if (index != std::string::npos)
        className = className.substr(index + 2);
    out << "\x1b[2m(" << className << ")\x1b[0m\x1b[3m" << getName() << "\x1b[0m (" << simsec(getDuration()) << " " << b(getBitLength()) << ")";
    return out.str();
}

// TODO: getFullName()

} // namespace physicallayer
} // namespace inet

