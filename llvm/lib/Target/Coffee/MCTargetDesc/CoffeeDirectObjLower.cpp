//===-- CoffeeDirectObjLower.cpp - Coffee LLVM direct object lowering -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower Coffee MCInst records that are normally
// left to the assembler to lower such as large shifts.
//
//===----------------------------------------------------------------------===//
#include "CoffeeInstrInfo.h"
#include "MCTargetDesc/CoffeeDirectObjLower.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"

using namespace llvm;

// If the D<shift> instruction has a shift amount that is greater
// than 31 (checked in calling routine), lower it to a D<shift>32 instruction
void Coffee::LowerLargeShift(MCInst& Inst) {

  assert(Inst.getNumOperands() == 3 && "Invalid no. of operands for shift!");
  assert(Inst.getOperand(2).isImm());

  int64_t Shift = Inst.getOperand(2).getImm();
  if (Shift <= 31)
    return; // Do nothing
  Shift -= 32;

  // saminus32
  Inst.getOperand(2).setImm(Shift);

  switch (Inst.getOpcode()) {
  default:
    // Calling function is not synchronized
    llvm_unreachable("Unexpected shift instruction");
  /*case Coffee::DSLL:
    Inst.setOpcode(Coffee::DSLL32);
    return;
  case Coffee::DSRL:
    Inst.setOpcode(Coffee::DSRL32);
    return;
  case Coffee::DSRA:
    Inst.setOpcode(Coffee::DSRA32);
    return;*/
  }
}

// Pick a DEXT or DINS instruction variant based on the pos and size operands
void Coffee::LowerDextDins(MCInst& InstIn) {
 /* int Opcode = InstIn.getOpcode();

  if (Opcode == Coffee::DEXT)
    assert(InstIn.getNumOperands() == 4 &&
           "Invalid no. of machine operands for DEXT!");
  else // Only DEXT and DINS are possible
    assert(InstIn.getNumOperands() == 5 &&
           "Invalid no. of machine operands for DINS!");

  assert(InstIn.getOperand(2).isImm());
  int64_t pos = InstIn.getOperand(2).getImm();
  assert(InstIn.getOperand(3).isImm());
  int64_t size = InstIn.getOperand(3).getImm();

  if (size <= 32) {
    if (pos < 32)  // DEXT/DINS, do nothing
      return;
    // DEXTU/DINSU
    InstIn.getOperand(2).setImm(pos - 32);
    InstIn.setOpcode((Opcode == Coffee::DEXT) ? Coffee::DEXTU : Coffee::DINSU);
    return;
  }
  // DEXTM/DINSM
  assert(pos < 32 && "DEXT/DINS cannot have both size and pos > 32");
  InstIn.getOperand(3).setImm(size - 32);
  InstIn.setOpcode((Opcode == Coffee::DEXT) ? Coffee::DEXTM : Coffee::DINSM);
  return;*/
}
