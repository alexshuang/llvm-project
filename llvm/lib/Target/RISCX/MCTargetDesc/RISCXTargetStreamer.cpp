//===-- RISCXTargetStreamer.cpp - RISCX Target Streamer Methods -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides RISCX specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "RISCXTargetStreamer.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

RISCXTargetStreamer::RISCXTargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

// This part is for ascii assembly output
RISCXTargetAsmStreamer::RISCXTargetAsmStreamer(MCStreamer &S,
                                               formatted_raw_ostream &OS)
    : RISCXTargetStreamer(S), OS(OS) {}

void RISCXTargetAsmStreamer::emitDirectiveOptionPush() {
  OS << "\t.option\tpush\n";
}

void RISCXTargetAsmStreamer::emitDirectiveOptionPop() {
  OS << "\t.option\tpop\n";
}

void RISCXTargetAsmStreamer::emitDirectiveOptionRVC() {
  OS << "\t.option\trvc\n";
}

void RISCXTargetAsmStreamer::emitDirectiveOptionNoRVC() {
  OS << "\t.option\tnorvc\n";
}

void RISCXTargetAsmStreamer::emitDirectiveOptionRelax() {
  OS << "\t.option\trelax\n";
}

void RISCXTargetAsmStreamer::emitDirectiveOptionNoRelax() {
  OS << "\t.option\tnorelax\n";
}
