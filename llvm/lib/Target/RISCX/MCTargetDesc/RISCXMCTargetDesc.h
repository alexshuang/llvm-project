//===-- RISCXMCTargetDesc.h - RISCX Target Descriptions ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides RISCX specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCX_MCTARGETDESC_RISCXMCTARGETDESC_H
#define LLVM_LIB_TARGET_RISCX_MCTARGETDESC_RISCXMCTARGETDESC_H

#include "llvm/Config/config.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"
#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
class Target;
class Triple;
class raw_ostream;
class raw_pwrite_stream;

MCCodeEmitter *createRISCXMCCodeEmitter(const MCInstrInfo &MCII,
                                        const MCRegisterInfo &MRI,
                                        MCContext &Ctx);

MCAsmBackend *createRISCXAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                    const MCRegisterInfo &MRI,
                                    const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createRISCXELFObjectWriter(uint8_t OSABI,
                                                                 bool Is64Bit);
}

// Defines symbolic names for RISC-V registers.
#define GET_REGINFO_ENUM
#include "RISCXGenRegisterInfo.inc"

// Defines symbolic names for RISC-V instructions.
#define GET_INSTRINFO_ENUM
#include "RISCXGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "RISCXGenSubtargetInfo.inc"

#endif
