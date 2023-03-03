//
// Copyright (C) 2013 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "inet/physicallayer/wireless/apsk/packetlevel/ApskTransmitter.h"

#include "inet/mobility/contract/IMobility.h"
#include "inet/physicallayer/wireless/apsk/packetlevel/ApskTransmission.h"
#include "inet/physicallayer/wireless/apsk/packetlevel/ApskPhyHeader_m.h"
#include "inet/physicallayer/wireless/common/analogmodel/scalar/ScalarSignalAnalogModel.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/RadioControlInfo_m.h"

namespace inet {

namespace physicallayer {

Define_Module(ApskTransmitter);

std::ostream& ApskTransmitter::printToStream(std::ostream& stream, int level, int evFlags) const
{
    stream << "ApskTransmitter";
    return FlatTransmitterBase::printToStream(stream, level);
}

const ITransmission *ApskTransmitter::createTransmission(const IRadio *transmitter, const Packet *packet, const simtime_t startTime) const
{
    auto phyHeader = packet->peekAtFront<ApskPhyHeader>();
    ASSERT(phyHeader->getChunkLength() == headerLength);
    auto dataLength = packet->getTotalLength() - phyHeader->getChunkLength();
    // TODO move these to analog model
    W transmissionPower = computeTransmissionPower(packet);
    Hz transmissionCenterFrequency = computeCenterFrequency(packet);
    Hz transmissionBandwidth = computeBandwidth(packet);
    bps transmissionBitrate = computeTransmissionDataBitrate(packet);
    const simtime_t headerDuration = b(headerLength).get() / bps(transmissionBitrate).get();
    const simtime_t dataDuration = b(dataLength).get() / bps(transmissionBitrate).get();
    const simtime_t duration = preambleDuration + headerDuration + dataDuration;
    const simtime_t endTime = startTime + duration;
    IMobility *mobility = transmitter->getAntenna()->getMobility();
    const Coord& startPosition = mobility->getCurrentPosition();
    const Coord& endPosition = mobility->getCurrentPosition();
    const Quaternion& startOrientation = mobility->getCurrentAngularPosition();
    const Quaternion& endOrientation = mobility->getCurrentAngularPosition();
    // TODO KLUDGE
    auto symbolTime = 0;
    auto transmission = new ApskTransmission(transmitter, packet, startTime, endTime, preambleDuration, headerDuration, dataDuration, startPosition, endPosition, startOrientation, endOrientation, headerLength, dataLength, modulation, bandwidth, symbolTime, transmissionBitrate, codeRate);
    // TODO constructor parameter
    transmission->analogModel = getAnalogModel()->createAnalogModel(packet, preambleDuration, headerDuration, dataDuration, transmissionCenterFrequency, transmissionBandwidth, transmissionPower);
    return transmission;
}

} // namespace physicallayer

} // namespace inet

