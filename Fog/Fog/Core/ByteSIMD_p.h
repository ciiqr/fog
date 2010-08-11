// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_BYTESIMD_P_H
#define _FOG_CORE_BYTESIMD_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Graphics/Constants.h>

namespace Fog {

//! @internal
//! 
//! @brief Collection of typedefs and functions to work with bytes/words packed
//! in 32-bit or 64-bit unsigned integers.
//!
//! Functions defined by @c ByteSIMD do operation on:
//!
//!   1. scalar values - all functions using prefix "scalar_"
//!   2. packed values - all functions using prefix "byteXxX_"
//!
//! Data-type width is always described by the function prefix or type definition:
//!
//!   1. @c b32_1x1 - [00.00.00.B0] - Scalar one byte stored in 32-bit unsigned
//!      integer.
//!   2. @c b32_1x2 - [00.B1.00.B0] - Packed two bytes stored in 32-bit unsigned
//!      integer.
//!   3. @c b64_1x1 - [00.00.00.00.00.00.00.B0] - Scalar one byte stored in
//!      64-bit unsigned integer.
//!   4. @c b64_1x3 - [00.00.00.B2.00.B1.00.B0] - Packed three bytes stored in
//!      64-bit unsigned integer.
//!   4. @c b64_1x4 - [00.B3.00.B2.00.B1.00.B0] - Packed four bytes stored in
//!      64-bit unsigned integer.
//!
//! SIMD functionality is usually used to do manipulation with pixels, but
//! the @c ByteSIMD module was written without computer graphics expectations
//! and can be used to work with other multimedia content (for example audio).
//!
//! Data format for working with pixels in 32-bit mode:
//!
//!   - @c b32_1x2 b0 [0x00RR00BB]
//!   - @c b32_1x2 b1 [0x00AA00GG]
//!
//!   - unpacked using b32_2x2Unpack0213(b0, b1, source);
//!   - packed using b32_2x2Pack0213(dest, b0, b1);
//!
//! Data format for working with pixels in 64-bit mode:
//!
//!   - @c b64_1x4 b0 [0x00AA00GG00RR00BB]
//!
//!   - unpacked using b64_1x4Unpack0213(b0, source);
//!   - packed using b64_1x4Pack0213(dest, b0);
//!
//! The reason to not unpack bytes to 0x00AA00RR/00GG00BB is that it's more
//! expensive, unpacking even and odd bytes separately is simpler and working
//! with the data is the same. Notice that the position of bytes/pixels is
//! different to position when using MMX/SSE2 assembler.
//!
//! Function input/output parameter naming terminology:
//!
//!   - @c b32_1x1 - scalar at range of 0 to 255.
//!   - @c uint32_t - scalar at range of 0 to 256.
//!   - @c b32_1x2 - packed two BYTEs in 32-bit unsigned integer.
//!   - @c b32_2x2 - packed two BYTEs in two 32-bit unsigned integers. These two
//!     integers are always passed/returned together.
//!   - @c b64_1x3 - packed three BYTEs in 64-bit unsigned integer.
//!   - @c b64_1x4 - packed four BYTEs in 64-bit unsigned integer.
namespace ByteSIMD {

//! @addtogroup Fog_Core_Private
//! @{

// ============================================================================
// [Fog::ByteSIMD - Implementation Choices]
// ============================================================================

// Whether to use conditional instruction (if) instead of bit manipulation in
// some cases. Enabling this may improve or decrease the performance of code
// depending on compiler and target platform. If you are sure that your 
// compiler can generate good code (using cmov on x86 platform) then it's
// good to define it.
// #define FOG_BYTESIMD_USE_CONDITIONAL_INSTRUCTIONS

// ============================================================================
// [Fog::ByteSIMD - Types]
// ============================================================================

//! @brief Scalar BYTE stored in 32-bit unsigned integer.
//!
//! Data format of value is [00.00.00.B0] or [00.00.00.LO].
typedef uint32_t b32_1x1;

//! @brief Packed two BYTEs stored in 32-bit unsigned integer.
//!
//! Data format of value is [00.B1.00.B0] or [00.HI.00.LO].
typedef uint32_t b32_1x2;

//! @brief Scalar BYTE stored in 64-bit unsigned integer.
//!
//! Data format of value is [00.00.00.00.00.00.00.B0].
typedef uint64_t b64_1x1;

//! @brief Packed three BYTEs stored in 64-bit unsigned integer.
//!
//! Data format of value is [00.00.00.B2.00.B1.00.B0].
typedef uint64_t b64_1x3;

//! @brief Packed four BYTEs stored in 64-bit unsigned integer.
//!
//! Data format of value is [00.B3.00.B2.00.B1.00.B0].
typedef uint64_t b64_1x4;

// ============================================================================
// [Fog::ByteSIMD - Constants]
// ============================================================================

//! @brief Scalar half-BYTE value stored in 32-bit integer.
static const b32_1x1 BYTE1x1HALF = 0x00000080U;
//! @brief Scalar full-BYTE mask stored in 32-bit integer.
static const b32_1x1 BYTE1x1MASK = 0x000000FFU;
//! @brief Scalar full-BYTE mask+1 stored in 32-bit integer.
static const b32_1x1 BYTE1x1MASK_PLUS_ONE = 0x00000100U;

//! @brief Packed two half-BYTE values stored in 32-bit integer.
static const b32_1x2 BYTE1x2HALF = 0x00800080U;
//! @brief Packed two full-BYTE masks stored in 32-bit integer.
static const b32_1x2 BYTE1x2MASK = 0x00FF00FFU;
//! @brief Packed two full-BYTE masks+1 stored in 32-bit integer.
static const b32_1x2 BYTE1x2MASK_PLUS_ONE = 0x10000100U;

//! @brief Packed three half-BYTE values stored in 64-bit integer.
static const b64_1x3 BYTE_1x3HALF = FOG_UINT64_C(0x0000008000800080);
//! @brief Packed three half-BYTE masks stored in 64-bit integer.
static const b64_1x3 BYTE_1x3MASK = FOG_UINT64_C(0x000000FF00FF00FF);
//! @brief Packed three half-BYTE masks+1 stored in 64-bit integer.
static const b64_1x3 BYTE_1x3MASK_PLUS_ONE = FOG_UINT64_C(0x0000010010000100);

//! @brief Packed four half-BYTE values stored in 64-bit integer.
static const b64_1x4 BYTE_1x4HALF = FOG_UINT64_C(0x0080008000800080);
//! @brief Packed four half-BYTE masks stored in 64-bit integer.
static const b64_1x4 BYTE_1x4MASK = FOG_UINT64_C(0x00FF00FF00FF00FF);
//! @brief Packed four half-BYTE masks+1 stored in 64-bit integer.
static const b64_1x4 BYTE_1x4MASK_PLUS_ONE = FOG_UINT64_C(0x1000010010000100);

// ============================================================================
// [Fog::ByteSIMD - U32 - Add/Addus]
// ============================================================================

//! @brief Result = x + y
//!
//! @note Result is not saturated.
static FOG_INLINE uint32_t u32Add(b32_1x1 x, b32_1x1 y)
{
  return x + y;
}

//! @brief Result = min(x + y, 255)
static FOG_INLINE uint32_t u32Addus(b32_1x1 x, b32_1x1 y)
{
  x += y;
#if defined(FOG_BYTESIMD_USE_CONDITIONAL_INSTRUCTIONS)
  if (x > 255) x = 255;
#else
  x |= 0x0100U - ((x >> 8) & 0x00FFU);
  x &= 0x00FFU;
#endif
  return x;
}

// ============================================================================
// [Fog::ByteSIMD - U32 - Sub/Subus]
// ============================================================================

//! @brief Result = x - y
//!
//! @note Result is not saturated.
static FOG_INLINE uint32_t u32Sub(b32_1x1 x, b32_1x1 y)
{
  return x - y;
}

//! @brief Result = max(x - y, 0)
static FOG_INLINE uint32_t u32Subus(b32_1x1 x, b32_1x1 y)
{
  x -= y;
#if defined(FOG_BYTESIMD_USE_CONDITIONAL_INSTRUCTIONS)
  if ((int32_t)x < 0) x = 0;
#else
  x &= (x >> 24) ^ 0xFFU;
#endif
  return x;
}

// ============================================================================
// [Fog::ByteSIMD - U32 - Div255/256]
// ============================================================================

//! @brief Accurate division by 255. The result is equal to <code>(val / 255.0)</code>.
//!
//! @note This template is used for accurate alpha blending.
static FOG_INLINE uint32_t u32Div255(uint32_t i)
{
  return ( (((uint32_t)i << 8U) + ((uint32_t)i + 256U)) >> 16U );
}

//! @brief Accurate division by 256. The result is equal to <code>(val / 256.0)</code>.
//!
//! @not This template is simply implemented as a right shift by 8-bits.
static FOG_INLINE uint32_t u32Div256(uint32_t i)
{
  return (i >> 8U);
}

// ============================================================================
// [Fog::ByteSIMD - U32 - MulDiv255/256]
// ============================================================================

//! @brief Result = (x * a) / 255
static FOG_INLINE b32_1x1 u32MulDiv255(b32_1x1 x, b32_1x1 a)
{
  x *= a;
  x = ((x + (x >> 8) + 0x80U) >> 8);
  return x;
}

//! @brief Result = (x * a) / 256
static FOG_INLINE uint32_t u32MulDiv256(b32_1x1 x, uint32_t a)
{
  return (x * a) >> 8;
}

// ============================================================================
// [Fog::ByteSIMD - U32 - Lerp255/256]
// ============================================================================

//! @brief Result = {(x * a) + (y * (255 - a))} / 255
static FOG_INLINE uint32_t u32Lerp255(b32_1x1 x, b32_1x1 y, b32_1x1 a)
{
  x *= a;
  y *= a ^ 0xFFU;
  x += y;
  x = ((x + (x >> 8) + 0x80U) >> 8);
  return x;
}

//! @brief Result = {(x * a) + (y * (256 - a))} / 256
static FOG_INLINE uint32_t u32Lerp256(b32_1x1 x, b32_1x1 y, uint32_t a)
{
  x *= a;
  y *= 256U - a;
  return (x + y) >> 8;
}

// ============================================================================
// [Fog::ByteSIMD - U32 - Neg255/256]
// ============================================================================

//! @brief Result = 255 - x
static FOG_INLINE uint32_t u32Negate255(b32_1x1 x)
{
  return x ^ 0xFFU;
}

//! @brief Result = 256 - x
static FOG_INLINE uint32_t u32Negate256(uint32_t x)
{
  return 256U - x;
}

// ============================================================================
// [Fog::ByteSIMD - U32 - Extend]
// ============================================================================

//! @brief Result = x | (x << 8) | (x << 16) | (x << 24)
static FOG_INLINE uint32_t u32Extend(b32_1x1 x)
{
  return x * 0x01010101U;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Pack]
// ============================================================================

//! @brief Pack x0 and x1 into single DWORD.
static FOG_INLINE void b32_2x2Pack0213(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 a1)
{
  dst0 = a0 | (a1 << 8);
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Unpack]
// ============================================================================

//! @brief Unpack 0st and 2nd BYTE of single DWORD into one b32_1x2 value.
static FOG_INLINE void b32_2x2Unpack02(
  b32_1x2& dst0, uint32_t a0)
{
  dst0 = (a0 & BYTE1x2MASK);
}

//! @brief Unpack 1st and 3rd BYTE of single DWORD into one b32_1x2 value.
static FOG_INLINE void b32_2x2Unpack13(
  b32_1x2& dst0, uint32_t a0)
{
  dst0 = (a0 >> 8) & BYTE1x2MASK;
}

//! @brief Unpack all bytes of DWORD into two b32_1x2 values.
static FOG_INLINE void b32_2x2Unpack0213(
  b32_1x2& dst0,
  b32_1x2& dst1, uint32_t a0)
{
  dst0 = a0;
  dst1 = a0 >> 8;
  dst0 &= BYTE1x2MASK;
  dst1 &= BYTE1x2MASK;
}

//! @brief Unpack '021' bytes of DWORD into two b32_1x2 values.
static FOG_INLINE void b32_2x2Unpack021X(
  b32_1x2& dst0,
  b32_1x2& dst1, uint32_t a0)
{
  dst0 = a0;
  dst1 = a0 >> 8;
  dst0 &= BYTE1x2MASK;
  dst1 &= BYTE1x1MASK;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Zero / Fill]
// ============================================================================

static FOG_INLINE void b32_1x2ZeroB0(
  b32_1x2& dst0, b32_1x2 a0)
{
  dst0 = a0 & 0x00FF0000U;
}

static FOG_INLINE void b32_1x2ZeroB1(
  b32_1x2& dst0, b32_1x2 a0)
{
  dst0 = a0 & 0x000000FFU;
}

static FOG_INLINE void b32_1x2FillB0(
  b32_1x2& dst0, b32_1x2 a0)
{
  dst0 = a0 | 0x000000FFU;
}

static FOG_INLINE void b32_1x2FillB1(
  b32_1x2& dst0, b32_1x2 a0)
{
  dst0 = a0 | 0x00FF0000U;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Get]
// ============================================================================

//! @brief Get B0 (LO) BYTE from B32_1x2.
static FOG_INLINE b32_1x1 b32_1x2GetB0(b32_1x2 a0) { return (a0 & 0xFFU); }
//! @brief Get B1 (HI) BYTE from B32_1x2.
static FOG_INLINE b32_1x1 b32_1x2GetB1(b32_1x2 a0) { return (a0 >> 16); }

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Set]
// ============================================================================

static FOG_INLINE void b32_1x2SetB0(
  b32_1x2& dst0, b32_1x2 a0, b32_1x1 u)
{
  dst0 = (a0 & 0x00FF0000U) | u;
}

static FOG_INLINE void b32_1x2SetB1(
  b32_1x2& dst0, b32_1x2 a0, b32_1x1 u)
{
  dst0 = (a0 & 0x000000FFU) | (u << 16);
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Copy]
// ============================================================================

//! @brief Copy @a a0 to @a dst0.
//!
//! @verbatim
//! dst0 = a0;
static FOG_INLINE void b32_1x2Copy(
  b32_1x2& dst0, b32_1x2 a0)
{
  dst0 = a0;
}

//! @brief Copy @a a0/a1 to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = a0
//! dst1 = a1
//! @endverbatim
static FOG_INLINE void b32_2x2Copy(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1)
{
  dst0 = a0;
  dst1 = a1;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Extend]
// ============================================================================

//! @brief Extend src0.B0 to dst0, src0.B1 must be zero before call.
//!
//! @verbatim
//! dst0 = [src0.B0, src0.B0]
//! @endverbatim
static FOG_INLINE void b32_1x2ExtendB0(
  b32_1x2& dst0, b32_1x2 src0)
{
  dst0 = src0 * 0x00010001U;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Expand]
// ============================================================================

//! @brief Expand src0.B0 to [dst0.B0, dst0.B1].
//!
//! @verbatim
//! dst0 = [src0.B0, src0.B0]
//! @endverbatim
static FOG_INLINE void b32_1x2ExpandB0(
  b32_1x2& dst0, b32_1x2 src0)
{
  dst0 = (src0 << 16);
  dst0 |= (src0 & 0x000000FFU);
}

//! @brief Expand src0.B0 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0 = [src0.B0, src0.B0]
//! dst1 = [src0.B0, src0.B0]
//! @endverbatim
static FOG_INLINE void b32_2x2ExpandB0(
  b32_1x2& dst0,
  b32_1x2& dst1, b32_1x2 src0)
{
  b32_1x2ExpandB0(dst0, src0);
  dst1 = dst0;
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1].
//!
//! @verbatim
//! dst0 = [src0.B1, src0.B1]
//! @endverbatim
static FOG_INLINE void b32_1x2ExpandB1(
  b32_1x2& dst0, b32_1x2 src0)
{
  dst0 = (src0 >> 16);
  dst0 |= (src0 & 0x00FF0000U);
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0 = [src0.B1, src0.B1]
//! dst1 = [src0.B1, src0.B1]
//! @endverbatim
static FOG_INLINE void b32_2x2ExpandB1(
  b32_1x2& dst0,
  b32_1x2& dst1, b32_1x2 src0)
{
  b32_1x2ExpandB1(dst0, src0);
  dst1 = dst0;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Saturate]
// ============================================================================

//! @brief
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! @endverbatim
static FOG_INLINE void b32_1x2Saturate(
  b32_1x2& dst0)
{
  dst0 |= BYTE1x2MASK_PLUS_ONE - ((dst0 >> 8) & BYTE1x2MASK);

  dst0 &= 0x00FF00FFU;
}

//! @brief
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! dst1 = min(dst1, 255)
//! @endverbatim
static FOG_INLINE void b32_2x2Saturate(
  b32_1x2& dst0, b32_1x2& dst1)
{
  dst0 |= BYTE1x2MASK_PLUS_ONE - ((dst0 >> 8) & BYTE1x2MASK);
  dst1 |= BYTE1x2MASK_PLUS_ONE - ((dst1 >> 8) & BYTE1x2MASK);

  dst0 &= 0x00FF00FFU;
  dst1 &= 0x00FF00FFU;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Add]
// ============================================================================

//! @brief
//!
//! @verbatim
//! dst0 = a0 + b0
//! @endverbatim
static FOG_INLINE void b32_1x2AddB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0)
{
  dst0 = a0 + b0;
}

//! @brief
//!
//! @verbatim
//! dst0 = a0 + b0
//! dst1 = a1 + b1
//! @endverbatim
static FOG_INLINE void b32_2x2AddB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1)
{
  dst0 = a0 + b0;
  dst1 = a1 + b1;
}

