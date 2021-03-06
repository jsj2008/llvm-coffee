//===-- CoffeeMCTargetDesc.h - Coffee Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Coffee specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef CoffeeMCTARGETDESC_H
#define CoffeeMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
class Target;
class raw_ostream;

extern Target TheCoffeeTarget;

MCCodeEmitter *createCoffeeMCCodeEmitter(const MCInstrInfo &MCII,
                                         const MCRegisterInfo &MRI,
                                         const MCSubtargetInfo &STI,
                                         MCContext &Ctx);

MCAsmBackend *createCoffeeAsmBackend(const Target &T, StringRef TT,
                                       StringRef CPU);


MCObjectWriter *createCoffeeELFObjectWriter(raw_ostream &OS,
                                          uint8_t OSABI,
                                          bool IsLittleEndian,
                                          bool Is64Bit);
} // End llvm namespace

// Defines symbolic names for Coffee registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "CoffeeGenRegisterInfo.inc"

// Defines symbolic names for the Coffee instructions.
#define GET_INSTRINFO_ENUM
#include "CoffeeGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "CoffeeGenSubtargetInfo.inc"

#endif
