//===-- CoffeeCLBaseInfo.h - Top level definitions for CoffeeCL MC ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the CoffeeCL target useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef CoffeeCLBASEINFO_H
#define CoffeeCLBASEINFO_H

#include "CoffeeCLFixupKinds.h"
#include "CoffeeCLMCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

/// CoffeeCLII - This namespace holds all of the target specific flags that
/// instruction info tracks.
///
namespace CoffeeCLII {
  /// Target Operand Flag enum.
  enum TOF {
    //===------------------------------------------------------------------===//
    // CoffeeCL Specific MachineOperand flags.

    MO_NO_FLAG,

    /// MO_GOT16 - Represents the offset into the global offset table at which
    /// the address the relocation entry symbol resides during execution.
    MO_GOT16,
    MO_GOT,

    /// MO_GOT_CALL - Represents the offset into the global offset table at
    /// which the address of a call site relocation entry symbol resides
    /// during execution. This is different from the above since this flag
    /// can only be present in call instructions.
    MO_GOT_CALL,

    /// MO_GPREL - Represents the offset from the current gp value to be used
    /// for the relocatable object file being produced.
    MO_GPREL,

    /// MO_ABS_HI/LO - Represents the hi or low part of an absolute symbol
    /// address.
    MO_ABS_HI,
    MO_ABS_LO,

    /// MO_TLSGD - Represents the offset into the global offset table at which
    // the module ID and TSL block offset reside during execution (General
    // Dynamic TLS).
    MO_TLSGD,

    /// MO_TLSLDM - Represents the offset into the global offset table at which
    // the module ID and TSL block offset reside during execution (Local
    // Dynamic TLS).
    MO_TLSLDM,
    MO_DTPREL_HI,
    MO_DTPREL_LO,

    /// MO_GOTTPREL - Represents the offset from the thread pointer (Initial
    // Exec TLS).
    MO_GOTTPREL,

    /// MO_TPREL_HI/LO - Represents the hi and low part of the offset from
    // the thread pointer (Local Exec TLS).
    MO_TPREL_HI,
    MO_TPREL_LO,

    // N32/64 Flags.
    MO_GPOFF_HI,
    MO_GPOFF_LO,
    MO_GOT_DISP,
    MO_GOT_PAGE,
    MO_GOT_OFST,

    /// MO_HIGHER/HIGHEST - Represents the highest or higher half word of a
    /// 64-bit symbol address.
    MO_HIGHER,
    MO_HIGHEST
  };

  enum {
    //===------------------------------------------------------------------===//
    // Instruction encodings.  These are the standard/most common forms for
    // CoffeeCL instructions.
    //

    // Pseudo - This represents an instruction that is a pseudo instruction
    // or one that has not been implemented yet.  It is illegal to code generate
    // it, but tolerated for intermediate implementation stages.
    Pseudo   = 0,

    /// FrmR - This form is for instructions of the format R.
    FrmR  = 1,
    /// FrmI - This form is for instructions of the format I.
    FrmI  = 2,
    /// FrmJ - This form is for instructions of the format J.
    FrmJ  = 3,
    /// FrmFR - This form is for instructions of the format FR.
    FrmFR = 4,
    /// FrmFI - This form is for instructions of the format FI.
    FrmFI = 5,
    /// FrmOther - This form is for instructions that have no specific format.
    FrmOther = 6,

    FormMask = 15
  };
}


/// getCoffeeCLRegisterNumbering - Given the enum value for some register,
/// return the number that it corresponds to.
inline static unsigned getCoffeeCLRegisterNumbering(unsigned RegEnum)
{
  switch (RegEnum) {
  case CoffeeCL::T0: case CoffeeCL::CR0:
    return 0;
  case CoffeeCL::T1: case CoffeeCL::CR1:
    return 1;
  case CoffeeCL::T2: case CoffeeCL::CR2:
    return 2;
  case CoffeeCL::T3: case CoffeeCL::CR3:
    return 3;
  case CoffeeCL::T4: case CoffeeCL::CR4:
    return 4;
  case CoffeeCL::T5: case CoffeeCL::CR5:
    return 5;
  case CoffeeCL::T6: case CoffeeCL::CR6:
    return 6;
  case CoffeeCL::T7: case CoffeeCL::CR7:
    return 7;
  case CoffeeCL::T8:
    return 8;
  case CoffeeCL::T9:
    return 9;
  case CoffeeCL::S0:
    return 10;
  case CoffeeCL::S1:
    return 11;
  case CoffeeCL::S2:
    return 12;
  case CoffeeCL::S3:
    return 13;
  case CoffeeCL::S4:
    return 14;
  case CoffeeCL::S5:
    return 15;
  case CoffeeCL::S6:
    return 16;
  case CoffeeCL::S7:
    return 17;
  case CoffeeCL::S8:
    return 18;
  case CoffeeCL::S9:
    return 19;
  case CoffeeCL::S10:
    return 20;
  case CoffeeCL::A0:
    return 21;
  case CoffeeCL::A1:
    return 22;
  case CoffeeCL::A2:
    return 23;
  case CoffeeCL::A3:
    return 24;
  case CoffeeCL::V0:
    return 25;
  case CoffeeCL::V1:
    return 26;
  case CoffeeCL::SP:
    return 27;
  case CoffeeCL::FP:
    return 28;
  case CoffeeCL::PSR:
    return 29;
  case CoffeeCL::SPSR:
    return 30;
  case CoffeeCL::LR:
    return 31;
  default: llvm_unreachable("Unknown register number!");
  }
}

inline static std::pair<const MCSymbolRefExpr*, int64_t>
CoffeeCLGetSymAndOffset(const MCFixup &Fixup) {
  MCFixupKind FixupKind = Fixup.getKind();

  if ((FixupKind < FirstTargetFixupKind) ||
      (FixupKind >= MCFixupKind(CoffeeCL::LastTargetFixupKind)))
    return std::make_pair((const MCSymbolRefExpr*)0, (int64_t)0);

  const MCExpr *Expr = Fixup.getValue();
  MCExpr::ExprKind Kind = Expr->getKind();

  if (Kind == MCExpr::Binary) {
    const MCBinaryExpr *BE = static_cast<const MCBinaryExpr*>(Expr);
    const MCExpr *LHS = BE->getLHS();
    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());

    if ((LHS->getKind() != MCExpr::SymbolRef) || !CE)
      return std::make_pair((const MCSymbolRefExpr*)0, (int64_t)0);

    return std::make_pair(cast<MCSymbolRefExpr>(LHS), CE->getValue());
  }

  if (Kind != MCExpr::SymbolRef)
    return std::make_pair((const MCSymbolRefExpr*)0, (int64_t)0);

  return std::make_pair(cast<MCSymbolRefExpr>(Expr), 0);
}
}

#endif
