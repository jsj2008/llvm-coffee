//===-- CoffeeMCAsmInfo.cpp - Coffee asm properties -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the MCAsmInfoDarwin properties.
//
//===----------------------------------------------------------------------===//

#include "CoffeeMCAsmInfo.h"
using namespace llvm;

void CoffeeMCAsmInfoDarwin::anchor() { }

CoffeeMCAsmInfoDarwin::CoffeeMCAsmInfoDarwin(bool is64Bit) {
  if (is64Bit)
    PointerSize = 8;
  IsLittleEndian = false;

  PCSymbol = ".";
  CommentString = ";";
  ExceptionsType = ExceptionHandling::DwarfCFI;

  if (!is64Bit)
    Data64bitsDirective = 0;      // We can't emit a 64-bit unit in Coffee32 mode.

  AssemblerDialect = 1;           // New-Style mnemonics.
  SupportsDebugInformation= true; // Debug information.
}

void CoffeeLinuxMCAsmInfo::anchor() { }

CoffeeLinuxMCAsmInfo::CoffeeLinuxMCAsmInfo(bool is64Bit) {
  if (is64Bit)
    PointerSize = 8;
  IsLittleEndian = false;

  // ".comm align is in bytes but .align is pow-2."
  AlignmentIsInBytes = false;

  CommentString = "#";
  GlobalPrefix = "";
  PrivateGlobalPrefix = ".L";
  WeakRefDirective = "\t.weak\t";
  
  // Uses '.section' before '.bss' directive
  UsesELFSectionDirectiveForBSS = true;  

  // Debug Information
  SupportsDebugInformation = true;

  PCSymbol = ".";

  // Set up DWARF directives
  HasLEB128 = true;  // Target asm supports leb128 directives (little-endian)

  // Exceptions handling
  if (!is64Bit)
    ExceptionsType = ExceptionHandling::DwarfCFI;
    
  ZeroDirective = "\t.space\t";
  Data64bitsDirective = is64Bit ? "\t.quad\t" : 0;
  LCOMMDirectiveType = LCOMM::NoAlignment;
  AssemblerDialect = 0;           // Old-Style mnemonics.
}
