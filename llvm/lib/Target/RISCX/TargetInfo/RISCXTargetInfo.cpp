//===-- RISCXTargetInfo.cpp - RISCX Target Implementation -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/RISCXTargetInfo.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheRISCX32Target() {
  static Target TheRISCX32Target;
  return TheRISCX32Target;
}

Target &llvm::getTheRISCX64Target() {
  static Target TheRISCX64Target;
  return TheRISCX64Target;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeRISCXTargetInfo() {
  RegisterTarget<Triple::riscx32> X(getTheRISCX32Target(), "riscx32",
                                    "32-bit RISC-V", "RISCX");
  RegisterTarget<Triple::riscx64> Y(getTheRISCX64Target(), "riscx64",
                                    "64-bit RISC-V", "RISCX");
}