//! @brief
//!
//! @verbatim
//! dst0 = a0 + u
//! @endverbatim
static FOG_INLINE void b32_1x2AddU(
  b32_1x2& dst0, b32_1x2 a0, b32_1x1 u)
{
  b32_1x2ExtendB0(u, u);
  dst0 = a0 + u;
}

//! @brief
//!
//! @verbatim
//! dst0 = a0 + u
//! dst1 = a1 + u
//! @endverbatim
static FOG_INLINE void b32_2x2AddU(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x1 u)
{
  b32_1x2ExtendB0(u, u);
  dst0 = a0 + u;
  dst1 = a1 + u;
}

//! @brief
//!
//! @verbatim
//! dst0 = min(a0 + b0, 255)
//! @endverbatim
static FOG_INLINE void b32_1x2AddusB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0)
{
  dst0 = a0 + b0;
  b32_1x2Saturate(dst0);
}

//! @brief
//!
//! @verbatim
//! dst0 = min(a0 + b0, 255)
//! dst1 = min(a1 + b1, 255)
//! @endverbatim
static FOG_INLINE void b32_2x2AddusB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1)
{
  dst0 = a0 + b0;
  dst1 = a1 + b1;
  b32_2x2Saturate(dst0, dst1);
}

//! @brief
//!
//! @verbatim
//! dst0 = min(a0 + u, 255)
//! @endverbatim
static FOG_INLINE void b32_1x2AddusU(
  b32_1x2& dst0, b32_1x2 a0, b32_1x1 u)
{
  b32_1x2ExtendB0(u, u);
  dst0 = a0 + u;
  b32_1x2Saturate(dst0);
}

