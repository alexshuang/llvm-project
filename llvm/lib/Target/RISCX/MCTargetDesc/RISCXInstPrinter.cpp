//===-- RISCXInstPrinter.cpp - Convert RISCX MCInst to asm syntax ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an RISCX MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "RISCXInstPrinter.h"
#include "MCTargetDesc/RISCXMCExpr.h"
#include "Utils/RISCXBaseInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "RISCXGenAsmWriter.inc"

static cl::opt<bool>
    NoAliases("riscx-no-aliases",
              cl::desc("Disable the emission of assembler pseudo instructions"),
              cl::init(false), cl::Hidden);

static cl::opt<bool>
    ArchRegNames("riscx-arch-reg-names",
                 cl::desc("Print architectural register names rather than the "
                          "ABI names (such as x2 instead of sp)"),
                 cl::init(false), cl::Hidden);

// The command-line flags above are used by llvm-mc and llc. They can be used by
// `llvm-objdump`, but we override their values here to handle options passed to
// `llvm-objdump` with `-M` (which matches GNU objdump). There did not seem to
// be an easier way to allow these options in all these tools, without doing it
// this way.
bool RISCXInstPrinter::applyTargetSpecificCLOption(StringRef Opt) {
  if (Opt == "no-aliases") {
    NoAliases = true;
    return true;
  }
  if (Opt == "numeric") {
    ArchRegNames = true;
    return true;
  }

  return false;
}

void RISCXInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                 StringRef Annot, const MCSubtargetInfo &STI,
                                 raw_ostream &O) {
  bool Res = false;
  const MCInst *NewMI = MI;
  if (NoAliases || !printAliasInstr(NewMI, STI, O))
    printInstruction(NewMI, Address, STI, O);
  printAnnotation(O, Annot);
}

void RISCXInstPrinter::printRegName(raw_ostream &O, unsigned RegNo) const {
  O << getRegisterName(RegNo);
}

void RISCXInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                    const MCSubtargetInfo &STI, raw_ostream &O,
                                    const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &MO = MI->getOperand(OpNo);

  if (MO.isReg()) {
    printRegName(O, MO.getReg());
    return;
  }

  if (MO.isImm()) {
    O << MO.getImm();
    return;
  }

  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  MO.getExpr()->print(O, &MAI);
}

void RISCXInstPrinter::printCSRSystemRegister(const MCInst *MI, unsigned OpNo,
                                              const MCSubtargetInfo &STI,
                                              raw_ostream &O) {
  unsigned Imm = MI->getOperand(OpNo).getImm();
  auto SysReg = RISCXSysReg::lookupSysRegByEncoding(Imm);
  if (SysReg && SysReg->haveRequiredFeatures(STI.getFeatureBits()))
    O << SysReg->Name;
  else
    O << Imm;
}

void RISCXInstPrinter::printFenceArg(const MCInst *MI, unsigned OpNo,
                                     const MCSubtargetInfo &STI,
                                     raw_ostream &O) {
  unsigned FenceArg = MI->getOperand(OpNo).getImm();
  assert (((FenceArg >> 4) == 0) && "Invalid immediate in printFenceArg");

  if ((FenceArg & RISCXFenceField::I) != 0)
    O << 'i';
  if ((FenceArg & RISCXFenceField::O) != 0)
    O << 'o';
  if ((FenceArg & RISCXFenceField::R) != 0)
    O << 'r';
  if ((FenceArg & RISCXFenceField::W) != 0)
    O << 'w';
  if (FenceArg == 0)
    O << "unknown";
}

void RISCXInstPrinter::printFRMArg(const MCInst *MI, unsigned OpNo,
                                   const MCSubtargetInfo &STI, raw_ostream &O) {
  auto FRMArg =
      static_cast<RISCXFPRndMode::RoundingMode>(MI->getOperand(OpNo).getImm());
  O << RISCXFPRndMode::roundingModeToString(FRMArg);
}

void RISCXInstPrinter::printAtomicMemOp(const MCInst *MI, unsigned OpNo,
                                        const MCSubtargetInfo &STI,
                                        raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(OpNo);

  assert(MO.isReg() && "printAtomicMemOp can only print register operands");
  O << "(";
  printRegName(O, MO.getReg());
  O << ")";
  return;
}

const char *RISCXInstPrinter::getRegisterName(unsigned RegNo) {
  return getRegisterName(RegNo, ArchRegNames ? RISCX::NoRegAltName
                                             : RISCX::ABIRegAltName);
}
