set(LLVM_TARGET_DEFINITIONS RISCX.td)

tablegen(LLVM RISCXGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM RISCXGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM RISCXGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM RISCXGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM RISCXGenGlobalISel.inc -gen-global-isel)
tablegen(LLVM RISCXGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM RISCXGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM RISCXGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM RISCXGenRegisterBank.inc -gen-register-bank)
tablegen(LLVM RISCXGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM RISCXGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM RISCXGenSystemOperands.inc -gen-searchable-tables)

add_public_tablegen_target(RISCXCommonTableGen)

add_llvm_target(RISCXCodeGen
  RISCXAsmPrinter.cpp
  RISCXCallLowering.cpp
  RISCXExpandPseudoInsts.cpp
  RISCXFrameLowering.cpp
  RISCXInstrInfo.cpp
  RISCXInstructionSelector.cpp
  RISCXISelDAGToDAG.cpp
  RISCXISelLowering.cpp
  RISCXLegalizerInfo.cpp
  RISCXMCInstLower.cpp
  RISCXMergeBaseOffset.cpp
  RISCXRegisterBankInfo.cpp
  RISCXRegisterInfo.cpp
  RISCXSubtarget.cpp
  RISCXTargetMachine.cpp
  RISCXTargetObjectFile.cpp
  RISCXTargetTransformInfo.cpp
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
add_subdirectory(Utils)
