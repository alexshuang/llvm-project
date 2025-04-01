//===-- RISCXMCExpr.h - RISCX specific MC expression classes ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes RISCX-specific MCExprs, used for modifiers like
// "%hi" or "%lo" etc.,
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RISCX_MCTARGETDESC_RISCXMCEXPR_H
#define LLVM_LIB_TARGET_RISCX_MCTARGETDESC_RISCXMCEXPR_H

#include "llvm/MC/MCExpr.h"

namespace llvm {

class StringRef;
class MCOperand;
class RISCXMCExpr : public MCTargetExpr {
public:
  enum VariantKind {
    VK_RISCX_None,
    VK_RISCX_LO,
    VK_RISCX_HI,
    VK_RISCX_PCREL_LO,
    VK_RISCX_PCREL_HI,
    VK_RISCX_GOT_HI,
    VK_RISCX_TPREL_LO,
    VK_RISCX_TPREL_HI,
    VK_RISCX_TPREL_ADD,
    VK_RISCX_TLS_GOT_HI,
    VK_RISCX_TLS_GD_HI,
    VK_RISCX_CALL,
    VK_RISCX_CALL_PLT,
    VK_RISCX_32_PCREL,
    VK_RISCX_Invalid
  };

private:
  const MCExpr *Expr;
  const VariantKind Kind;

  int64_t evaluateAsInt64(int64_t Value) const;

  explicit RISCXMCExpr(const MCExpr *Expr, VariantKind Kind)
      : Expr(Expr), Kind(Kind) {}

public:
  static const RISCXMCExpr *create(const MCExpr *Expr, VariantKind Kind,
                                   MCContext &Ctx);

  VariantKind getKind() const { return Kind; }

  const MCExpr *getSubExpr() const { return Expr; }

  /// Get the corresponding PC-relative HI fixup that a VK_RISCX_PCREL_LO
  /// points to, and optionally the fragment containing it.
  ///
  /// \returns nullptr if this isn't a VK_RISCX_PCREL_LO pointing to a
  /// known PC-relative HI fixup.
  const MCFixup *getPCRelHiFixup(const MCFragment **DFOut) const;

  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
  bool evaluateAsRelocatableImpl(MCValue &Res, const MCAsmLayout *Layout,
                                 const MCFixup *Fixup) const override;
  void visitUsedExpr(MCStreamer &Streamer) const override;
  MCFragment *findAssociatedFragment() const override {
    return getSubExpr()->findAssociatedFragment();
  }

  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const override;

  bool evaluateAsConstant(int64_t &Res) const;

  static bool classof(const MCExpr *E) {
    return E->getKind() == MCExpr::Target;
  }

  static bool classof(const RISCXMCExpr *) { return true; }

  static VariantKind getVariantKindForName(StringRef name);
  static StringRef getVariantKindName(VariantKind Kind);
};

} // end namespace llvm.

#endif
