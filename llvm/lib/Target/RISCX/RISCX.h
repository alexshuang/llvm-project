//===-- RISCX.h - Top-level interface for RISCX -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// RISC-V back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCX_RISCX_H
#define LLVM_LIB_TARGET_RISCX_RISCX_H

#include "Utils/RISCXBaseInfo.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class RISCXRegisterBankInfo;
class RISCXSubtarget;
class RISCXTargetMachine;
class AsmPrinter;
class FunctionPass;
class InstructionSelector;
class MCInst;
class MCOperand;
class MachineInstr;
class MachineOperand;
class PassRegistry;

void LowerRISCXMachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                    const AsmPrinter &AP);
bool LowerRISCXMachineOperandToMCOperand(const MachineOperand &MO,
                                         MCOperand &MCOp, const AsmPrinter &AP);

FunctionPass *createRISCXISelDag(RISCXTargetMachine &TM);

FunctionPass *createRISCXMergeBaseOffsetOptPass();
void initializeRISCXMergeBaseOffsetOptPass(PassRegistry &);

FunctionPass *createRISCXExpandPseudoPass();
void initializeRISCXExpandPseudoPass(PassRegistry &);

InstructionSelector *createRISCXInstructionSelector(const RISCXTargetMachine &,
                                                    RISCXSubtarget &,
                                                    RISCXRegisterBankInfo &);
}

#endif
