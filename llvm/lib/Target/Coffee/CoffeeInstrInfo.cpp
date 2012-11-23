//===-- CoffeeInstrInfo.cpp - Coffee Instruction Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Coffee implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "CoffeeInstrInfo.h"
#include "Coffee.h"
#include "CoffeeInstrBuilder.h"
#include "CoffeeMachineFunctionInfo.h"
#include "CoffeeTargetMachine.h"
#include "CoffeeAnalyzeImmediate.h"
#include "CoffeeHazardRecognizers.h"
#include "MCTargetDesc/CoffeePredicates.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/STLExtras.h"

#define GET_INSTRINFO_CTOR
#include "CoffeeGenInstrInfo.inc"

using namespace llvm;

CoffeeInstrInfo::CoffeeInstrInfo(CoffeeTargetMachine &tm)
  : CoffeeGenInstrInfo(Coffee::ADJCALLSTACKDOWN, Coffee::ADJCALLSTACKUP),
    TM(tm), RI(*this, *tm.getSubtargetImpl()) {
}


void CoffeeInstrInfo::insertNoop(MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI) const {
    DebugLoc DL;
    BuildMI(MBB, MI, DL, get(Coffee::NOP));
}

MachineInstr*
CoffeeInstrInfo::emitFrameIndexDebugValue(MachineFunction &MF,
                                       int FrameIx, uint64_t Offset,
                                       const MDNode *MDPtr,
                                       DebugLoc DL) const {
    MachineInstrBuilder MIB = BuildMI(MF, DL, get(Coffee::DBG_VALUE))
      .addFrameIndex(FrameIx).addImm(0).addImm(Offset).addMetadata(MDPtr);
    return &*MIB;
}

//===----------------------------------------------------------------------===//
// Branch Analysis
//===----------------------------------------------------------------------===//

bool CoffeeInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,MachineBasicBlock *&TBB,
                                 MachineBasicBlock *&FBB,
                                 SmallVectorImpl<MachineOperand> &Cond,
                                 bool AllowModify) const {
}

unsigned CoffeeInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const {
}

unsigned
CoffeeInstrInfo::InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                           MachineBasicBlock *FBB,
                           const SmallVectorImpl<MachineOperand> &Cond,
                           DebugLoc DL) const {

}

void CoffeeInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator I, DebugLoc DL,
                               unsigned DestReg, unsigned SrcReg,
                               bool KillSrc) const {
    unsigned Opc = 0;

    if (Coffee::GPRCRegClass.contains(DestReg)) { // Copy to CPU Reg.
      if (Coffee::GPRCRegClass.contains(SrcReg))
        Opc = Coffee::Mov;
    }

    assert(Opc && "Cannot copy registers");

    MachineInstrBuilder MIB = BuildMI(MBB, I, DL, get(Opc));

    if (DestReg)
      MIB.addReg(DestReg, RegState::Define);

    if (SrcReg)
      MIB.addReg(SrcReg, getKillRegState(KillSrc));
  }


static MachineMemOperand* GetMemOperand(MachineBasicBlock &MBB, int FI,
                                        unsigned Flag) {
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = *MF.getFrameInfo();
  unsigned Align = MFI.getObjectAlignment(FI);

  return MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(FI), Flag,
                              MFI.getObjectSize(FI), Align);
}

void
CoffeeInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator I,
                                 unsigned SrcReg, bool isKill, int FI,
                                 const TargetRegisterClass *RC,
                                 const TargetRegisterInfo *TRI) const {
    DebugLoc DL;
    if (I != MBB.end()) DL = I->getDebugLoc();
    MachineMemOperand *MMO = GetMemOperand(MBB, FI, MachineMemOperand::MOStore);

    unsigned Opc = 0;

    /*if (RC == Mips::CPURegsRegisterClass)
      Opc = IsN64 ? Mips::SW_P8 : Mips::SW;
    else if (RC == Mips::CPU64RegsRegisterClass)
      Opc = IsN64 ? Mips::SD_P8 : Mips::SD;
    else if (RC == Mips::FGR32RegisterClass)
      Opc = IsN64 ? Mips::SWC1_P8 : Mips::SWC1;
    else if (RC == Mips::AFGR64RegisterClass)
      Opc = Mips::SDC1;
    else if (RC == Mips::FGR64RegisterClass)
      Opc = IsN64 ? Mips::SDC164_P8 : Mips::SDC164;*/
    Opc = Coffee::SW;

    assert(Opc && "Register class not handled!");
    BuildMI(MBB, I, DL, get(Opc)).addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FI).addImm(0).addMemOperand(MMO);
}