//! @brief
//!
//! @verbatim
//! dst0 = min(a0 + u, 255)
//! dst1 = min(a1 + u, 255)
//! @endverbatim
static FOG_INLINE void b32_2x2AddusU(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x1 u)
{
  b32_1x2ExtendB0(u, u);
  dst0 = a0 + u;
  dst1 = a1 + u;
  b32_2x2Saturate(dst0, dst1);
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Sub]
// ============================================================================

//! @brief Subtract @a b0 from @a a0 and store the result to @a dst0.
//!
//! @verbatim
//! dst0 = a0 - b0
//! @endverbatim
static FOG_INLINE void b32_1x2SubB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0)
{
  dst0 = a0 - b0;
}

//! @brief Subtract @a b0/b1 from @a a0/a1 and store the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = a0 - b0
//! dst1 = a1 - b1
//! @endverbatim
static FOG_INLINE void b32_2x2SubB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1)
{
  dst0 = a0 - b0;
  dst1 = a1 - b1;
}

//! @brief Subtract extended @a u from @a a0 and store the result to @a dst0.
//!
//! @verbatim
//! dst0 = a0 - u
//! @endverbatim
static FOG_INLINE void b32_1x2SubU(
  b32_1x2& dst0, b32_1x2 a0, uint32_t u)
{
  b32_1x2ExtendB0(u, u);
  dst0 = a0 - u;
}

