//===-- RISCXMCTargetDesc.cpp - RISCX Target Descriptions -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// This file provides RISCX-specific target descriptions.
///
//===----------------------------------------------------------------------===//

#include "RISCXMCTargetDesc.h"
#include "RISCXELFStreamer.h"
#include "RISCXInstPrinter.h"
#include "RISCXMCAsmInfo.h"
#include "RISCXTargetStreamer.h"
#include "TargetInfo/RISCXTargetInfo.h"
#include "Utils/RISCXBaseInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "RISCXGenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "RISCXGenRegisterInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "RISCXGenSubtargetInfo.inc"

using namespace llvm;

static MCInstrInfo *createRISCXMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitRISCXMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createRISCXMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitRISCXMCRegisterInfo(X, RISCX::X1);
  return X;
}

static MCAsmInfo *createRISCXMCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new RISCXMCAsmInfo(TT);

  Register SP = MRI.getDwarfRegNum(RISCX::X2, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(nullptr, SP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCSubtargetInfo *createRISCXMCSubtargetInfo(const Triple &TT,
                                                   StringRef CPU, StringRef FS) {
  std::string CPUName = CPU;
  if (CPUName.empty())
    CPUName = TT.isArch64Bit() ? "generic-rv64" : "generic-rv32";
  return createRISCXMCSubtargetInfoImpl(TT, CPUName, FS);
}

static MCInstPrinter *createRISCXMCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new RISCXInstPrinter(MAI, MII, MRI);
}

static MCTargetStreamer *
createRISCXObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  const Triple &TT = STI.getTargetTriple();
  if (TT.isOSBinFormatELF())
    return new RISCXTargetELFStreamer(S, STI);
  return nullptr;
}

static MCTargetStreamer *createRISCXAsmTargetStreamer(MCStreamer &S,
                                                      formatted_raw_ostream &OS,
                                                      MCInstPrinter *InstPrint,
                                                      bool isVerboseAsm) {
  return new RISCXTargetAsmStreamer(S, OS);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeRISCXTargetMC() {
  for (Target *T : {&getTheRISCX32Target(), &getTheRISCX64Target()}) {
    TargetRegistry::RegisterMCAsmInfo(*T, createRISCXMCAsmInfo);
    TargetRegistry::RegisterMCInstrInfo(*T, createRISCXMCInstrInfo);
    TargetRegistry::RegisterMCRegInfo(*T, createRISCXMCRegisterInfo);
    TargetRegistry::RegisterMCAsmBackend(*T, createRISCXAsmBackend);
    TargetRegistry::RegisterMCCodeEmitter(*T, createRISCXMCCodeEmitter);
    TargetRegistry::RegisterMCInstPrinter(*T, createRISCXMCInstPrinter);
    TargetRegistry::RegisterMCSubtargetInfo(*T, createRISCXMCSubtargetInfo);
    TargetRegistry::RegisterObjectTargetStreamer(
        *T, createRISCXObjectTargetStreamer);

    // Register the asm target streamer.
    TargetRegistry::RegisterAsmTargetStreamer(*T, createRISCXAsmTargetStreamer);
  }
}
