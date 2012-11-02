//===-- CoffeeMCAsmInfo.h - Coffee asm properties --------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the MCAsmInfoDarwin class.
//
//===----------------------------------------------------------------------===//

#ifndef CoffeeTARGETASMINFO_H
#define CoffeeTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoDarwin.h"

namespace llvm {

  class CoffeeMCAsmInfoDarwin : public MCAsmInfoDarwin {
    virtual void anchor();
  public:
    explicit CoffeeMCAsmInfoDarwin(bool is64Bit);
  };

  class CoffeeLinuxMCAsmInfo : public MCAsmInfo {
    virtual void anchor();
  public:
    explicit CoffeeLinuxMCAsmInfo(bool is64Bit);
  };

} // namespace llvm

#endif