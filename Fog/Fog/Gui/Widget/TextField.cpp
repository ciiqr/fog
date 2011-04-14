// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Text/TextRect.h>
#include <Fog/Gui/Widget/TextField.h>

FOG_IMPLEMENT_OBJECT(Fog::TextField)

namespace Fog {

// ============================================================================
// [Fog::TextField]
// ============================================================================

TextField::TextField(uint32_t createFlags) :
  Frame(createFlags)
{
  setFrameStyle(FRAME_TEXT_AREA);
}

TextField::~TextField()
{
}

void TextField::setValue(const String& value)
{
  _value = value;
  update(WIDGET_UPDATE_PAINT);
}

SizeI TextField::getSizeHint() const
{
  return SizeI(80, 20);
}

void TextField::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();
  SizeI s = getClientSize();
  RectI bounds(0, 0, s.w, s.h);

  p->setSource(Argb32(0xFFEEEEEE));
  p->clear();

  p->setSource(Argb32(0xFF000000));
  p->fillText(TextRectI(bounds, TEXT_ALIGN_LEFT | TEXT_ALIGN_VCENTER), _value, _font);
}

} // Fog namespace