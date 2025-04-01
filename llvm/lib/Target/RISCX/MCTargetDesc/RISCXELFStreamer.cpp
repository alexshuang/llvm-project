//===-- RISCXELFStreamer.cpp - RISCX ELF Target Streamer Methods ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides RISCX specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "RISCXELFStreamer.h"
#include "MCTargetDesc/RISCXAsmBackend.h"
#include "RISCXMCTargetDesc.h"
#include "Utils/RISCXBaseInfo.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCSubtargetInfo.h"

using namespace llvm;

// This part is for ELF object output.
RISCXTargetELFStreamer::RISCXTargetELFStreamer(MCStreamer &S,
                                               const MCSubtargetInfo &STI)
    : RISCXTargetStreamer(S) {
  MCAssembler &MCA = getStreamer().getAssembler();
  const FeatureBitset &Features = STI.getFeatureBits();
  auto &MAB = static_cast<RISCXAsmBackend &>(MCA.getBackend());
  RISCXABI::ABI ABI = MAB.getTargetABI();
  assert(ABI != RISCXABI::ABI_Unknown && "Improperly initialised target ABI");

  unsigned EFlags = MCA.getELFHeaderEFlags();

  if (Features[RISCX::FeatureStdExtC])
    EFlags |= ELF::EF_RISCX_RVC;

  switch (ABI) {
  case RISCXABI::ABI_ILP32:
  case RISCXABI::ABI_LP64:
    break;
  case RISCXABI::ABI_ILP32F:
  case RISCXABI::ABI_LP64F:
    EFlags |= ELF::EF_RISCX_FLOAT_ABI_SINGLE;
    break;
  case RISCXABI::ABI_ILP32D:
  case RISCXABI::ABI_LP64D:
    EFlags |= ELF::EF_RISCX_FLOAT_ABI_DOUBLE;
    break;
  case RISCXABI::ABI_ILP32E:
    EFlags |= ELF::EF_RISCX_RVE;
    break;
  case RISCXABI::ABI_Unknown:
    llvm_unreachable("Improperly initialised target ABI");
  }

  MCA.setELFHeaderEFlags(EFlags);
}

MCELFStreamer &RISCXTargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}

void RISCXTargetELFStreamer::emitDirectiveOptionPush() {}
void RISCXTargetELFStreamer::emitDirectiveOptionPop() {}
void RISCXTargetELFStreamer::emitDirectiveOptionRVC() {}
void RISCXTargetELFStreamer::emitDirectiveOptionNoRVC() {}
void RISCXTargetELFStreamer::emitDirectiveOptionRelax() {}
void RISCXTargetELFStreamer::emitDirectiveOptionNoRelax() {}
