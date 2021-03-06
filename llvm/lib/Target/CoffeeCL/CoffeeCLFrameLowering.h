//===-- CoffeeCLFrameLowering.h - Define frame lowering for CoffeeCL --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#ifndef CoffeeCL_FRAMEINFO_H
#define CoffeeCL_FRAMEINFO_H

#include "CoffeeCL.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ADT/STLExtras.h"

namespace llvm {

class CoffeeCLFrameLowering: public TargetFrameLowering {

public:
    CoffeeCLFrameLowering()

        : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 4, 0)
    {
        // currently, we don't have type size bigger than 4 bytes.
    }



    void emitPrologue(MachineFunction &MF) const;
    void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

    bool hasFP(const MachineFunction &MF) const;



    void processFunctionBeforeCalleeSavedScan(MachineFunction &MF,
                                              RegScavenger *RS = NULL) const;

    bool
    hasReservedCallFrame(const MachineFunction &MF) const;

    uint64_t estimateStackSize(const MachineFunction &MF) const;

    bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                            MachineBasicBlock::iterator MI,
                                            const std::vector<CalleeSavedInfo> &CSI,
                                            const TargetRegisterInfo *TRI) const;

};

} // End llvm namespace

#endif