//! @brief Subtract extended @a u from @a a0/a1 and store the result to 
//! @a dst0/dst1.
//!
//! @verbatim
//! dst0 = a0 - u
//! dst1 = a1 - u
//! @endverbatim
static FOG_INLINE void b32_2x2SubU(
  b32_1x2& dst0, uint32_t a0,
  b32_1x2& dst1, uint32_t a1, uint32_t u)
{
  b32_1x2ExtendB0(u, u);
  dst0 = a0 - u;
  dst1 = a1 - u;
}

//! @brief Subtract with unsigned saturation @a b0 from @a a0 and store
//! the result to @a dst0.
//!
//! @verbatim
//! dst0 = max(a0 - b0, 0)
//! @endverbatim
static FOG_INLINE void b32_1x2SubusB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0)
{
  dst0 = (a0 ^ BYTE1x2MASK) + b0;
  b32_1x2Saturate(dst0);
  dst0 ^= BYTE1x2MASK;
}

//! @brief Subtract with unsigned saturation @a b0/b1 from @a a0/a1 and 
//! store the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(a0 - b0, 0)
//! dst1 = max(a1 - b1, 0)
//! @endverbatim
static FOG_INLINE void b32_2x2SubusB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1)
{
  dst0 = (a0 ^ BYTE1x2MASK) + b0;
  dst1 = (a1 ^ BYTE1x2MASK) + b1;
  b32_2x2Saturate(dst0, dst1);
  dst0 ^= BYTE1x2MASK;
  dst1 ^= BYTE1x2MASK;
}

//! @brief Subtract with unsigned saturation extended @a u from @a a0 and 
//! store the result to @a dst0.
//!
//! @verbatim
//! dst0 = max(a0 - u, 0)
//! @endverbatim
static FOG_INLINE void b32_1x2SubusU(
  b32_1x2& dst0, b32_1x2 a0, uint32_t u)
{
  b32_1x2ExtendB0(u, u);
  dst0 = (a0 ^ BYTE1x2MASK) + u;
  b32_1x2Saturate(dst0);
  dst0 ^= BYTE1x2MASK;
}

//! @brief Subtract with unsigned saturation extended @a u from @a a0/a1 and 
//! store the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(a0 - u, 0)
//! dst1 = max(a1 - u, 0)
//! @endverbatim
static FOG_INLINE void b32_2x2SubusU(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t u)
{
  b32_1x2ExtendB0(u, u);
  dst0 = (a0 ^ BYTE1x2MASK) + u;
  dst1 = (a1 ^ BYTE1x2MASK) + u;
  b32_2x2Saturate(dst0, dst1);
  dst0 ^= BYTE1x2MASK;
  dst1 ^= BYTE1x2MASK;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - AddSub]
// ============================================================================

//! @brief Add @a b0 to @a a0, subtract @a c0 from it and store the result
//! to @a dst0.
//!
//! @verbatim
//! dst0 = a0 + b0 - c0
//! @endverbatim
static FOG_INLINE void b32_1x2AddSubB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0, b32_1x2 c0)
{
  dst0 = a0 + b0 - c0;
}

//! @brief Add @a b0/b1 to @a a0/a1, subtract @a c0/c1 from it and store 
//! the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = a0 + b0 - c0
//! dst1 = a1 + b1 - c1
//! @endverbatim
static FOG_INLINE void b32_2x2AddSubB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0, b32_1x2 c0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1, b32_1x2 c1)
{
  dst0 = a0 + b0 - c0;
  dst0 = a1 + b1 - c1;
}

//! @brief Add @a b0 to @a a0, subtract @a c0 from it, saturate and store 
//! the result to @a dst0.
//!
//! @verbatim
//! dst0 = saturate(a0 + b0 - c0)
//! @endverbatim
static FOG_INLINE void b32_1x2AddSubusB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0, b32_1x2 c0)
{
  dst0 = a0 + b0;

  uint32_t x0lo = (dst0 & 0x000001FFU);
  uint32_t x0hi = (dst0 & 0x01FF0000U) >> 16;

  x0lo -= (c0 & 0x0000FFFFU);
  x0hi -= (c0 & 0xFFFF0000U) >> 16;

  x0lo &= ((~x0lo) & 0xFFFF0000U) >> 16;
  x0hi &= ((~x0hi) & 0xFFFF0000U) >> 16;

  dst0 = x0lo | (x0hi << 16);
  b32_1x2Saturate(dst0);
}

//! @brief Add @a b0/b1 to @a a0/a1, subtract @a c0/c1 from it, saturate and 
//! store the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = saturate(a0 + b0 - c0)
//! dst1 = saturate(a1 + b1 - c1)
//! @endverbatim
static FOG_INLINE void b32_2x2AddSubusB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0, b32_1x2 c0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1, b32_1x2 c1)
{
  b32_1x2AddSubusB32_1x2(dst0, a0, b0, c0);
  b32_1x2AddSubusB32_1x2(dst1, a1, b1, c1);
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Mul]
// ============================================================================

//! @brief Multiply @a a0 with @a u and store the result to @a dst0.
//!
//! @verbatim
//! dst0 = a0 * u
//! @endverbatim
static FOG_INLINE void b32_1x2MulU(
  b32_1x2& dst0, b32_1x2 a0, b32_1x1 u)
{
  dst0 = a0 * u;
}

//! @brief Multiply @a a0/a1 with @a u and store the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = a0 * u
//! dst1 = a1 * u
//! @endverbatim
static FOG_INLINE void b32_2x2MulU(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x1 u)
{
  dst0 = a0 * u;
  dst1 = a1 * u;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - MulDiv]
// ============================================================================