void
CoffeeInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator I,
                                   unsigned DestReg, int FI,
                                   const TargetRegisterClass *RC,
                                   const TargetRegisterInfo *TRI) const {
    DebugLoc DL;
    if (I != MBB.end()) DL = I->getDebugLoc();
    MachineMemOperand *MMO = GetMemOperand(MBB, FI, MachineMemOperand::MOLoad);
    unsigned Opc = Coffee::LW;

    /*if (RC == Mips::CPURegsRegisterClass)
      Opc = IsN64 ? Mips::LW_P8 : Mips::LW;
    else if (RC == Mips::CPU64RegsRegisterClass)
      Opc = IsN64 ? Mips::LD_P8 : Mips::LD;
    else if (RC == Mips::FGR32RegisterClass)
      Opc = IsN64 ? Mips::LWC1_P8 : Mips::LWC1;
    else if (RC == Mips::AFGR64RegisterClass)
      Opc = Mips::LDC1;
    else if (RC == Mips::FGR64RegisterClass)
      Opc = IsN64 ? Mips::LDC164_P8 : Mips::LDC164;*/

    assert(Opc && "Register class not handled!");
    BuildMI(MBB, I, DL, get(Opc), DestReg).addFrameIndex(FI).addImm(0)
      .addMemOperand(MMO);
}


bool CoffeeInstrInfo::
ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const {
    assert( (Cond.size() && Cond.size() <= 3) &&
            "Invalid Mips branch condition!");
    Cond[0].setImm(GetOppositeBranchOpc(Cond[0].getImm()));
    return false;
}

unsigned CoffeeInstrInfo::GetInstSizeInBytes(const MachineInstr *MI) const {
    switch (MI->getOpcode()) {
    default:
      return MI->getDesc().getSize();
    case  TargetOpcode::INLINEASM: {       // Inline Asm: Variable size.
      const MachineFunction *MF = MI->getParent()->getParent();
      const char *AsmStr = MI->getOperand(0).getSymbolName();
      return getInlineAsmLength(AsmStr, *MF->getTarget().getMCAsmInfo());
    }
    }
}

/// GetOppositeBranchOpc - Return the inverse of the specified
/// opcode, e.g. turning BEQ to BNE.
unsigned CoffeeInstrInfo::GetOppositeBranchOpc(unsigned Opc) const {
  switch (Opc) {
  default:           llvm_unreachable("Illegal opcode!");
  case Coffee::BEQ:    return Coffee::BNE;
  case Coffee::BNE:    return Coffee::BEQ;
  case Coffee::BEGT:   return Coffee::BLT;
  case Coffee::BLT:   return Coffee::BEGT;

  case Coffee::BELT:   return Coffee::BGT;
  case Coffee::BGT:   return Coffee::BELT;
  }
}

void CoffeeInstrInfo::ExpandRetLR(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I,
                                unsigned Opc) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(Opc)).addReg(Coffee::LR);
}


bool CoffeeInstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const {
  MachineBasicBlock &MBB = *MI->getParent();

  switch(MI->getDesc().getOpcode()) {
  default:
    return false;
  case Coffee::RetLR:
    ExpandRetLR(MBB, MI, Coffee::RET);
    break;
  }

  MBB.erase(MI);
  return true;
}

/// Adjust SP by Amount bytes.
void CoffeeInstrInfo::adjustStackPtr(unsigned SP, int64_t Amount,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const {
  DebugLoc DL = I != MBB.end() ? I->getDebugLoc() : DebugLoc();

  if (isInt<16>(Amount))// addi sp, sp, amount
    BuildMI(MBB, I, DL, get(Coffee::ADDi), Coffee::SP).addReg(Coffee::SP).addImm(Amount);
  else { // Expand immediate that doesn't fit in 16-bit.
    unsigned Reg = loadImmediate(Amount, MBB, I, DL, 0);
    BuildMI(MBB, I, DL, get(Coffee::ADDu), Coffee::SP).addReg(Coffee::SP).addReg(Reg, RegState::Kill);
  }
}

/// This function generates the sequence of instructions needed to get the
/// result of adding register REG and immediate IMM.
unsigned
CoffeeInstrInfo::loadImmediate(int64_t Imm, MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator II, DebugLoc DL,
                               unsigned *NewImm) const {
  CoffeeAnalyzeImmediate AnalyzeImm;

  MachineRegisterInfo &RegInfo = MBB.getParent()->getRegInfo();
  unsigned Size = 32;
  unsigned LUi = Coffee::LUi;

  const TargetRegisterClass *RC = &Coffee::GPRCRegClass;
  bool LastInstrIsADDiu = NewImm;

  const CoffeeAnalyzeImmediate::InstSeq &Seq =
    AnalyzeImm.Analyze(Imm, Size, LastInstrIsADDiu);
  CoffeeAnalyzeImmediate::InstSeq::const_iterator Inst = Seq.begin();

  assert(Seq.size() && (!LastInstrIsADDiu || (Seq.size() > 1)));

  // The first instruction can be a LUi, which is different from other
  // instructions (ADDiu, ORI and SLL) in that it does not have a register
  // operand.
  unsigned Reg = RegInfo.createVirtualRegister(RC);

  if (Inst->Opc == LUi)
    BuildMI(MBB, II, DL, get(LUi), Reg).addImm(SignExtend64<16>(Inst->ImmOpnd));


  // Build the remaining instructions in Seq.
  for (++Inst; Inst != Seq.end() - LastInstrIsADDiu; ++Inst)
    BuildMI(MBB, II, DL, get(Inst->Opc), Reg).addReg(Reg, RegState::Kill)
      .addImm(SignExtend64<16>(Inst->ImmOpnd));

  if (LastInstrIsADDiu)
    *NewImm = Inst->ImmOpnd;

  return Reg;
}
