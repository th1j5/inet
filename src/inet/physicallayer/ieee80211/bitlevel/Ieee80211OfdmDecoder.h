//
// Copyright (C) 2014 OpenSim Ltd.
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

#ifndef __INET_IEEE80211OFDMDECODER_H
#define __INET_IEEE80211OFDMDECODER_H

#include "inet/physicallayer/base/packetlevel/ApskModulationBase.h"
#include "inet/physicallayer/common/bitlevel/AdditiveScrambler.h"
#include "inet/physicallayer/common/bitlevel/ConvolutionalCoder.h"
#include "inet/physicallayer/contract/bitlevel/IDecoder.h"
#include "inet/physicallayer/contract/bitlevel/ISignalBitModel.h"
#include "inet/physicallayer/contract/bitlevel/ISignalPacketModel.h"
#include "inet/physicallayer/ieee80211/bitlevel/Ieee80211ConvolutionalCode.h"
#include "inet/physicallayer/ieee80211/bitlevel/Ieee80211OfdmInterleaver.h"
#include "inet/physicallayer/ieee80211/bitlevel/Ieee80211OfdmInterleaving.h"
#include "inet/physicallayer/ieee80211/mode/Ieee80211OfdmCode.h"

namespace inet {
namespace physicallayer {

class INET_API Ieee80211OfdmDecoder : public IDecoder
{
  protected:
    const Ieee80211OfdmCode *code = nullptr;
    const IScrambler *descrambler = nullptr;
    const IFecCoder *fecDecoder = nullptr;
    const IInterleaver *deinterleaver = nullptr;

  protected:
    const IReceptionPacketModel *createPacketModel(const BitVector *decodedBits, bool hasBitError, const IScrambling *scrambling, const IForwardErrorCorrection *fec, const IInterleaving *interleaving) const;
    ShortBitVector getSignalFieldRate(const BitVector& signalField) const;
    unsigned int getSignalFieldLength(const BitVector& signalField) const;
    unsigned int calculatePadding(unsigned int dataFieldLengthInBits, const IModulation *modulationScheme, const Ieee80211ConvolutionalCode *fec) const;

  public:
    Ieee80211OfdmDecoder(const IScrambler *descrambler, const IFecCoder *fecDecoder, const IInterleaver *deinterleaver);
    Ieee80211OfdmDecoder(const Ieee80211OfdmCode *code);
    virtual ~Ieee80211OfdmDecoder();

    virtual std::ostream& printToStream(std::ostream& stream, int level, int evFlags = 0) const override;
    const IReceptionPacketModel *decode(const IReceptionBitModel *bitModel) const override;
    const Ieee80211OfdmCode *getCode() const { return code; }
};

} /* namespace physicallayer */
} /* namespace inet */

#endif