//! @brief Multiply @a a0 with @a u, divide by 255 and store the result to
//! @a dst0.
//!
//! @verbatim
//! dst0 = (a0 * u) / 255
//! @endverbatim
static FOG_INLINE void b32_1x2MulDiv255U(
  b32_1x2& dst0, b32_1x2 a0, uint32_t u)
{
  dst0 = a0 * u; dst0 = ((dst0 + ((dst0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
}

//! @brief Multiply @a a0/a1 with @a u, divide by 255 and store the result
//! to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = (a0 * u) / 255
//! dst1 = (a1 * u) / 255
//! @endverbatim
static FOG_INLINE void b32_2x2MulDiv255U(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t u)
{
  dst0 = a0 * u; dst0 = ((dst0 + ((dst0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
  dst1 = a1 * u; dst1 = ((dst1 + ((dst1 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
}

//! @brief Multiply @a a0/a1 with @a u0/u1, divide by 255 and store the result 
//! to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = (a0 * u0) / 255
//! dst1 = (a1 * u1) / 255
//! @endverbatim
static FOG_INLINE void b32_2x2MulDiv255U(
  b32_1x2& dst0, b32_1x2 a0, uint32_t u0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t u1)
{
  dst0 = a0 * u0; dst0 = ((dst0 + ((dst0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
  dst1 = a1 * u1; dst1 = ((dst1 + ((dst1 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
}

//! @brief Multiply @a a0/a1 with @a u, divide by 255, pack and return as 
//! packed DWORD.
//!
//! @verbatim
//! Result = pack((a0 * u) / 255, (a1 * u) / 255)
//! @endverbatim
static FOG_INLINE uint32_t b32_2x2MulDiv255U_Pack0213(
  b32_1x2 a0,
  b32_1x2 a1, uint32_t u)
{
  a0 *= u;
  a0 = ((a0 + ((a0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & 0x00FF00FFU;

  a1 *= u;
  a1 = ((a1 + ((a1 >> 8) & BYTE1x2MASK) + BYTE1x2HALF)     ) & 0xFF00FF00U;

  return a0 | a1;
}

//! @brief Multiply @a a0/a1 with @a u0/u1, divide by 255, pack and return as 
//! packed DWORD.
//!
//! @verbatim
//! Result = pack((a0 * u0) / 255, (a1 * u1) / 255)
//! @endverbatim
static FOG_INLINE uint32_t b32_2x2MulDiv255U_Pack0213(
  b32_1x2 a0, uint32_t u0,
  b32_1x2 a1, uint32_t u1)
{
  a0 *= u0;
  a0 = ((a0 + ((a0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & 0x00FF00FFU;

  a1 *= u1;
  a1 = ((a1 + ((a1 >> 8) & BYTE1x2MASK) + BYTE1x2HALF)     ) & 0xFF00FF00U;

  return a0 | a1;
}

//! @brief Multiply @a a0 with @a b0, divide by 255, add @a c0 and store the 
//! result to @a dst0.
//!
//! @verbatim
//! dst0 = saturate((a0 * b0) / 255 + c0)
//! @endverbatim
static FOG_INLINE void b32_1x2MulDiv255U_AddusB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, uint32_t b0, b32_1x2 c0)
{
  b32_1x2MulDiv255U(dst0, a0, b0);
  b32_1x2AddusB32_1x2(dst0, dst0, c0);
}

//! @brief Multiply @a a0/a1 with @a b0/b1, divide by 255, add @a c0/c1 and 
//! store the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = saturate((a0 * b0) / 255 + c0)
//! dst1 = saturate((a1 * b1) / 255 + c1)
//! @endverbatim
static FOG_INLINE void b32_2x2MulDiv255U_AddusB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, uint32_t b0, b32_1x2 c0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t b1, b32_1x2 c1)
{
  b32_1x2MulDiv255U(dst0, a0, b0);
  b32_1x2MulDiv255U(dst1, a1, b1);
  b32_1x2AddusB32_1x2(dst0, dst0, c0);
  b32_1x2AddusB32_1x2(dst1, dst1, c1);
}

//! @brief Multiply @a a0 with @a b0, divide by 255 and store the result to 
//! @a dst0.
//! 
//! @verbatim
//! dst0 = (a0 * b0) / 255
//! @endverbatim
static FOG_INLINE void b32_1x2MulDiv255B32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0)
{
  dst0 = ((a0 & 0x000000FFU) * (b0 & 0x000000FFU)) | ((a0 & 0x00FF0000U) * ((b0 & 0x00FF0000U) >> 16));
  dst0 = ((dst0 + ((dst0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
}

//! @brief Multiply @a a0/a1 with @a b0/b1, divide by 255 and store the result 
//! to @a dst0/dst1.
//! 
//! @verbatim
//! dst0 = (a0 * b0) / 255
//! dst1 = (a1 * b1) / 255
//! @endverbatim
static FOG_INLINE void b32_2x2MulDiv255B32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1)
{
  dst0 = ((a0 & 0x000000FFU) * (b0 & 0x000000FFU)) | ((a0 & 0x00FF0000U) * ((b0 & 0x00FF0000U) >> 16));
  dst1 = ((a1 & 0x000000FFU) * (b1 & 0x000000FFU)) | ((a1 & 0x00FF0000U) * ((b1 & 0x00FF0000U) >> 16));
  dst0 = ((dst0 + ((dst0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
  dst1 = ((dst1 + ((dst1 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
}

//! @brief Multiply @a a0 with @a b0/b1, divide by 255, add @a c0 and store 
//! the result to @a dst0.
//! 
//! @verbatim
//! dst0 = (a0 * b0) / 255 + c0
//! @endverbatim
static FOG_INLINE void b32_1x2MulDiv255B32_1x2_AddB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0, b32_1x2 c0)
{
  b32_1x2MulDiv255B32_1x2(dst0, a0, b0);
  b32_1x2AddB32_1x2(dst0, dst0, c0);
}

//! @brief Multiply @a a0/a1 with @a b0/b1, divide by 255, add @a c0/c1 and 
//! store the result to @a dst0/dst1.
//! 
//! @verbatim
//! dst0 = (a0 * b0) / 255 + c0
//! dst1 = (a1 * b1) / 255 + c1
//! @endverbatim
static FOG_INLINE void b32_2x2MulDiv255B32_2x2_AddB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0, b32_1x2 c0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1, b32_1x2 c1)
{
  b32_2x2MulDiv255B32_2x2(dst0, a0, b0, dst1, a1, b1);
  b32_2x2AddB32_2x2(dst0, dst0, c0, dst1, dst1, c1);
}

//! @brief Multiply @a a0 with @a b0, divide by 255, add @a c0 and store the 
//! result to @a dst0.
//!
//! @verbatim
//! dst0 = (a0 * b0) / 255 + c0
//! dst1 = (a1 * b1) / 255 + c1
//! @endverbatim
static FOG_INLINE void b32_1x2MulDiv255B32_1x2_AddusB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0, b32_1x2 c0)
{
  b32_1x2MulDiv255B32_1x2(dst0, a0, b0);
  b32_1x2AddusB32_1x2(dst0, dst0, c0);
}

//! @brief Multiply @a a0/a1 with @a b0/b1, divide by 255, add @a c0/c1 and 
//! store the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = (a0 * b0) / 255 + c0
//! dst1 = (a1 * b1) / 255 + c1
//! @endverbatim
static FOG_INLINE void b32_2x2MulDiv255B32_2x2_AddusB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0, b32_1x2 c0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1, b32_1x2 c1)
{
  b32_2x2MulDiv255B32_2x2(dst0, a0, b0, dst1, a1, b1);
  b32_2x2AddusB32_2x2(dst0, dst0, c0, dst1, dst1, c1);
}

//! @brief Multiply @a a0 with @a b0, @a c0 with @a d0, add, divide by 255 and 
//! store the result to @a dst0.
//!
//! @verbatim
//! dst0 = (a0 * b0 + c0 * d0) / 255
//! @endverbatim
static FOG_INLINE void b32_1x2MulDiv255U_2x_Add(
  b32_1x2& dst0, b32_1x2 a0, uint32_t b0, b32_1x2 c0, uint32_t d0)
{
  dst0 = a0 * b0 + c0 * d0; dst0 = ((dst0 + ((dst0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
}

//! @brief Multiply @a a0/a1 with @a b0/b1, @a c0/c1 with @a d0/d1, add, divide by 255 and 
//! store the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = (a0 * b0 + c0 * d0) / 255
//! dst1 = (a1 * b1 + c1 * d1) / 255
//! @endverbatim
static FOG_INLINE void b32_2x2MulDiv255U_2x_Add(
  b32_1x2& dst0, b32_1x2 a0, uint32_t b0, b32_1x2 c0, uint32_t d0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t b1, b32_1x2 c1, uint32_t d1)
{
  dst0 = a0 * b0 + c0 * d0; dst0 = ((dst0 + ((dst0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
  dst1 = a1 * b1 + c1 * d1; dst1 = ((dst1 + ((dst1 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
}

//! @brief Multiply @a a0 with @a b0, @a c0 with @a d0, add using saturation, 
//! divide by 255 and store the result to @a dst0.
//!
//! @verbatim
//! dst0 = saturate((a0 * b0) / 255) + (c0 * d0) / 255)
//! @endverbatim
static FOG_INLINE void b32_1x2MulDiv255U_2x_Addus(
  b32_1x2& dst0, b32_1x2 a0, uint32_t b0, b32_1x2 c0, uint32_t d0)
{
  b32_1x2MulDiv255U(a0, a0, b0);
  b32_1x2MulDiv255U(c0, c0, d0);

  b32_1x2AddusB32_1x2(dst0, a0, c0);
}

//! @brief Multiply @a a0/a1 with @a b0/b1, @a c0/c1 with @a d0/d1, add using 
//! saturation, divide by 255 and store the result to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = saturate((a0 * b0) / 255) + (c0 * d0) / 255)
//! dst1 = saturate((a1 * b1) / 255) + (c1 * d1) / 255)
//! @endverbatim
static FOG_INLINE void b32_2x2MulDiv255U_2x_Addus(
  b32_1x2& dst0, b32_1x2 a0, uint32_t b0, b32_1x2 c0, uint32_t d0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t b1, b32_1x2 c1, uint32_t d1)
{
  b32_2x2MulDiv255U(a0, a0, b0, a1, a1, b1);
  b32_2x2MulDiv255U(c0, c0, d0, c1, c1, d1);

  b32_2x2AddusB32_2x2(dst0, a0, c0, dst1, a1, c1);
}

//! @brief Multiply @a a0 with @a u, divide by 256 and store the result to
//! @a dst0.
//!
//! @verbatim
//! dst0 = (a0 * u) / 256
//! @endverbatim
static FOG_INLINE void b32_1x2MulDiv256U(
  b32_1x2& dst0, b32_1x2 a0, uint32_t u)
{
  dst0 = ((a0 * u) >> 8) & BYTE1x2MASK;
}

//! @brief Multiply @a a0/a1 with @a u, divide by 256 and store the result to
//! @a dst0/dst1.
//!
//! @verbatim
//! dst0 = (a0 * u) / 256
//! dst1 = (a1 * u) / 256
//! @endverbatim
static FOG_INLINE void b32_2x2MulDiv256U(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t u)
{
  dst0 = ((a0 * u) >> 8) & BYTE1x2MASK;
  dst0 = ((a1 * u) >> 8) & BYTE1x2MASK;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Min / Max]
// ============================================================================

//! @brief Take smaller value from @a a0 and @a u and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(a0, u)
//! @endverbatim
static FOG_INLINE void b32_1x2MinU(
  b32_1x2& dst0, b32_1x2 a0, uint32_t u)
{
  uint32_t a0lo = a0 & 0x000000FFU;
  if (a0lo > u) a0lo = u;

  u <<= 16;

  uint32_t a0hi = a0 & 0x00FF0000U;
  if (a0hi > u) a0hi = u;

  dst0 = a0lo | a0hi;
}

//! @brief Take smaller value from @a a0/a1 and @a u and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(a0, u)
//! dst1 = min(a1, u)
//! @endverbatim
static FOG_INLINE void b32_2x2MinU(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t u)
{
  uint32_t a0lo = a0 & 0x000000FFU;
  uint32_t a1lo = a1 & 0x000000FFU;

  if (a0lo > u) a0lo = u;
  if (a1lo > u) a1lo = u;

  u <<= 16;

  uint32_t a0hi = a0 & 0x00FF0000U;
  uint32_t a1hi = a1 & 0x00FF0000U;

  if (a0hi > u) a0hi = u;
  if (a1hi > u) a1hi = u;

  dst0 = a0lo | a0hi;
  dst1 = a1lo | a1hi;
}

//! @brief Take smaller value from @a a0 and @a b0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(a0, b0)
//! @endverbatim
static FOG_INLINE void b32_1x2MinB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0)
{
  uint32_t a0lo = a0 & 0x000000FFU;
  uint32_t b0lo = b0 & 0x000000FFU;

  uint32_t a0hi = a0 & 0x00FF0000U;
  uint32_t b0hi = b0 & 0x00FF0000U;

  if (a0lo > b0lo) a0lo = b0lo;
  if (a0hi > b0hi) a0hi = b0hi;

  dst0 = a0lo | a0hi;
}

//! @brief Take smaller value from @a a0/a1 and @a b0/b1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(a0, b0)
//! dst1 = min(a1, b1)
//! @endverbatim
static FOG_INLINE void b32_2x2MinB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1)
{
  b32_1x2MinB32_1x2(dst0, a0, b0);
  b32_1x2MinB32_1x2(dst1, a1, b1);
}

//! @brief Take larger value from @a a0 and @a u and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(a0, u);
//! @endverbatim
static FOG_INLINE void b32_1x2MaxU(
  b32_1x2& dst0, b32_1x2 a0, uint32_t u)
{
  uint32_t a0lo = a0 & 0x000000FFU;
  uint32_t a0hi = a0 & 0x00FF0000U;

  if (a0lo < u) a0lo = u;
  if (a0hi < u) a0hi = u;

  dst0 = a0lo | a0hi;
}

//! @brief Take larger value from @a a0/a1 and @a u and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(a0, u)
//! dst1 = max(a1, u)
//! @endverbatim
static FOG_INLINE void b32_2x2MaxU(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t u)
{
  b32_1x2MaxU(dst0, a0, u);
  b32_1x2MaxU(dst1, a1, u);
}

//! @brief Take larger value from @a a0 and @a b0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(a0, b0);
//! @endverbatim
static FOG_INLINE void b32_1x2MaxB32_1x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0)
{
  uint32_t a0lo = a0 & 0x000000FFU;
  uint32_t a0hi = a0 & 0x00FF0000U;

  uint32_t b0lo = b0 & 0x000000FFU;
  uint32_t b0hi = b0 & 0x00FF0000U;

  // I hope this will be compiled using cmov...
  if (a0lo < b0lo) a0lo = b0lo;
  if (a0hi < b0hi) a0hi = b0hi;

  dst0 = a0lo | a0hi;
}

//! @brief Take larger value from @a a0/a1 and @a b0/b1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(a0, b0)
//! dst1 = max(a1, b1)
//! @endverbatim
static FOG_INLINE void b32_2x2MaxB32_2x2(
  b32_1x2& dst0, b32_1x2 a0, b32_1x2 b0,
  b32_1x2& dst1, b32_1x2 a1, b32_1x2 b1)
{
  b32_1x2MaxB32_1x2(dst0, a0, b0);
  b32_1x2MaxB32_1x2(dst1, a1, b1);
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Shift]
// ============================================================================

static FOG_INLINE void b32_1x2ShiftLeftU(
  b32_1x2& dst0, uint32_t a0, uint32_t u)
{
  dst0 = a0 << u;
}

static FOG_INLINE void b32_2x2ShiftLeftU(
  b32_1x2& dst0, b32_1x2& a0,
  b32_1x2& dst1, b32_1x2& a1, uint32_t u)
{
  dst0 = a0 << u;
  dst1 = a1 << u;
}

static FOG_INLINE void b32_1x2ShiftLeft1(
  b32_1x2& dst0, b32_1x2 a0)
{
  dst0 = a0 + a0;
}

static FOG_INLINE void b32_2x2ShiftLeft1(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1)
{
  dst0 = a0 + a0;
  dst1 = a1 + a1;
}

static FOG_INLINE void b32_1x2ShiftLeft1_B0(
  b32_1x2& dst0, b32_1x2 a0)
{
  dst0 = a0 + (a0 & 0x000000FFU);
}

static FOG_INLINE void b32_2x2ShiftLeft1_B0(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1)
{
  dst0 = a0 + (a0 & 0x000000FFU);
  dst1 = a1 + (a1 & 0x000000FFU);
}

static FOG_INLINE void b32_1x2ShiftLeft1_B1(
  b32_1x2& dst0, b32_1x2 a0)
{
  dst0 = a0 + (a0 & 0x00FF0000U);
}

static FOG_INLINE void b32_2x2ShiftLeft1_B1(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1)
{
  dst0 = a0 + (a0 & 0x00FF0000U);
  dst1 = a1 + (a1 & 0x00FF0000U);
}

static FOG_INLINE void b32_1x2ShiftRightU(
  b32_1x2& dst0, b32_1x2 a0, uint32_t u)
{
  dst0 = a0 >> u;
}

static FOG_INLINE void b32_2x2ShiftRightU(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t u)
{
  dst0 = a0 >> u;
  dst1 = a1 >> u;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Negate]
// ============================================================================

static FOG_INLINE void b32_1x2Negate(
  b32_1x2& dst0, uint32_t a0)
{
  dst0 = a0 ^ 0x00FF00FFU;
}

static FOG_INLINE void b32_2x2Negate(
  b32_1x2& dst0, uint32_t a0,
  b32_1x2& dst1, uint32_t a1)
{
  dst0 = a0 ^ 0x00FF00FFU;
  dst1 = a1 ^ 0x00FF00FFU;
}

static FOG_INLINE void b32_1x2Negate_B0(
  b32_1x2& dst0, uint32_t a0)
{
  dst0 = a0 ^ 0x000000FFU;
}

static FOG_INLINE void b32_2x2Negate_B0(
  b32_1x2& dst0, uint32_t a0,
  b32_1x2& dst1, uint32_t a1)
{
  dst0 = a0 ^ 0x000000FFU;
  dst1 = a1 ^ 0x000000FFU;
}

static FOG_INLINE void b32_1x2Negate_B1(
  b32_1x2& dst0, uint32_t a0)
{
  dst0 = a0 ^ 0x00FF0000U;
}

static FOG_INLINE void b32_2x2Negate_B1(
  b32_1x2& dst0, uint32_t a0,
  b32_1x2& dst1, uint32_t a1)
{
  dst0 = a0 ^ 0x00FF0000U;
  dst1 = a1 ^ 0x00FF0000U;
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Premultiply]
// ============================================================================

//! @brief Premultiply the pixel stored in @a a0 and @a a1 and store the result
//! to @a dst0 and @a dst1.
static FOG_INLINE void b32_2x2PremultiplyU(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t u)
{
  dst0 = a0 * u;
  dst1 = (a1 & 0xFF) * u;

  u <<= 16;

  dst0 = ((dst0 + ((dst0 >> 8) & BYTE1x2MASK) + BYTE1x2HALF) >> 8) & BYTE1x2MASK;
  dst1 = ((dst1 +  (dst1 >> 8)                + BYTE1x1HALF) >> 8);
  dst1 |= u;
}

//! @brief Premultiply the pixel stored in @a a0 and @a a1 and store the result
//! to @a dst0 and @a dst1.
//!
//! The alpha component is in [a1.B1].
static FOG_INLINE void b32_2x2PremultiplyA(
  b32_1x2& dst0, b32_1x2 a0,
  b32_1x2& dst1, b32_1x2 a1)
{
  b32_2x2PremultiplyU(dst0, a0, dst1, a1, b32_1x2GetB1(a1));
}

// ============================================================================
// [Fog::ByteSIMD - B32_1x2 - Interpolate]
// ============================================================================

static FOG_INLINE void b32_2x2InterpolateU255(
  b32_1x2& dst0, b32_1x2 a0, uint32_t b0, b32_1x2 c0, uint32_t d0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t b1, b32_1x2 c1, uint32_t d1)
{
  dst0 = a0 * b0 + c0 * d0;
  dst1 = a1 * b1 + c1 * d1;

  dst0 = dst0 + ((dst0 >> 8) & 0x00FF00FFU) + 0x00800080U;
  dst1 = dst1 + ((dst1 >> 8) & 0x00FF00FFU) + 0x00800080U;

  dst0 &= BYTE1x2MASK;
  dst1 &= BYTE1x2MASK;
}

static FOG_INLINE void b32_2x2InterpolateU256(
  b32_1x2& dst0, b32_1x2 a0, uint32_t b0, b32_1x2 c0, uint32_t d0,
  b32_1x2& dst1, b32_1x2 a1, uint32_t b1, b32_1x2 c1, uint32_t d1)
{
  dst0 = a0 * b0 + c0 * d0;
  dst1 = a1 * b1 + c1 * d1;

  dst0 >>= 8;
  dst1 >>= 8;

  dst0 &= BYTE1x2MASK;
  dst1 &= BYTE1x2MASK;
}

static FOG_INLINE uint32_t b32_2x2InterpolateU255_Pack0213(
  b32_1x2 a0, uint32_t b0, b32_1x2 c0, uint32_t d0,
  b32_1x2 a1, uint32_t b1, b32_1x2 c1, uint32_t d1)
{
  a0 = a0 * b0 + c0 * d0;
  a1 = a1 * b1 + c1 * d1;

  a0 = (a0 + ((a0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  a1 = (a1 + ((a1 >> 8) & 0x00FF00FFU) + 0x00800080U) << 8;

  a0 &= 0x00FF00FFU;
  a1 &= 0xFF00FF00U;

  return a0 | a1;
}

static FOG_INLINE uint32_t b32_2x2InterpolateU256_Pack0213(
  b32_1x2 a0, uint32_t b0, b32_1x2 c0, uint32_t d0,
  b32_1x2 a1, uint32_t b1, b32_1x2 c1, uint32_t d1)
{
  a0 = a0 * b0 + c0 * d0;
  a1 = a1 * b1 + c1 * d1;

  a0 >>= 8;

  a0 &= 0x00FF00FFU;
  a1 &= 0xFF00FF00U;

  return a0 | a1;
}

// ============================================================================
// [Fog::ByteSIMD - B64_1x4 - Pack]
// ============================================================================

//! @brief Pack b64_1x4 value into DWORD.
static FOG_INLINE void b64_1x4Pack0213(uint32_t& dst0, b64_1x4 x0)
{
  dst0 = (uint32_t)(x0 | (x0 >> 24));
}

// ============================================================================
// [Fog::ByteSIMD - B64_1x4 - Unpack]
// ============================================================================

//! @brief Unpack all bytes of DWORD into one b64_1x4 value.
static FOG_INLINE void b64_1x4Unpack0213(
  b64_1x4& dst0, uint32_t a0)
{
  dst0 = ((b64_1x4)a0 | ((b64_1x4)a0 << 24)) & BYTE_1x4MASK;
}

//! @brief Unpack '021' bytes of DWORD into one b64_1x4 value.
static FOG_INLINE void b64_1x4Unpack021X(
  b64_1x4& dst0, uint32_t a0)
{
  dst0 = ((b64_1x4)a0 | ((b64_1x4)a0 << 24)) & BYTE_1x3MASK;
}

// ============================================================================
// [Fog::ByteSIMD - P32 - MulDiv255]
// ============================================================================

// NOTE: These functions are included here, because there are sometimes useful,
// but note that each function here needs first to unpack input values, do
// operation and them pack result back. This can be overhead in some situations
// and you should consider to use pure b32_1x2 or b64_1x4 solution instead.

// x_c' = (x_c * a) / 255
static FOG_INLINE uint32_t p32MulDiv255U(uint32_t x, uint32_t a)
{
#if FOG_ARCH_BITS == 64
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = ((x & 0x00FF00FFU)     ) * a;
  uint32_t t1 = ((x & 0xFF00FF00U) >> 8) * a;

  x  = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  x |= ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U)     ) & 0xFF00FF00U;

  return x;
#endif
}

// x_c' = (x_c * a) / 255
// x_a' = 0x00
static FOG_INLINE uint32_t p32MulDiv255U_0XXX(uint32_t x, uint32_t a)
{
#if FOG_ARCH_BITS == 64
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0x0000008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x000000FF00FF00FF);
  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = ((x & 0x00FF00FFU)     ) * a;
  uint32_t t1 = ((x & 0x0000FF00U) >> 8) * a;

  x  = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  x |= ((t1 + ((t1 >> 8) & 0x000000FFU) + 0x00000080U)     ) & 0x0000FF00U;

  return x;
#endif
}

// x_c' = (x_c * a) / 255
// x_a' = 0xFF
static FOG_INLINE uint32_t p32MulDiv255U_FXXX(uint32_t x, uint32_t a)
{
#if FOG_ARCH_BITS == 64
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x000000FF00FF00FF)) + FOG_UINT64_C(0xFF00008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = ((x & 0x00FF00FFU)     ) * a;
  uint32_t t1 = ((x & 0x0000FF00U) >> 8) * a;

  x  = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  x |= ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0xFF000080U)     ) & 0xFF00FF00U;

  return x;
#endif
}

// ============================================================================
// [Fog::ByteSIMD - P32 - Add]
// ============================================================================

static FOG_INLINE uint32_t p32AddP32(uint32_t x, uint32_t y)
{
  return x + y;
}

// x_c' = min(x_c + y_c, 255)
static FOG_INLINE uint32_t p32AddusP32(uint32_t x, uint32_t y)
{
#if FOG_ARCH_BITS == 64
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t y0 = ((uint64_t)y | ((uint64_t)y << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  x0 += y0;
  x0 |= FOG_UINT64_C(0x0100010001000100) - ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = (x & 0x00FF00FFU);
  uint32_t t1 = (x & 0xFF00FF00U) >> 8;

  t0 += (y & 0x00FF00FFU);
  t1 += (y & 0xFF00FF00U) >> 8;

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  return t0 | (t1 << 8);
#endif
}

// x_c' = (x_c * a) / 255 + y
static FOG_INLINE uint32_t p32MulDiv255U_AddusP32(uint32_t x, uint32_t a, uint32_t y)
{
#if FOG_ARCH_BITS == 64
  uint64_t x0 = ((uint64_t)x | ((uint64_t)x << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t y0 = ((uint64_t)y | ((uint64_t)y << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  x0 *= a;
  x0 = (x0 + ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  x0 += y0;
  x0 |= FOG_UINT64_C(0x0100010001000100) - ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  x0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  return (uint32_t)(x0 | (x0 >> 24));
#else
  uint32_t t0 = ((x & 0x00FF00FFU)     ) * a;
  uint32_t t1 = ((x & 0xFF00FF00U) >> 8) * a;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8) & 0x00FF00FFU;

  t0 += (y & 0x00FF00FFU);
  t1 += (y & 0xFF00FF00U) >> 8;

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  return t0 | (t1 << 8);
#endif
}

// ============================================================================
// [Fog::ByteSIMD - P32 - MulDiv]
// ============================================================================

static FOG_INLINE uint32_t p32MulDiv255P32(
  uint32_t x0, uint32_t a0)
{
  uint32_t t0 = ((x0 & 0x00FF00FFU)     ) * a0;
  uint32_t t1 = ((x0 & 0xFF00FF00U) >> 8) * a0;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U);

  return ((t0 & 0xFF00FF00U) >> 8) + (t1 & 0xFF00FF00U);
}

static FOG_INLINE uint32_t p32_2x4MulDiv255U32_Add(
  uint32_t x0, uint32_t a0,
  uint32_t y0, uint32_t b0)
{
  uint32_t t0 = ((x0 & 0x00FF00FFU)     ) * a0 + ((y0 & 0x00FF00FFU)     ) * b0;
  uint32_t t1 = ((x0 & 0xFF00FF00U) >> 8) * a0 + ((y0 & 0xFF00FF00U) >> 8) * b0;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U);

  return ((t0 & 0xFF00FF00U) >> 8) + (t1 & 0xFF00FF00U);
}

static FOG_INLINE uint32_t p32_2x4MulDiv256U32_Add(
  uint32_t x0, uint32_t a0,
  uint32_t y0, uint32_t b0)
{
  uint32_t t0 = ((x0 & 0x00FF00FFU)     ) * a0 + ((y0 & 0x00FF00FFU)     ) * b0;
  uint32_t t1 = ((x0 & 0xFF00FF00U) >> 8) * a0 + ((y0 & 0xFF00FF00U) >> 8) * b0;

  return ((t0 & 0xFF00FF00U) >> 8) + (t1 & 0xFF00FF00U);
}

//! @}

} // ByteSIMD namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_BYTESIMD_P_H