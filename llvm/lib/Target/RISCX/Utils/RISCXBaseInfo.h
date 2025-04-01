//===-- RISCXBaseInfo.h - Top level definitions for RISCX MC ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone enum definitions for the RISCX target
// useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_RISCX_MCTARGETDESC_RISCXBASEINFO_H
#define LLVM_LIB_TARGET_RISCX_MCTARGETDESC_RISCXBASEINFO_H

#include "RISCXRegisterInfo.h"
#include "MCTargetDesc/RISCXMCTargetDesc.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/SubtargetFeature.h"

namespace llvm {

// RISCXII - This namespace holds all of the target specific flags that
// instruction info tracks. All definitions must match RISCXInstrFormats.td.
namespace RISCXII {
enum {
  InstFormatPseudo = 0,
  InstFormatR = 1,
  InstFormatR4 = 2,
  InstFormatI = 3,
  InstFormatS = 4,
  InstFormatB = 5,
  InstFormatU = 6,
  InstFormatJ = 7,
  InstFormatCR = 8,
  InstFormatCI = 9,
  InstFormatCSS = 10,
  InstFormatCIW = 11,
  InstFormatCL = 12,
  InstFormatCS = 13,
  InstFormatCA = 14,
  InstFormatCB = 15,
  InstFormatCJ = 16,
  InstFormatOther = 17,

  InstFormatMask = 31
};

// RISC-V Specific Machine Operand Flags
enum {
  MO_None = 0,
  MO_CALL = 1,
  MO_PLT = 2,
  MO_LO = 3,
  MO_HI = 4,
  MO_PCREL_LO = 5,
  MO_PCREL_HI = 6,
  MO_GOT_HI = 7,
  MO_TPREL_LO = 8,
  MO_TPREL_HI = 9,
  MO_TPREL_ADD = 10,
  MO_TLS_GOT_HI = 11,
  MO_TLS_GD_HI = 12,

  // Used to differentiate between target-specific "direct" flags and "bitmask"
  // flags. A machine operand can only have one "direct" flag, but can have
  // multiple "bitmask" flags.
  MO_DIRECT_FLAG_MASK = 15
};
} // namespace RISCXII

namespace RISCXOp {
enum OperandType : unsigned {
  OPERAND_FIRST_RISCX_IMM = MCOI::OPERAND_FIRST_TARGET,
  OPERAND_UIMM4 = OPERAND_FIRST_RISCX_IMM,
  OPERAND_UIMM5,
  OPERAND_UIMM12,
  OPERAND_SIMM12,
  OPERAND_SIMM13_LSB0,
  OPERAND_UIMM20,
  OPERAND_SIMM21_LSB0,
  OPERAND_UIMMLOG2XLEN,
  OPERAND_LAST_RISCX_IMM = OPERAND_UIMMLOG2XLEN
};
} // namespace RISCXOp

// Describes the predecessor/successor bits used in the FENCE instruction.
namespace RISCXFenceField {
enum FenceField {
  I = 8,
  O = 4,
  R = 2,
  W = 1
};
}

// Describes the supported floating point rounding mode encodings.
namespace RISCXFPRndMode {
enum RoundingMode {
  RNE = 0,
  RTZ = 1,
  RDN = 2,
  RUP = 3,
  RMM = 4,
  DYN = 7,
  Invalid
};

inline static StringRef roundingModeToString(RoundingMode RndMode) {
  switch (RndMode) {
  default:
    llvm_unreachable("Unknown floating point rounding mode");
  case RISCXFPRndMode::RNE:
    return "rne";
  case RISCXFPRndMode::RTZ:
    return "rtz";
  case RISCXFPRndMode::RDN:
    return "rdn";
  case RISCXFPRndMode::RUP:
    return "rup";
  case RISCXFPRndMode::RMM:
    return "rmm";
  case RISCXFPRndMode::DYN:
    return "dyn";
  }
}

inline static RoundingMode stringToRoundingMode(StringRef Str) {
  return StringSwitch<RoundingMode>(Str)
      .Case("rne", RISCXFPRndMode::RNE)
      .Case("rtz", RISCXFPRndMode::RTZ)
      .Case("rdn", RISCXFPRndMode::RDN)
      .Case("rup", RISCXFPRndMode::RUP)
      .Case("rmm", RISCXFPRndMode::RMM)
      .Case("dyn", RISCXFPRndMode::DYN)
      .Default(RISCXFPRndMode::Invalid);
}

inline static bool isValidRoundingMode(unsigned Mode) {
  switch (Mode) {
  default:
    return false;
  case RISCXFPRndMode::RNE:
  case RISCXFPRndMode::RTZ:
  case RISCXFPRndMode::RDN:
  case RISCXFPRndMode::RUP:
  case RISCXFPRndMode::RMM:
  case RISCXFPRndMode::DYN:
    return true;
  }
}
} // namespace RISCXFPRndMode

namespace RISCXSysReg {
struct SysReg {
  const char *Name;
  unsigned Encoding;
  // FIXME: add these additional fields when needed.
  // Privilege Access: Read, Write, Read-Only.
  // unsigned ReadWrite;
  // Privilege Mode: User, System or Machine.
  // unsigned Mode;
  // Check field name.
  // unsigned Extra;
  // Register number without the privilege bits.
  // unsigned Number;
  FeatureBitset FeaturesRequired;
  bool isRV32Only;

  bool haveRequiredFeatures(FeatureBitset ActiveFeatures) const {
    // Not in 32-bit mode.
    if (isRV32Only && ActiveFeatures[RISCX::Feature64Bit])
      return false;
    // No required feature associated with the system register.
    if (FeaturesRequired.none())
      return true;
    return (FeaturesRequired & ActiveFeatures) == FeaturesRequired;
  }
};

#define GET_SysRegsList_DECL
#include "RISCXGenSystemOperands.inc"
} // end namespace RISCXSysReg

namespace RISCXABI {

enum ABI {
  ABI_ILP32,
  ABI_ILP32F,
  ABI_ILP32D,
  ABI_ILP32E,
  ABI_LP64,
  ABI_LP64F,
  ABI_LP64D,
  ABI_Unknown
};

// Returns the target ABI, or else a StringError if the requested ABIName is
// not supported for the given TT and FeatureBits combination.
ABI computeTargetABI(const Triple &TT, FeatureBitset FeatureBits,
                     StringRef ABIName);

ABI getTargetABI(StringRef ABIName);

// Returns the register used to hold the stack pointer after realignment.
Register getBPReg();

} // namespace RISCXABI

namespace RISCXFeatures {

// Validates if the given combination of features are valid for the target
// triple. Exits with report_fatal_error if not.
void validate(const Triple &TT, const FeatureBitset &FeatureBits);

} // namespace RISCXFeatures

} // namespace llvm

#endif
