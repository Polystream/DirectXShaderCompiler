//===-- SpirvFunction.h - SPIR-V Function ---------------------*- C++ -*---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_SPIRVFUNCTION_H
#define LLVM_CLANG_SPIRV_SPIRVFUNCTION_H

#include <vector>

#include "clang/SPIRV/SpirvInstruction.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"

namespace clang {
namespace spirv {

class SpirvBasicBlock;
class SpirvVisitor;

/// The class representing a SPIR-V function in memory.
class SpirvFunction {
public:
  SpirvFunction(QualType astReturnType, SpirvType *fnSpirvType,
                spv::FunctionControlMask, SourceLocation,
                llvm::StringRef name = "");
  ~SpirvFunction() = default;

  // Forbid copy construction and assignment
  SpirvFunction(const SpirvFunction &) = delete;
  SpirvFunction &operator=(const SpirvFunction &) = delete;

  // Forbid move construction and assignment
  SpirvFunction(SpirvFunction &&) = delete;
  SpirvFunction &operator=(SpirvFunction &&) = delete;

  // Handle SPIR-V function visitors.
  bool invokeVisitor(Visitor *, bool reverseOrder = false);

  uint32_t getResultId() const { return functionId; }
  void setResultId(uint32_t id) { functionId = id; }

  // Sets the lowered (SPIR-V) return type.
  void setReturnType(SpirvType *type) { returnType = type; }
  // Returns the lowered (SPIR-V) return type.
  SpirvType *getReturnType() const { return returnType; }

  void setAstReturnType(QualType type) { astReturnType = type; }
  QualType getAstReturnType() const { return astReturnType; }

  // Sets the SPIR-V type of the function
  void setFunctionType(SpirvType *type) { fnType = type; }
  // Returns the SPIR-V type of the function
  SpirvType *getFunctionType() const { return fnType; }

  void setSourceLocation(SourceLocation loc) { functionLoc = loc; }
  SourceLocation getSourceLocation() const { return functionLoc; }

  void setFunctionName(llvm::StringRef name) { functionName = name; }
  llvm::StringRef getFunctionName() const { return functionName; }

  void addParameter(SpirvFunctionParameter *);
  void addVariable(SpirvVariable *);
  void addBasicBlock(SpirvBasicBlock *);

  /// Legalization-specific code
  ///
  /// Note: the following methods are used for properly handling aliasing.
  ///
  /// TODO: Clean up aliasing and try to move it to a separate pass.
  void setConstainsAliasComponent(bool isAlias) { containsAlias = isAlias; }
  bool constainsAliasComponent() { return containsAlias; }
  void setRValue() { rvalue = true; }
  bool isRValue() { return rvalue; }

private:
  uint32_t functionId; ///< This function's <result-id>

  QualType astReturnType; ///< The return type
  SpirvType *returnType;  ///< The lowered return type
  SpirvType *fnType;      ///< The SPIR-V function type

  /// Legalization-specific code
  ///
  /// Note: the following two member variables are currently needed in order to
  /// support aliasing for functions.
  ///
  /// TODO: Clean up aliasing and try to move it to a separate pass.
  bool containsAlias; ///< Whether function return type is aliased
  bool rvalue;        ///< Whether the return value is an rvalue

  spv::FunctionControlMask functionControl; ///< SPIR-V function control
  SourceLocation functionLoc;               ///< Location in source code
  std::string functionName;                 ///< This function's name

  /// Parameters to this function.
  llvm::SmallVector<SpirvFunctionParameter *, 8> parameters;

  /// Variables defined in this function.
  ///
  /// Local variables inside a function should be defined at the beginning
  /// of the entry basic block. This serves as a temporary place for holding
  /// these variables.
  std::vector<SpirvVariable *> variables;

  /// Basic blocks inside this function.
  std::vector<SpirvBasicBlock *> basicBlocks;
};

} // end namespace spirv
} // end namespace clang

#endif // LLVM_CLANG_SPIRV_SPIRVFUNCTION_H
