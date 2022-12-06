//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "inet/queueing/gate/PeriodicGate.h"

#include "inet/common/ModuleAccess.h"

namespace inet {
namespace queueing {

Define_Module(PeriodicGate);

void PeriodicGate::initialize(int stage)
{
    ClockUserModuleMixin::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        isOpen_ = par("initiallyOpen");
        initialOffset = par("offset");
        scheduleForAbsoluteTime = par("scheduleForAbsoluteTime");
        changeTimer = new ClockEvent("ChangeTimer");
        openSchedulingPriority = par("openSchedulingPriority");
        closeSchedulingPriority = par("closeSchedulingPriority");
        initializeGating();
    }
}

void PeriodicGate::handleParameterChange(const char *name)
{
    if (name != nullptr) {
        if (!strcmp(name, "offset")) {
            initialOffset = par("offset");
            initializeGating();
        }
        else if (!strcmp(name, "initiallyOpen")) {
            isOpen_ = par("initiallyOpen");
            initializeGating();
        }
        else if (!strcmp(name, "durations")) {
            initializeGating();
        }
    }
}

void PeriodicGate::handleMessage(cMessage *message)
{
    if (message == changeTimer) {
        scheduleChangeTimer(isOpen_);
        processChangeTimer();
    }
    else
        throw cRuntimeError("Unknown message");
}

void PeriodicGate::initializeGating()
{
    auto durationsArray = check_and_cast<cValueArray *>(par("durations").objectValue());
    size_t size = durationsArray->size();
    if (size % 2 != 0)
        throw cRuntimeError("The duration parameter must contain an even number of values");
    durations.resize(size);
    for (size_t i=0; i<size; i++)
        durations[i] = durationsArray->get(i).doubleValueInUnit("s");

    index = 0;
    offset = initialOffset = par("offset");
    if (size > 0) {
        clocktime_t lastDuration = durations[size-1];
        if (lastDuration == CLOCKTIME_ZERO) {
            if (size > 2) {
                clocktime_t d2 = durations[size - 2];
                durations[0] += d2;
                initialOffset += d2;
                offset += d2;
            }
            size -= 2;
            durations.resize(size);
        }
        if (size > 0) {
            while (offset > 0) {
                if (offset > durations[index]) {
                    isOpen_ = !isOpen_;
                    offset -= durations[index];
                    index = (index + 1) % size;
                }
                else
                    break;
            }
        }
        else
            offset = CLOCKTIME_ZERO;
    }
    if (changeTimer->isScheduled())
        cancelClockEvent(changeTimer);
    if (size > 0)
        scheduleChangeTimer(!isOpen_);
}

void PeriodicGate::scheduleChangeTimer(bool toOpen)
{
    ASSERT(0 <= index && index < (int)durations.size());
    clocktime_t duration = durations[index];
    if (duration == CLOCKTIME_ZERO)
        throw cRuntimeError("The duration parameter contains zero value at position %d", index);
    index = (index + 1) % durations.size();
    //std::cout << getFullPath() << " " << duration << std::endl;
    changeTimer->setSchedulingPriority(toOpen ? closeSchedulingPriority : openSchedulingPriority);
    if (scheduleForAbsoluteTime)
        scheduleClockEventAt(getClockTime() + duration - offset, changeTimer);
    else
        scheduleClockEventAfter(duration - offset, changeTimer);
    offset = 0;
}

void PeriodicGate::processChangeTimer()
{
    if (isOpen_)
        close();
    else
        open();
}

bool PeriodicGate::canPacketFlowThrough(Packet *packet) const
{
    if (!isOpen_)
        return false;
    else if (std::isnan(bitrate.get()))
        return PacketGateBase::canPacketFlowThrough(packet);
    else if (packet == nullptr)
        return false;
    else {
        clocktime_t flowEndTime = getClockTime() + s((packet->getDataLength() + extraLength) / bitrate).get() + SIMTIME_AS_CLOCKTIME(extraDuration);
        return !changeTimer->isScheduled() || flowEndTime <= getArrivalClockTime(changeTimer);
    }
}

} // namespace queueing
} // namespace inet

