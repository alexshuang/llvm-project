//===-- RISCXRegisterInfo.cpp - RISCX Register Information ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the RISCX implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "RISCXRegisterInfo.h"
#include "RISCX.h"
#include "RISCXSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "RISCXGenRegisterInfo.inc"

using namespace llvm;

static_assert(RISCX::X1 == RISCX::X0 + 1, "Register list not consecutive");
static_assert(RISCX::X31 == RISCX::X0 + 31, "Register list not consecutive");
static_assert(RISCX::F1_F == RISCX::F0_F + 1, "Register list not consecutive");
static_assert(RISCX::F31_F == RISCX::F0_F + 31,
              "Register list not consecutive");
static_assert(RISCX::F1_D == RISCX::F0_D + 1, "Register list not consecutive");
static_assert(RISCX::F31_D == RISCX::F0_D + 31,
              "Register list not consecutive");

RISCXRegisterInfo::RISCXRegisterInfo(unsigned HwMode)
    : RISCXGenRegisterInfo(RISCX::X1, /*DwarfFlavour*/0, /*EHFlavor*/0,
                           /*PC*/0, HwMode) {}

const MCPhysReg *
RISCXRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  auto &Subtarget = MF->getSubtarget<RISCXSubtarget>();
  if (MF->getFunction().hasFnAttribute("interrupt")) {
    if (Subtarget.hasStdExtD())
      return CSR_XLEN_F64_Interrupt_SaveList;
    if (Subtarget.hasStdExtF())
      return CSR_XLEN_F32_Interrupt_SaveList;
    return CSR_Interrupt_SaveList;
  }

  switch (Subtarget.getTargetABI()) {
  default:
    llvm_unreachable("Unrecognized ABI");
  case RISCXABI::ABI_ILP32:
  case RISCXABI::ABI_LP64:
    return CSR_ILP32_LP64_SaveList;
  case RISCXABI::ABI_ILP32F:
  case RISCXABI::ABI_LP64F:
    return CSR_ILP32F_LP64F_SaveList;
  case RISCXABI::ABI_ILP32D:
  case RISCXABI::ABI_LP64D:
    return CSR_ILP32D_LP64D_SaveList;
  }
}

BitVector RISCXRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  const RISCXFrameLowering *TFI = getFrameLowering(MF);
  BitVector Reserved(getNumRegs());

  // Mark any registers requested to be reserved as such
  for (size_t Reg = 0; Reg < getNumRegs(); Reg++) {
    if (MF.getSubtarget<RISCXSubtarget>().isRegisterReservedByUser(Reg))
      markSuperRegs(Reserved, Reg);
  }

  // Use markSuperRegs to ensure any register aliases are also reserved
  markSuperRegs(Reserved, RISCX::X0); // zero
  markSuperRegs(Reserved, RISCX::X2); // sp
  markSuperRegs(Reserved, RISCX::X3); // gp
  markSuperRegs(Reserved, RISCX::X4); // tp
  if (TFI->hasFP(MF))
    markSuperRegs(Reserved, RISCX::X8); // fp
  // Reserve the base register if we need to realign the stack and allocate
  // variable-sized objects at runtime.
  if (TFI->hasBP(MF))
    markSuperRegs(Reserved, RISCXABI::getBPReg()); // bp
  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

bool RISCXRegisterInfo::isAsmClobberable(const MachineFunction &MF,
                                         unsigned PhysReg) const {
  return !MF.getSubtarget<RISCXSubtarget>().isRegisterReservedByUser(PhysReg);
}

bool RISCXRegisterInfo::isConstantPhysReg(unsigned PhysReg) const {
  return PhysReg == RISCX::X0;
}

const uint32_t *RISCXRegisterInfo::getNoPreservedMask() const {
  return CSR_NoRegs_RegMask;
}

void RISCXRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  const RISCXInstrInfo *TII = MF.getSubtarget<RISCXSubtarget>().getInstrInfo();
  DebugLoc DL = MI.getDebugLoc();

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  unsigned FrameReg;
  int Offset =
      getFrameLowering(MF)->getFrameIndexReference(MF, FrameIndex, FrameReg) +
      MI.getOperand(FIOperandNum + 1).getImm();

  if (!isInt<32>(Offset)) {
    report_fatal_error(
        "Frame offsets outside of the signed 32-bit range not supported");
  }

  MachineBasicBlock &MBB = *MI.getParent();
  bool FrameRegIsKill = false;

  if (!isInt<12>(Offset)) {
    assert(isInt<32>(Offset) && "Int32 expected");
    // The offset won't fit in an immediate, so use a scratch register instead
    // Modify Offset and FrameReg appropriately
    Register ScratchReg = MRI.createVirtualRegister(&RISCX::GPRRegClass);
    TII->movImm(MBB, II, DL, ScratchReg, Offset);
    BuildMI(MBB, II, DL, TII->get(RISCX::ADD), ScratchReg)
        .addReg(FrameReg)
        .addReg(ScratchReg, RegState::Kill);
    Offset = 0;
    FrameReg = ScratchReg;
    FrameRegIsKill = true;
  }

  MI.getOperand(FIOperandNum)
      .ChangeToRegister(FrameReg, false, false, FrameRegIsKill);
  MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
}

Register RISCXRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = getFrameLowering(MF);
  return TFI->hasFP(MF) ? RISCX::X8 : RISCX::X2;
}

const uint32_t *
RISCXRegisterInfo::getCallPreservedMask(const MachineFunction & MF,
                                        CallingConv::ID /*CC*/) const {
  auto &Subtarget = MF.getSubtarget<RISCXSubtarget>();

  switch (Subtarget.getTargetABI()) {
  default:
    llvm_unreachable("Unrecognized ABI");
  case RISCXABI::ABI_ILP32:
  case RISCXABI::ABI_LP64:
    return CSR_ILP32_LP64_RegMask;
  case RISCXABI::ABI_ILP32F:
  case RISCXABI::ABI_LP64F:
    return CSR_ILP32F_LP64F_RegMask;
  case RISCXABI::ABI_ILP32D:
  case RISCXABI::ABI_LP64D:
    return CSR_ILP32D_LP64D_RegMask;
  }
}
