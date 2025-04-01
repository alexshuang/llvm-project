//===-- RISCXMCAsmInfo.h - RISCX Asm Info ----------------------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the RISCXMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCX_MCTARGETDESC_RISCXMCASMINFO_H
#define LLVM_LIB_TARGET_RISCX_MCTARGETDESC_RISCXMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class RISCXMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit RISCXMCAsmInfo(const Triple &TargetTriple);

  const MCExpr *getExprForFDESymbol(const MCSymbol *Sym, unsigned Encoding,
                                    MCStreamer &Streamer) const override;
};

} // namespace llvm

#endif
