//===-- llvm/Target/CoffeeTargetObjectFile.h - Coffee Object Info ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_Coffee_TARGETOBJECTFILE_H
#define LLVM_TARGET_Coffee_TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

  class CoffeeTargetObjectFile : public TargetLoweringObjectFileELF {
    const MCSection *SmallDataSection;
    const MCSection *SmallBSSSection;
  public:

    void Initialize(MCContext &Ctx, const TargetMachine &TM);


    /// IsGlobalInSmallSection - Return true if this global address should be
    /// placed into small data/bss section.
    bool IsGlobalInSmallSection(const GlobalValue *GV,
                                const TargetMachine &TM, SectionKind Kind)const;
    bool IsGlobalInSmallSection(const GlobalValue *GV,
                                const TargetMachine &TM) const;

    const MCSection *SelectSectionForGlobal(const GlobalValue *GV,
                                            SectionKind Kind,
                                            Mangler *Mang,
                                            const TargetMachine &TM) const;

    // TODO: Classify globals as Coffee wishes.
  };
} // end namespace llvm

#endif