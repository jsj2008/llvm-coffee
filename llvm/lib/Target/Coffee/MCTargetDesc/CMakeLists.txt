add_llvm_library(LLVMCoffeeDesc
  CoffeeAsmBackend.cpp
  CoffeeDirectObjLower.cpp
  CoffeeMCAsmInfo.cpp
  CoffeeMCCodeEmitter.cpp
  CoffeeMCTargetDesc.cpp
  CoffeeELFObjectWriter.cpp
  )

add_dependencies(LLVMCoffeeDesc CoffeeCommonTableGen)
