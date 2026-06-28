/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef jit_ppc_Lowering_ppc_h
#define jit_ppc_Lowering_ppc_h

#include "jit/shared/Lowering-shared.h"

namespace js {
namespace jit {

class LIRGeneratorPPC : public LIRGeneratorShared
{
  protected:
    LIRGeneratorPPC(MIRGenerator *gen, MIRGraph &graph, LIRGraph &lirGraph)
      : LIRGeneratorShared(gen, graph, lirGraph)
    { }

  protected:
    LBoxAllocation useBoxFixed(MDefinition *mir, Register reg1, Register reg2,
                               bool useAtStart = false);

    // x86 has constraints on what registers can be formatted for 1-byte
    // stores and loads; on PowerPC all registers are okay.
    LAllocation useByteOpRegister(MDefinition *mir);
    LAllocation useByteOpRegisterAtStart(MDefinition *mir);
    LAllocation useByteOpRegisterOrNonDoubleConstant(MDefinition *mir);
    LDefinition tempByteOpRegister();

    inline LDefinition tempToUnbox() {
        return LDefinition::BogusTemp();
    }

    bool needTempForPostBarrier() { return false; } // keep bool

    // PowerPC has a scratch register, so no need for another temp for dispatch ICs.
    LDefinition tempForDispatchCache(MIRType outputType = MIRType::None) {
        return LDefinition::BogusTemp();
    }

    void lowerUntypedPhiInput(MPhi *phi, uint32_t inputPosition, LBlock *block, size_t lirIndex);
    void defineUntypedPhi(MPhi *phi, size_t lirIndex);
    void lowerInt64PhiInput(MPhi *phi, uint32_t inputPosition, LBlock *block, size_t lirIndex);
    void defineInt64Phi(MPhi *phi, size_t lirIndex);
    void lowerForALUInt64(LInstructionHelper<INT64_PIECES, 2 * INT64_PIECES, 0> *ins,
                          MDefinition *mir, MDefinition *lhs, MDefinition *rhs);
    void lowerForMulInt64(LMulI64 *ins, MMul *mir, MDefinition *lhs, MDefinition *rhs);
    template<size_t Temps>
    void lowerForShiftInt64(LInstructionHelper<INT64_PIECES, INT64_PIECES + 1, Temps> *ins,
                            MDefinition *mir, MDefinition *lhs, MDefinition *rhs);
    void lowerDivI64(MDiv *div);
    void lowerModI64(MMod *mod);
    void lowerUDivI64(MDiv *div);
    void lowerUModI64(MMod *mod);
    void lowerForShift(LInstructionHelper<1, 2, 0> *ins, MDefinition *mir, MDefinition *lhs,
                       MDefinition *rhs);
    void lowerUrshD(MUrsh *mir);

    void lowerForALU(LInstructionHelper<1, 1, 0> *ins, MDefinition *mir,
                     MDefinition *input);
    void lowerForALU(LInstructionHelper<1, 2, 0> *ins, MDefinition *mir,
                     MDefinition *lhs, MDefinition *rhs);

    void lowerForFPU(LInstructionHelper<1, 1, 0> *ins, MDefinition *mir,
                     MDefinition *src);
    void lowerForFPU(LInstructionHelper<1, 2, 0> *ins, MDefinition *mir,
                     MDefinition *lhs, MDefinition *rhs);
    void lowerForFPU(LInstructionHelper<1, 2, 1> *ins, MDefinition *mir,
                     MDefinition *lhs, MDefinition *rhs);
    template<size_t Temps> // ?!?!
    void lowerForFPU(LInstructionHelper<1, 2, Temps> *ins, MDefinition *mir,
                     MDefinition *lhs, MDefinition *rhs);
    void lowerForBitAndAndBranch(LBitAndAndBranch *baab, MInstruction *mir,
                                 MDefinition *lhs, MDefinition *rhs);
    void lowerConstantDouble(double d, MInstruction *ins);
    void lowerConstantFloat32(float d, MInstruction *ins);
    void lowerTruncateDToInt32(MTruncateToInt32 *ins);
    void lowerTruncateFToInt32(MTruncateToInt32 *ins);
    void lowerDivI(MDiv *div);
    void lowerModI(MMod *mod);
    void lowerMulI(MMul *mul, MDefinition *lhs, MDefinition *rhs);
    void lowerUDiv(MDiv *div);
    void lowerUMod(MMod *mod);
    void visitPowHalf(MPowHalf *ins);
    void visitAsmJSNeg(MAsmJSNeg *ins);

    LTableSwitch *newLTableSwitch(const LAllocation &in, const LDefinition &inputCopy,
                                  MTableSwitch *ins);
    LTableSwitchV *newLTableSwitchV(MTableSwitch *ins);

  public:
    void visitConstant(MConstant *ins);
    void visitBox(MBox *box);
    void visitUnbox(MUnbox *unbox);
    void visitReturn(MReturn *ret);
    void lowerPhi(MPhi *phi);
    void visitGuardShape(MGuardShape *ins);
    void visitGuardObjectGroup(MGuardObjectGroup *ins);
    void visitWasmUnsignedToDouble(MWasmUnsignedToDouble *ins);
    void visitWasmUnsignedToFloat32(MWasmUnsignedToFloat32 *ins);
    void visitWasmLoad(MWasmLoad *ins);
    void visitWasmStore(MWasmStore *ins);
    void visitWasmSelect(MWasmSelect *ins);
    void visitWasmTruncateToInt64(MWasmTruncateToInt64 *ins);
    void visitInt64ToFloatingPoint(MInt64ToFloatingPoint *ins);
    void visitExtendInt32ToInt64(MExtendInt32ToInt64 *ins);
    void visitSignExtendInt64(MSignExtendInt64 *ins);
    void visitCopySign(MCopySign *ins);
    void visitAsmJSLoadHeap(MAsmJSLoadHeap *ins);
    void visitAsmJSStoreHeap(MAsmJSStoreHeap *ins);
    void visitAsmJSCompareExchangeHeap(MAsmJSCompareExchangeHeap *ins);
    void visitAsmJSAtomicExchangeHeap(MAsmJSAtomicExchangeHeap* ins);
    void visitAsmJSAtomicBinopHeap(MAsmJSAtomicBinopHeap *ins);
    void visitStoreTypedArrayElementStatic(MStoreTypedArrayElementStatic *ins);
    void visitCompareExchangeTypedArrayElement(MCompareExchangeTypedArrayElement* ins);
    void visitAtomicExchangeTypedArrayElement(MAtomicExchangeTypedArrayElement* ins);
    void visitAtomicTypedArrayElementBinop(MAtomicTypedArrayElementBinop *ins);
    void visitSubstr(MSubstr *ins);
    void visitRandom(MRandom* ins);
};

typedef LIRGeneratorPPC LIRGeneratorSpecific;

} // namespace jit
} // namespace js

#endif /* jit_ppc_Lowering_ppc_h */
