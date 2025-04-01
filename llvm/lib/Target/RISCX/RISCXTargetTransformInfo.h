//===- RISCXTargetTransformInfo.h - RISC-V specific TTI ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file defines a TargetTransformInfo::Concept conforming object specific
/// to the RISC-V target machine. It uses the target's detailed information to
/// provide more precise answers to certain TTI queries, while letting the
/// target independent and default TTI implementations handle the rest.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCX_RISCXTARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_RISCX_RISCXTARGETTRANSFORMINFO_H

#include "RISCXSubtarget.h"
#include "RISCXTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/IR/Function.h"

namespace llvm {

class RISCXTTIImpl : public BasicTTIImplBase<RISCXTTIImpl> {
  using BaseT = BasicTTIImplBase<RISCXTTIImpl>;
  using TTI = TargetTransformInfo;

  friend BaseT;

  const RISCXSubtarget *ST;
  const RISCXTargetLowering *TLI;

  const RISCXSubtarget *getST() const { return ST; }
  const RISCXTargetLowering *getTLI() const { return TLI; }

public:
  explicit RISCXTTIImpl(const RISCXTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  int getIntImmCost(const APInt &Imm, Type *Ty);
  int getIntImmCostInst(unsigned Opcode, unsigned Idx, const APInt &Imm, Type *Ty);
  int getIntImmCostIntrin(Intrinsic::ID IID, unsigned Idx, const APInt &Imm,
                          Type *Ty);
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_RISCX_RISCXTARGETTRANSFORMINFO_H