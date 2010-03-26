// [Fog-Graphics Library - Public API]
//
// [Licence]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_FONTENGINE_FREETYPE_H
#define _FOG_GRAPHICS_FONTENGINE_FREETYPE_H

// [Dependencies]
#include <Fog/Core/Lock.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/FontEngine.h>

#if defined(FOG_FONT_FREETYPE)

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

struct FTFontEngine;
struct FTFontFace;
struct FTFontFile;

// ============================================================================
// [Fog::FTFontFace]
// ============================================================================

//! @brief Font face used by FreeType font engine.
//!
//! @note This class is platform specific.
//!
//! @sa @c FTFontEngine.
struct FOG_API FTFontFace : public FontFace
{
  FTFontFace();
  virtual ~FTFontFace();

  virtual err_t getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet);
  virtual err_t getOutline(const Char* str, sysuint_t length, Path& dst);
  virtual err_t getTextExtents(const Char* str, sysuint_t length, TextExtents& extents);

  //! @brief Glyph cache lock.
  mutable Lock lock;
  //! @brief Glyph cache.
  GlyphCache glyphCache;
  //! @brief FreeType file reference.
  FTFontFile* file;

private:
  GlyphData* renderGlyph(uint32_t uc);

  FOG_DISABLE_COPY(FTFontFace)
};

// ============================================================================
// [Fog::FTFontEngine]
// ============================================================================

//! @brief FreeType font engine.
//!
//! @note This class is platform specific.
//!
//! @sa @c FTFontFace.
struct FOG_API FTFontEngine : public FontEngine
{
  FTFontEngine();
  virtual ~FTFontEngine();

  virtual List<String> getFontList();

  virtual FontFace* createDefaultFace();

  virtual FontFace* createFace(
    const String& family, uint32_t size, 
    const FontCaps& caps);

  virtual List<String> getDefaultFontDirectories();

protected:
  void close();

  Hash<String, FTFontFile*> _ftCache;

private:
  FOG_DISABLE_COPY(FTFontEngine)
};

} // Fog namespace

//! @}

#else
#warning "Fog::FontEngine::FreeType support not enabled but header file included!"
#endif // FOG_FONT_FREETYPE

// [Guard]
#endif // _FOG_GRAPHICS_FONTENGINE_FREETYPE_H
