/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef jit_mips32_MacroAssembler_ppc_inl_h
#define jit_mips32_MacroAssembler_ppc_inl_h

#include "jit/osxppc/MacroAssembler-ppc.h"

namespace js {
namespace jit {

//{{{ check_macroassembler_style
// ===============================================================
// Logical instructions
// MacroAssembler:: -> MacroAssembler::

void
MacroAssembler::move64(Imm64 imm, Register64 dest)
{
    move32(Imm32(int32_t(imm.value >> 32)), dest.high);
    move32(Imm32(int32_t(imm.value)), dest.low);
}

void
MacroAssembler::move64(Register64 src, Register64 dest)
{
    MacroAssemblerPPCCompat::move64(src, dest);
}

// XXX. Convert these to direct assembler calls, since the idea is for speed here.
// XXX. 64 bit equivalents for G5?

void
MacroAssembler::not32(Register reg)
{
	ispew("not32(reg)");
	// OPPCC appendix A p.540
    nor(reg, reg, reg);
}

void
MacroAssembler::and32(Register src, Register dest)
{
	ispew("and32(reg, reg)");
    ma_and(dest, dest, src);
}

void
MacroAssembler::and32(Imm32 imm, Register dest)
{
	ispew("and32(imm, reg)");
    ma_and(dest, imm);
}

void
MacroAssembler::and32(Imm32 imm, const Address &dest)
{
	ispew("[[ and32(imm, adr)");
    load32(dest, addressTempRegister);
    ma_and(addressTempRegister, imm);
    store32(addressTempRegister, dest);
    ispew("   and32(imm, adr) ]]");
}

void
MacroAssembler::and32(const Address &src, Register dest)
{
	ispew("[[ and32(adr, reg)");
    load32(src, tempRegister);
    ma_and(dest, tempRegister);
    ispew("   and32(adr, reg) ]]");
}

void
MacroAssembler::andPtr(Imm32 imm, Register dest)
{
	ispew("andPtr(imm, reg)");
    ma_and(dest, imm);
}

void
MacroAssembler::andPtr(Register src, Register dest)
{
	ispew("andPtr(reg, reg)");
    ma_and(dest, src);
}

void
MacroAssembler::and64(Imm64 imm, Register64 dest)
{
// This could be optimized if one of the 32-bit halves were 0xffffffff.
// XXX G5
	ispew("and64(imm64, reg64)");
    and32(Imm32(imm.value & 0xFFFFFFFFL), dest.low);
    and32(Imm32((imm.value >> 32) & 0xFFFFFFFFL), dest.high);
}

void
MacroAssembler::or32(Register src, Register dest)
{
	ispew("or32(reg, reg)");
    ma_or(dest, src);
}

void
MacroAssembler::or32(Imm32 imm, Register dest)
{
	ispew("or32(imm, reg)");
    ma_or(dest, imm);
}

void
MacroAssembler::or32(Imm32 imm, const Address &dest)
{
	ispew("[[ or32(imm, adr)");
    load32(dest, addressTempRegister);
    ma_or(addressTempRegister, imm);
    store32(addressTempRegister, dest);
	ispew("   or32(imm, adr) ]]");
}

void
MacroAssembler::orPtr(Register src, Register dest)
{
	ispew("orPtr(reg, reg)");
    ma_or(dest, src);
}

void
MacroAssembler::orPtr(Imm32 imm, Register dest)
{
	ispew("orPtr(imm, reg)");
    ma_or(dest, imm);
}

void
MacroAssembler::or64(Register64 src, Register64 dest)
{
// XXX. On G5, or rx,rx,rx is already a 64-bit operation.
    ispew("or64(reg64, reg64)");
    or32(src.low, dest.low);
    or32(src.high, dest.high);
}

void
MacroAssembler::xor32(Imm32 imm, Register dest)
{
	ispew("xor32(imm, reg)");
    ma_xor(dest, imm);
}

void
MacroAssembler::xor32(Register src, Register dest)
{
    ma_xor(dest, src);
}

void
MacroAssembler::xor64(Register64 src, Register64 dest)
{
// XXX G5
    ma_xor(dest.low, src.low);
    ma_xor(dest.high, src.high);
}

void
MacroAssembler::xorPtr(Imm32 imm, Register dest)
{
	ispew("xorPtr(imm, reg)");
    ma_xor(dest, imm);
}

void
MacroAssembler::xorPtr(Register src, Register dest)
{
	ispew("xorPtr(reg, reg)");
    ma_xor(dest, src);
}

// ===============================================================
// Arithmetic functions

#if(0)
void
MacroAssembler::add64(Imm32 imm, Register64 dest)
{
    ispew("[[ add64(imm, r64)"); 
    // This is pretty simple because it's a 32-bit integer, so we just do add with carry.
    x_li32(tempRegister, imm.value);
    addc(dest.low, dest.low, tempRegister); /* XXX: optimize to addic if possible */
    addze(dest.high, dest.high);
    ispew("   add64(imm, r64) ]]");
}
#endif

void
MacroAssembler::add64(Register64 src, Register64 dest)
{
// XXX: rldimi should be able to turn an r64 into a real 64 bit register
// then srd and and can do it back
    ispew("[[ add64(r64, r64)");
    // Just do add with carry.
    addc(dest.low, dest.low, src.low);
    adde(dest.high, dest.high, src.high);
    ispew("   add64(r64, r64) ]]");
}

void
MacroAssembler::sub32(Imm32 imm, Register dest)
{
        ispew("sub32(imm, reg)");
    ma_subu(dest, dest, imm);
}

void
MacroAssembler::sub32(Register src, Register dest)
{
        ispew("sub32(reg, reg)");
    ma_subu(dest, dest, src);
}

void
MacroAssembler::sub32(const Address& src, Register dest)
{
    ispew("[[ sub32(adr, reg)");
    MOZ_ASSERT(dest != tempRegister);
    load32(src, tempRegister);
    ma_subu(dest, dest, tempRegister); // keep MIPS operand order
    ispew("   sub32(adr, reg) ]]");
}

void
MacroAssembler::add32(Register src, Register dest)
{
    MacroAssemblerPPCCompat::add32(src, dest);
}

void
MacroAssembler::add32(Imm32 imm, Register dest)
{
    MacroAssemblerPPCCompat::add32(imm, dest);
}

void
MacroAssembler::add32(Imm32 imm, const Address& dest)
{
    MacroAssemblerPPCCompat::add32(imm, dest);
}

void
MacroAssembler::addPtr(Register src, Register dest)
{
    MacroAssemblerPPCCompat::addPtr(src, dest);
}

void
MacroAssembler::addPtr(Imm32 imm, Register dest)
{
    MacroAssemblerPPCCompat::addPtr(imm, dest);
}

void
MacroAssembler::addPtr(ImmWord imm, Register dest)
{
    MacroAssemblerPPCCompat::addPtr(imm, dest);
}

void
MacroAssembler::addPtr(Imm32 imm, const Address& dest)
{
    MacroAssemblerPPCCompat::addPtr(imm, dest);
}

void
MacroAssembler::addPtr(const Address& src, Register dest)
{
    MacroAssemblerPPCCompat::addPtr(src, dest);
}

void
MacroAssembler::subPtr(Register src, Register dest)
{
    MacroAssemblerPPCCompat::subPtr(src, dest);
}

void
MacroAssembler::subPtr(Imm32 imm, Register dest)
{
    MacroAssemblerPPCCompat::subPtr(imm, dest);
}

void
MacroAssembler::subPtr(const Address& src, Register dest)
{
    MacroAssemblerPPCCompat::subPtr(src, dest);
}

void
MacroAssembler::mulBy3(Register src, Register dest)
{
    MacroAssemblerPPCCompat::mulBy3(src, dest);
}

void
MacroAssembler::mulDouble(FloatRegister src, FloatRegister dest)
{
    MacroAssemblerPPC::mulDouble(src, dest);
}

void
MacroAssembler::mulDoublePtr(ImmPtr imm, Register temp, FloatRegister dest)
{
    MacroAssemblerPPC::mulDoublePtr(imm, temp, dest);
}

void
MacroAssembler::addDouble(FloatRegister src, FloatRegister dest)
{
    MacroAssemblerPPC::addDouble(src, dest);
}

void
MacroAssembler::subDouble(FloatRegister src, FloatRegister dest)
{
    MacroAssemblerPPC::subDouble(src, dest);
}

void
MacroAssembler::divDouble(FloatRegister src, FloatRegister dest)
{
    MacroAssemblerPPC::divDouble(src, dest);
}

void
MacroAssembler::inc64(AbsoluteAddress dest)
{
    MacroAssemblerPPC::inc64(dest);
}

void
MacroAssembler::neg32(Register reg)
{
    MacroAssemblerPPCCompat::neg32(reg);
}

void
MacroAssembler::negateDouble(FloatRegister reg)
{
    MacroAssemblerPPC::negateDouble(reg);
}

void
MacroAssembler::move8SignExtend(Register src, Register dest)
{
    extsb(dest, src);
}

void
MacroAssembler::move16SignExtend(Register src, Register dest)
{
    extsh(dest, src);
}

template <typename T1, typename T2>
void
MacroAssembler::cmpPtrSet(Condition cond, T1 lhs, T2 rhs, Register dest)
{
    MacroAssemblerPPCCompat::cmpPtrSet(cond, lhs, rhs, dest);
}

template <typename T1, typename T2>
void
MacroAssembler::cmp32Set(Condition cond, T1 lhs, T2 rhs, Register dest)
{
    MacroAssemblerPPCCompat::cmp32Set(cond, lhs, rhs, dest);
}

CodeOffset
MacroAssembler::farJumpWithPatch()
{
    MOZ_CRASH("wasm is not supported on PPC");
    return CodeOffset();
}

void
MacroAssembler::patchFarJump(CodeOffset, uint32_t)
{
    MOZ_CRASH("wasm is not supported on PPC");
}

void
MacroAssembler::repatchFarJump(uint8_t*, uint32_t, uint32_t)
{
    MOZ_CRASH("wasm is not supported on PPC");
}

template <class L>
void
MacroAssembler::branch32(Condition cond, Register lhs, Register rhs, L label)
{
    MacroAssemblerPPCCompat::branch32(cond, lhs, rhs, label);
}

template <>
void
MacroAssembler::branch32<wasm::TrapDesc>(Condition cond, Register lhs, Register rhs,
                                         wasm::TrapDesc target)
{
    Label done;
    MacroAssemblerPPCCompat::branch32(InvertCondition(cond), lhs, rhs, &done);
    jump(target);
    bind(&done);
}

template <class L>
void
MacroAssembler::branch32(Condition cond, Register lhs, Imm32 rhs, L label)
{
    MacroAssemblerPPCCompat::branch32(cond, lhs, rhs, label);
}

template <>
void
MacroAssembler::branch32<wasm::TrapDesc>(Condition cond, Register lhs, Imm32 rhs,
                                         wasm::TrapDesc target)
{
    Label done;
    MacroAssemblerPPCCompat::branch32(InvertCondition(cond), lhs, rhs, &done);
    jump(target);
    bind(&done);
}

void
MacroAssembler::branch32(Condition cond, const Address& lhs, Register rhs, Label* label)
{
    MacroAssemblerPPCCompat::branch32(cond, lhs, rhs, label);
}

void
MacroAssembler::branch32(Condition cond, const Address& lhs, Imm32 rhs, Label* label)
{
    MacroAssemblerPPCCompat::branch32(cond, lhs, rhs, label);
}

void
MacroAssembler::branch32(Condition cond, const AbsoluteAddress& lhs, Register rhs, Label* label)
{
    MacroAssemblerPPCCompat::branch32(cond, lhs, rhs, label);
}

void
MacroAssembler::branch32(Condition cond, const AbsoluteAddress& lhs, Imm32 rhs, Label* label)
{
    MacroAssemblerPPCCompat::branch32(cond, lhs, rhs, label);
}

void
MacroAssembler::branch32(Condition cond, const BaseIndex& lhs, Imm32 rhs, Label* label)
{
    MacroAssemblerPPCCompat::branch32(cond, lhs, rhs, label);
}

template <class L>
void
MacroAssembler::branchPtr(Condition cond, Register lhs, Register rhs, L label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

template <>
void
MacroAssembler::branchPtr<wasm::TrapDesc>(Condition cond, Register lhs, Register rhs,
                                          wasm::TrapDesc target)
{
    Label done;
    MacroAssemblerPPCCompat::branchPtr(InvertCondition(cond), lhs, rhs, &done);
    jump(target);
    bind(&done);
}

void
MacroAssembler::branchPtr(Condition cond, Register lhs, Imm32 rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchPtr(Condition cond, Register lhs, ImmPtr rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchPtr(Condition cond, Register lhs, ImmGCPtr rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchPtr(Condition cond, Register lhs, ImmWord rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

template <class L>
void
MacroAssembler::branchPtr(Condition cond, const Address& lhs, Register rhs, L label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchPtr(Condition cond, const Address& lhs, ImmPtr rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchPtr(Condition cond, const Address& lhs, ImmGCPtr rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchPtr(Condition cond, const Address& lhs, ImmWord rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchPtr(Condition cond, const AbsoluteAddress& lhs, Register rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchPtr(Condition cond, const AbsoluteAddress& lhs, ImmWord rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPtr(cond, lhs, rhs, label);
}

template <typename T>
CodeOffsetJump
MacroAssembler::branchPtrWithPatch(Condition cond, Register lhs, T rhs, RepatchLabel* label)
{
    return MacroAssemblerPPCCompat::branchPtrWithPatch(cond, lhs, rhs, label);
}

template <typename T>
CodeOffsetJump
MacroAssembler::branchPtrWithPatch(Condition cond, Address lhs, T rhs, RepatchLabel* label)
{
    return MacroAssemblerPPCCompat::branchPtrWithPatch(cond, lhs, rhs, label);
}

template <typename T, typename L>
void
MacroAssembler::branchAdd32(Condition cond, T src, Register dest, L label)
{
    MacroAssemblerPPCCompat::branchAdd32(cond, src, dest, label);
}

template <typename T>
void
MacroAssembler::branchSub32(Condition cond, T src, Register dest, Label* label)
{
    MacroAssemblerPPCCompat::branchSub32(cond, src, dest, label);
}

void
MacroAssembler::decBranchPtr(Condition cond, Register lhs, Imm32 rhs, Label* label)
{
    MacroAssemblerPPCCompat::decBranchPtr(cond, lhs, rhs, label);
}

template <class L>
void
MacroAssembler::branchTest32(Condition cond, Register lhs, Register rhs, L label)
{
    MacroAssemblerPPCCompat::branchTest32(cond, lhs, rhs, label);
}

template <>
void
MacroAssembler::branchTest32<wasm::TrapDesc>(Condition cond, Register lhs, Register rhs,
                                             wasm::TrapDesc target)
{
    Label done;
    MacroAssemblerPPCCompat::branchTest32(InvertCondition(cond), lhs, rhs, &done);
    jump(target);
    bind(&done);
}

template <class L>
void
MacroAssembler::branchTest32(Condition cond, Register lhs, Imm32 rhs, L label)
{
    MacroAssemblerPPCCompat::branchTest32(cond, lhs, rhs, label);
}

template <>
void
MacroAssembler::branchTest32<wasm::TrapDesc>(Condition cond, Register lhs, Imm32 rhs,
                                             wasm::TrapDesc target)
{
    Label done;
    MacroAssemblerPPCCompat::branchTest32(InvertCondition(cond), lhs, rhs, &done);
    jump(target);
    bind(&done);
}

void
MacroAssembler::branchTest32(Condition cond, const Address& lhs, Imm32 rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchTest32(cond, lhs, rhs, label);
}

void
MacroAssembler::branchTest32(Condition cond, const AbsoluteAddress& lhs, Imm32 rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchTest32(cond, lhs, rhs, label);
}

template <class L>
void
MacroAssembler::branchTestPtr(Condition cond, Register lhs, Register rhs, L label)
{
    MacroAssemblerPPCCompat::branchTestPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchTestPtr(Condition cond, Register lhs, Imm32 rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchTestPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchTestPtr(Condition cond, const Address& lhs, Imm32 rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchTestPtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branch64(Condition cond, const Address& lhs, Imm64 val, Label* label)
{
    MacroAssemblerPPCCompat::branch64(cond, lhs, val, label);
}

void
MacroAssembler::branch64(Condition cond, const Address& lhs, const Address& rhs, Register scratch,
                         Label* label)
{
    MacroAssemblerPPCCompat::branch64(cond, lhs, rhs, scratch, label);
}

void
MacroAssembler::branchTestString(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestString(cond, tag, label);
}

void
MacroAssembler::branchTestString(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestString(cond, address, label);
}

void
MacroAssembler::branchTestString(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestString(cond, value, label);
}

void
MacroAssembler::branchTestSymbol(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestSymbol(cond, tag, label);
}

void
MacroAssembler::branchTestSymbol(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestSymbol(cond, address, label);
}

void
MacroAssembler::branchTestSymbol(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestSymbol(cond, value, label);
}

void
MacroAssembler::branchTestObject(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestObject(cond, tag, label);
}

void
MacroAssembler::branchTestObject(Condition cond, const Address& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestObject(cond, address, label);
}

void
MacroAssembler::branchTestObject(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestObject(cond, address, label);
}

void
MacroAssembler::branchTestObject(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestObject(cond, value, label);
}

void
MacroAssembler::branchTestNull(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestNull(cond, tag, label);
}

void
MacroAssembler::branchTestNull(Condition cond, const Address& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestNull(cond, address, label);
}

void
MacroAssembler::branchTestNull(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestNull(cond, address, label);
}

void
MacroAssembler::branchTestNull(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestNull(cond, value, label);
}

void
MacroAssembler::branchTestUndefined(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestUndefined(cond, tag, label);
}

void
MacroAssembler::branchTestUndefined(Condition cond, const Address& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestUndefined(cond, address, label);
}

void
MacroAssembler::branchTestUndefined(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestUndefined(cond, address, label);
}

void
MacroAssembler::branchTestUndefined(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestUndefined(cond, value, label);
}

void
MacroAssembler::branchTestInt32(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestInt32(cond, tag, label);
}

void
MacroAssembler::branchTestInt32(Condition cond, const Address& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestInt32(cond, address, label);
}

void
MacroAssembler::branchTestInt32(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestInt32(cond, address, label);
}

void
MacroAssembler::branchTestInt32(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestInt32(cond, value, label);
}

void
MacroAssembler::branchTestDouble(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestDouble(cond, tag, label);
}

void
MacroAssembler::branchTestDouble(Condition cond, const Address& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestDouble(cond, address, label);
}

void
MacroAssembler::branchTestDouble(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestDouble(cond, address, label);
}

void
MacroAssembler::branchTestDouble(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestDouble(cond, value, label);
}

void
MacroAssembler::branchTestNumber(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestNumber(cond, tag, label);
}

void
MacroAssembler::branchTestNumber(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestNumber(cond, value, label);
}

void
MacroAssembler::branchTestBoolean(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestBoolean(cond, tag, label);
}

void
MacroAssembler::branchTestBoolean(Condition cond, const Address& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestBoolean(cond, address, label);
}

void
MacroAssembler::branchTestBoolean(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestBoolean(cond, address, label);
}

void
MacroAssembler::branchTestBoolean(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestBoolean(cond, value, label);
}

void
MacroAssembler::branchTestGCThing(Condition cond, const Address& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestGCThing(cond, address, label);
}

void
MacroAssembler::branchTestGCThing(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestGCThing(cond, address, label);
}

void
MacroAssembler::branchTestPrimitive(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestPrimitive(cond, tag, label);
}

void
MacroAssembler::branchTestPrimitive(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestPrimitive(cond, value, label);
}

void
MacroAssembler::branchTestValue(Condition cond, const ValueOperand& value, const Value& v,
                                Label* label)
{
    MacroAssemblerPPCCompat::branchTestValue(cond, value, v, label);
}

void
MacroAssembler::branchTestBooleanTruthy(bool truthy, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestBooleanTruthy(truthy, value, label);
}

void
MacroAssembler::branchTestDoubleTruthy(bool truthy, FloatRegister value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestDoubleTruthy(truthy, value, label);
}

void
MacroAssembler::branchTestInt32Truthy(bool truthy, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestInt32Truthy(truthy, value, label);
}

void
MacroAssembler::branchTestBigInt(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestBigInt(cond, tag, label);
}

void
MacroAssembler::branchTestBigInt(Condition cond, const BaseIndex& address, Label* label)
{
    extractTag(address, addressTempRegister);
    MacroAssemblerPPCCompat::branchTestBigInt(cond, addressTempRegister, label);
}

void
MacroAssembler::branchTestBigInt(Condition cond, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestBigInt(cond, value, label);
}

void
MacroAssembler::branchTestBigIntTruthy(bool truthy, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestBigIntTruthy(truthy, value, label);
}

void
MacroAssembler::branchTestMagic(Condition cond, Register tag, Label* label)
{
    MacroAssemblerPPCCompat::branchTestMagic(cond, tag, label);
}

void
MacroAssembler::branchTestMagic(Condition cond, const Address& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestMagic(cond, address, label);
}

void
MacroAssembler::branchTestMagic(Condition cond, const BaseIndex& address, Label* label)
{
    MacroAssemblerPPCCompat::branchTestMagic(cond, address, label);
}

template <class L>
void
MacroAssembler::branchTestMagic(Condition cond, const ValueOperand& value, L label)
{
    MacroAssemblerPPCCompat::branchTestMagic(cond, value, label);
}

void
MacroAssembler::branchTestMagic(Condition cond, const Address& valaddr, JSWhyMagic why,
                                Label* label)
{
    branchTestMagic(cond, valaddr, label);
    branch32(cond, ToPayload(valaddr), Imm32(why), label);
}

void
MacroAssembler::branchPrivatePtr(Condition cond, const Address& lhs, Register rhs, Label* label)
{
    MacroAssemblerPPCCompat::branchPrivatePtr(cond, lhs, rhs, label);
}

void
MacroAssembler::branchDouble(DoubleCondition cond, FloatRegister lhs, FloatRegister rhs,
                             Label* label)
{
    MacroAssemblerPPCCompat::branchDouble(cond, lhs, rhs, label);
}

void
MacroAssembler::branchFloat(DoubleCondition cond, FloatRegister lhs, FloatRegister rhs,
                            Label* label)
{
    MacroAssemblerPPCCompat::branchFloat(cond, lhs, rhs, label);
}

void
MacroAssembler::clampIntToUint8(Register reg)
{
    MacroAssemblerPPCCompat::clampIntToUint8(reg);
}

void
MacroAssembler::branchValueIsNurseryObject(Condition cond, const Address& address, Register temp,
                                           Label* label)
{
    Label done;
    branchTestObject(NotEqual, address, cond == Equal ? &done : label);
    extractObject(address, temp);
    branchPtrInNurseryChunk(cond, temp, InvalidReg, label);
    bind(&done);
}

void
MacroAssembler::branchValueIsNurseryObject(Condition cond, ValueOperand value, Register temp,
                                           Label* label)
{
    MacroAssemblerPPCCompat::branchValueIsNurseryObject(cond, value, temp, label);
}

void
MacroAssembler::branchTestStringTruthy(bool truthy, const ValueOperand& value, Label* label)
{
    MacroAssemblerPPCCompat::branchTestStringTruthy(truthy, value, label);
}

void
MacroAssembler::storeUncanonicalizedDouble(FloatRegister src, const Address& dest)
{
    MacroAssemblerPPCCompat::storeDouble(src, dest);
}

void
MacroAssembler::storeUncanonicalizedDouble(FloatRegister src, const BaseIndex& dest)
{
    MacroAssemblerPPCCompat::storeDouble(src, dest);
}

void
MacroAssembler::storeUncanonicalizedFloat32(FloatRegister src, const Address& dest)
{
    MacroAssemblerPPCCompat::storeFloat32(src, dest);
}

void
MacroAssembler::storeUncanonicalizedFloat32(FloatRegister src, const BaseIndex& dest)
{
    MacroAssemblerPPCCompat::storeFloat32(src, dest);
}

void
MacroAssembler::branchTruncateDoubleMaybeModUint32(FloatRegister src, Register dest, Label* fail)
{
    branchTruncateDouble(src, dest, fail);
}

void
MacroAssembler::branchTruncateFloat32MaybeModUint32(FloatRegister src, Register dest, Label* fail)
{
    branchTruncateFloat32(src, dest, fail);
}

template <typename T>
void
MacroAssembler::storeUnboxedValue(const ConstantOrRegister& value, MIRType valueType,
                                  const T& dest, MIRType slotType)
{
    MacroAssemblerPPCCompat::storeUnboxedValue(value, valueType, dest, slotType);
}

template <class L>
void
MacroAssembler::wasmBoundsCheck(Condition, Register, L)
{
    MOZ_CRASH("wasm is not supported on PPC");
}

void
MacroAssembler::wasmPatchBoundsCheck(uint8_t*, uint32_t)
{
    MOZ_CRASH("wasm is not supported on PPC");
}


// ===============================================================
// Shift functions

void
MacroAssembler::rotateLeft(Imm32 count, Register input, Register dest)
{
    uint32_t shift = count.value & 31;
    if (!shift) {
        move32(input, dest);
        return;
    }

    rlwinm(dest, input, shift, 0, 31);
}

void
MacroAssembler::rotateLeft(Register count, Register input, Register dest)
{
    MOZ_ASSERT(count != tempRegister);
    MOZ_ASSERT(input != tempRegister);
    slw(tempRegister, input, count);
    x_li32(dest, 32);
    subf(dest, count, dest);
    srw(dest, input, dest);
    or_(dest, dest, tempRegister);
}

void
MacroAssembler::lshiftPtr(Imm32 imm, Register dest)
{
    ispew("lshiftPtr(imm, reg)");
    MOZ_ASSERT(imm.value < 32);
    x_slwi(dest, dest, imm.value);
}

void
MacroAssembler::lshift64(Imm32 imm, Register64 dest)
{
	ispew("lshift64(imm, reg64)");
	MOZ_ASSERT(imm.value < 32);

	// Left rotate high 32 first.
	x_slwi(dest.high, dest.high, imm.value);
	// In low 32, rotate the bits *right* to get the mask
	// to |or| with the rotated high 32.
	x_srwi(addressTempRegister, dest.low, 32-imm.value);
	or_(dest.high, dest.high, addressTempRegister);
	// Left rotate low 32.
	x_slwi(dest.low, dest.low, imm.value);
}

void
MacroAssembler::rshiftPtr(Imm32 imm, Register dest)
{
    ispew("rshiftPtr(imm, reg)");
    MOZ_ASSERT(imm.value < 32);
    x_srwi(dest, dest, imm.value);
}

void
MacroAssembler::rshiftPtrArithmetic(Imm32 imm, Register dest)
{
    ispew("rshiftPtrArithmetic(imm, reg)");
    MOZ_ASSERT(imm.value < 32);
    srawi(dest, dest, imm.value);
}

void
MacroAssembler::rotateRight(Imm32 count, Register input, Register dest)
{
    uint32_t shift = count.value & 31;
    if (!shift) {
        move32(input, dest);
        return;
    }

    rlwinm(dest, input, 32 - shift, 0, 31);
}

void
MacroAssembler::rotateRight(Register count, Register input, Register dest)
{
    MOZ_ASSERT(count != tempRegister);
    MOZ_ASSERT(input != tempRegister);
    x_li32(tempRegister, 32);
    subf(tempRegister, count, tempRegister);
    slw(tempRegister, input, tempRegister);
    srw(dest, input, count);
    or_(dest, dest, tempRegister);
}

void
MacroAssembler::rshift64(Imm32 imm, Register64 dest)
{
    ispew("rshift64(imm, reg64)");
    MOZ_ASSERT(imm.value < 32);

	// Reverse of above. Right rotate low 32 first.
	x_srwi(dest.low, dest.low, imm.value);
	// Left rotate high 32 to get the bits to add to low 32.
	x_slwi(addressTempRegister, dest.high, 32 - imm.value);
	or_(dest.low, addressTempRegister, dest.low);
	// Right rotate high 32.
	x_srwi(dest.high, dest.high, imm.value);
}

void
MacroAssembler::rotateLeft64(Imm32 count, Register64 input, Register64 dest, Register temp)
{
    uint32_t shift = count.value & 63;
    if (shift == 0) {
        if (dest != input)
            move64(input, dest);
        return;
    }

    if (shift < 32) {
        x_srwi(temp, input.low, 32 - shift);
        x_srwi(addressTempRegister, input.high, 32 - shift);
        x_slwi(dest.high, input.high, shift);
        or_(dest.high, dest.high, temp);
        x_slwi(dest.low, input.low, shift);
        or_(dest.low, dest.low, addressTempRegister);
        return;
    }

    if (shift == 32) {
        x_mr(temp, input.high);
        x_mr(dest.high, input.low);
        x_mr(dest.low, temp);
        return;
    }

    shift -= 32;
    x_srwi(temp, input.high, 32 - shift);
    x_srwi(addressTempRegister, input.low, 32 - shift);
    x_slwi(dest.high, input.low, shift);
    or_(dest.high, dest.high, temp);
    x_slwi(dest.low, input.high, shift);
    or_(dest.low, dest.low, addressTempRegister);
}

//}}} check_macroassembler_style
// ===============================================================

} // namespace jit
} // namespace js

#endif /* jit_mips32_MacroAssembler_mips32_inl_h */
