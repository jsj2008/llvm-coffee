//===-- CoffeeCLInstPrinter.cpp - Convert CoffeeCL MCInst to assembly syntax ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an CoffeeCL MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "CoffeeCLInstPrinter.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#include "CoffeeCLGenAsmWriter.inc"

const char* CoffeeCL::CoffeeCLFCCToString(CoffeeCL::CondCode CC) {
  switch (CC) {
  case FCOND_F:
  case FCOND_T:   return "f";
  case FCOND_UN:
  case FCOND_OR:  return "un";
  case FCOND_OEQ:
  case FCOND_UNE: return "eq";
  case FCOND_UEQ:
  case FCOND_ONE: return "ueq";
  case FCOND_OLT:
  case FCOND_UGE: return "olt";
  case FCOND_ULT:
  case FCOND_OGE: return "ult";
  case FCOND_OLE:
  case FCOND_UGT: return "ole";
  case FCOND_ULE:
  case FCOND_OGT: return "ule";
  case FCOND_SF:
  case FCOND_ST:  return "sf";
  case FCOND_NGLE:
  case FCOND_GLE: return "ngle";
  case FCOND_SEQ:
  case FCOND_SNE: return "seq";
  case FCOND_NGL:
  case FCOND_GL:  return "ngl";
  case FCOND_LT:
  case FCOND_NLT: return "lt";
  case FCOND_NGE:
  case FCOND_GE:  return "nge";
  case FCOND_LE:
  case FCOND_NLE: return "le";
  case FCOND_NGT:
  case FCOND_GT:  return "ngt";
  }
  llvm_unreachable("Impossible condition code!");
}

void CoffeeCLInstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const {
  OS << '$' << StringRef(getRegisterName(RegNo)).lower();
}

void CoffeeCLInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                StringRef Annot) {
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

static void printExpr(const MCExpr *Expr, raw_ostream &OS) {
  int Offset = 0;
  const MCSymbolRefExpr *SRE;

  if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
    SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());
    assert(SRE && CE && "Binary expression must be sym+const.");
    Offset = CE->getValue();
  }
  else if (!(SRE = dyn_cast<MCSymbolRefExpr>(Expr)))
    assert(false && "Unexpected MCExpr type.");

  MCSymbolRefExpr::VariantKind Kind = SRE->getKind();

  switch (Kind) {
  default:                                 llvm_unreachable("Invalid kind!");
  case MCSymbolRefExpr::VK_None:           break;

  //case MCSymbolRefExpr::VK_CoffeeCL_ABS_HI:    OS << "%hi(";     break;
  //case MCSymbolRefExpr::VK_CoffeeCL_ABS_LO:    OS << "%lo(";     break;

  }

  OS << SRE->getSymbol();

  if (Offset) {
    if (Offset > 0)
      OS << '+';
    OS << Offset;
  }

  if (Kind != MCSymbolRefExpr::VK_None)
    OS << ')';
}

void CoffeeCLInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    O << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  printExpr(Op.getExpr(), O);
}

void CoffeeCLInstPrinter::printUnsignedImm(const MCInst *MI, int opNum,
                                       raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(opNum);
  if (MO.isImm())
    O << (unsigned short int)MO.getImm();
  else
    printOperand(MI, opNum, O);
}

void CoffeeCLInstPrinter::
printMemOperand(const MCInst *MI, int opNum, raw_ostream &O) {
  // Load/Store memory operands -- imm($reg)
  // If PIC target the target is loaded as the
  // pattern lw $25,%call16($28)
  printOperand(MI, opNum+1, O);
  O << "(";
  printOperand(MI, opNum, O);
  O << ")";
}

void CoffeeCLInstPrinter::
printMemOperandEA(const MCInst *MI, int opNum, raw_ostream &O) {
  // when using stack locations for not load/store instructions
  // print the same way as all normal 3 operand instructions.
  printOperand(MI, opNum, O);
  O << ", ";
  printOperand(MI, opNum+1, O);
  return;
}

void CoffeeCLInstPrinter::
printFCCOperand(const MCInst *MI, int opNum, raw_ostream &O) {
  const MCOperand& MO = MI->getOperand(opNum);
  O << CoffeeCLFCCToString((CoffeeCL::CondCode)MO.getImm());
}
