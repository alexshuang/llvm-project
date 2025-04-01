//===-- RISCXELFObjectWriter.cpp - RISCX ELF Writer -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/RISCXFixupKinds.h"
#include "MCTargetDesc/RISCXMCExpr.h"
#include "MCTargetDesc/RISCXMCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class RISCXELFObjectWriter : public MCELFObjectTargetWriter {
public:
  RISCXELFObjectWriter(uint8_t OSABI, bool Is64Bit);

  ~RISCXELFObjectWriter() override;

  // Return true if the given relocation must be with a symbol rather than
  // section plus offset.
  bool needsRelocateWithSymbol(const MCSymbol &Sym,
                               unsigned Type) const override {
    // TODO: this is very conservative, update once RISC-V psABI requirements
    //       are clarified.
    return true;
  }

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
}

RISCXELFObjectWriter::RISCXELFObjectWriter(uint8_t OSABI, bool Is64Bit)
    : MCELFObjectTargetWriter(Is64Bit, OSABI, ELF::EM_RISCX,
                              /*HasRelocationAddend*/ true) {}

RISCXELFObjectWriter::~RISCXELFObjectWriter() {}

unsigned RISCXELFObjectWriter::getRelocType(MCContext &Ctx,
                                            const MCValue &Target,
                                            const MCFixup &Fixup,
                                            bool IsPCRel) const {
  const MCExpr *Expr = Fixup.getValue();
  // Determine the type of the relocation
  unsigned Kind = Fixup.getTargetKind();
  if (IsPCRel) {
    switch (Kind) {
    default:
      Ctx.reportError(Fixup.getLoc(), "Unsupported relocation type");
      return ELF::R_RISCX_NONE;
    case FK_Data_4:
    case FK_PCRel_4:
      return ELF::R_RISCX_32_PCREL;
    case RISCX::fixup_riscx_pcrel_hi20:
      return ELF::R_RISCX_PCREL_HI20;
    case RISCX::fixup_riscx_pcrel_lo12_i:
      return ELF::R_RISCX_PCREL_LO12_I;
    case RISCX::fixup_riscx_pcrel_lo12_s:
      return ELF::R_RISCX_PCREL_LO12_S;
    case RISCX::fixup_riscx_got_hi20:
      return ELF::R_RISCX_GOT_HI20;
    case RISCX::fixup_riscx_tls_got_hi20:
      return ELF::R_RISCX_TLS_GOT_HI20;
    case RISCX::fixup_riscx_tls_gd_hi20:
      return ELF::R_RISCX_TLS_GD_HI20;
    case RISCX::fixup_riscx_jal:
      return ELF::R_RISCX_JAL;
    case RISCX::fixup_riscx_branch:
      return ELF::R_RISCX_BRANCH;
    case RISCX::fixup_riscx_rvc_jump:
      return ELF::R_RISCX_RVC_JUMP;
    case RISCX::fixup_riscx_rvc_branch:
      return ELF::R_RISCX_RVC_BRANCH;
    case RISCX::fixup_riscx_call:
      return ELF::R_RISCX_CALL;
    case RISCX::fixup_riscx_call_plt:
      return ELF::R_RISCX_CALL_PLT;
    }
  }

  switch (Kind) {
  default:
    Ctx.reportError(Fixup.getLoc(), "Unsupported relocation type");
    return ELF::R_RISCX_NONE;
  case FK_Data_1:
    Ctx.reportError(Fixup.getLoc(), "1-byte data relocations not supported");
    return ELF::R_RISCX_NONE;
  case FK_Data_2:
    Ctx.reportError(Fixup.getLoc(), "2-byte data relocations not supported");
    return ELF::R_RISCX_NONE;
  case FK_Data_4:
    if (Expr->getKind() == MCExpr::Target &&
        cast<RISCXMCExpr>(Expr)->getKind() == RISCXMCExpr::VK_RISCX_32_PCREL)
      return ELF::R_RISCX_32_PCREL;
    return ELF::R_RISCX_32;
  case FK_Data_8:
    return ELF::R_RISCX_64;
  case FK_Data_Add_1:
    return ELF::R_RISCX_ADD8;
  case FK_Data_Add_2:
    return ELF::R_RISCX_ADD16;
  case FK_Data_Add_4:
    return ELF::R_RISCX_ADD32;
  case FK_Data_Add_8:
    return ELF::R_RISCX_ADD64;
  case FK_Data_Add_6b:
    return ELF::R_RISCX_SET6;
  case FK_Data_Sub_1:
    return ELF::R_RISCX_SUB8;
  case FK_Data_Sub_2:
    return ELF::R_RISCX_SUB16;
  case FK_Data_Sub_4:
    return ELF::R_RISCX_SUB32;
  case FK_Data_Sub_8:
    return ELF::R_RISCX_SUB64;
  case FK_Data_Sub_6b:
    return ELF::R_RISCX_SUB6;
  case RISCX::fixup_riscx_hi20:
    return ELF::R_RISCX_HI20;
  case RISCX::fixup_riscx_lo12_i:
    return ELF::R_RISCX_LO12_I;
  case RISCX::fixup_riscx_lo12_s:
    return ELF::R_RISCX_LO12_S;
  case RISCX::fixup_riscx_tprel_hi20:
    return ELF::R_RISCX_TPREL_HI20;
  case RISCX::fixup_riscx_tprel_lo12_i:
    return ELF::R_RISCX_TPREL_LO12_I;
  case RISCX::fixup_riscx_tprel_lo12_s:
    return ELF::R_RISCX_TPREL_LO12_S;
  case RISCX::fixup_riscx_tprel_add:
    return ELF::R_RISCX_TPREL_ADD;
  case RISCX::fixup_riscx_relax:
    return ELF::R_RISCX_RELAX;
  case RISCX::fixup_riscx_align:
    return ELF::R_RISCX_ALIGN;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createRISCXELFObjectWriter(uint8_t OSABI, bool Is64Bit) {
  return std::make_unique<RISCXELFObjectWriter>(OSABI, Is64Bit);
}
