//===-- RISCXSubtarget.cpp - RISCX Subtarget Information ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the RISCX specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "RISCXSubtarget.h"
#include "RISCX.h"
#include "RISCXCallLowering.h"
#include "RISCXFrameLowering.h"
#include "RISCXLegalizerInfo.h"
#include "RISCXRegisterBankInfo.h"
#include "RISCXTargetMachine.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "riscx-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "RISCXGenSubtargetInfo.inc"

void RISCXSubtarget::anchor() {}

RISCXSubtarget &RISCXSubtarget::initializeSubtargetDependencies(
    const Triple &TT, StringRef CPU, StringRef FS, StringRef ABIName) {
  // Determine default and user-specified characteristics
  bool Is64Bit = TT.isArch64Bit();
  std::string CPUName = CPU;
  if (CPUName.empty())
    CPUName = Is64Bit ? "generic-rv64" : "generic-rv32";
  ParseSubtargetFeatures(CPUName, FS);
  if (Is64Bit) {
    XLenVT = MVT::i64;
    XLen = 64;
  }

  TargetABI = RISCXABI::computeTargetABI(TT, getFeatureBits(), ABIName);
  RISCXFeatures::validate(TT, getFeatureBits());
  return *this;
}

RISCXSubtarget::RISCXSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                               StringRef ABIName, const TargetMachine &TM)
    : RISCXGenSubtargetInfo(TT, CPU, FS),
      UserReservedRegister(RISCX::NUM_TARGET_REGS),
      FrameLowering(initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrInfo(*this), RegInfo(getHwMode()), TLInfo(TM, *this) {
  CallLoweringInfo.reset(new RISCXCallLowering(*getTargetLowering()));
  Legalizer.reset(new RISCXLegalizerInfo(*this));

  auto *RBI = new RISCXRegisterBankInfo(*getRegisterInfo());
  RegBankInfo.reset(RBI);
  InstSelector.reset(createRISCXInstructionSelector(
      *static_cast<const RISCXTargetMachine *>(&TM), *this, *RBI));
}

const CallLowering *RISCXSubtarget::getCallLowering() const {
  return CallLoweringInfo.get();
}

InstructionSelector *RISCXSubtarget::getInstructionSelector() const {
  return InstSelector.get();
}

const LegalizerInfo *RISCXSubtarget::getLegalizerInfo() const {
  return Legalizer.get();
}

const RegisterBankInfo *RISCXSubtarget::getRegBankInfo() const {
  return RegBankInfo.get();
}
