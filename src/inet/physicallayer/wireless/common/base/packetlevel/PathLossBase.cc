//
// Copyright (C) 2013 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "inet/physicallayer/wireless/common/base/packetlevel/PathLossBase.h"

#include "inet/physicallayer/wireless/common/analogmodel/packetlevel/ScalarTransmissionAnalogModel.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/IRadioMedium.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/IRadioSignal.h"

#include "inet/physicallayer/wireless/common/radio/packetlevel/ScalarTransmitterAnalogModel.h"

namespace inet {

namespace physicallayer {

double PathLossBase::computePathLoss(const ITransmission *transmission, const IArrival *arrival) const
{
    auto radioMedium = transmission->getMedium();
    // TODO: don't require scalar analog model!
    auto analogModel = check_and_cast<const ScalarTransmissionAnalogModel *>(transmission->getNewAnalogModel());
    mps propagationSpeed = radioMedium->getPropagation()->getPropagationSpeed();
    Hz centerFrequency = Hz(analogModel->getCenterFrequency());
    m distance = m(arrival->getStartPosition().distance(transmission->getStartPosition()));
    return computePathLoss(propagationSpeed, centerFrequency, distance);
}

} // namespace physicallayer

} // namespace inet

