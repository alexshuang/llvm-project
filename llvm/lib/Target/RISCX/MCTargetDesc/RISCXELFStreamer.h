//===-- RISCXELFStreamer.h - RISCX ELF Target Streamer ---------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCX_RISCXELFSTREAMER_H
#define LLVM_LIB_TARGET_RISCX_RISCXELFSTREAMER_H

#include "RISCXTargetStreamer.h"
#include "llvm/MC/MCELFStreamer.h"

namespace llvm {

class RISCXTargetELFStreamer : public RISCXTargetStreamer {
public:
  MCELFStreamer &getStreamer();
  RISCXTargetELFStreamer(MCStreamer &S, const MCSubtargetInfo &STI);

  virtual void emitDirectiveOptionPush();
  virtual void emitDirectiveOptionPop();
  virtual void emitDirectiveOptionRVC();
  virtual void emitDirectiveOptionNoRVC();
  virtual void emitDirectiveOptionRelax();
  virtual void emitDirectiveOptionNoRelax();
};
}
#endif
