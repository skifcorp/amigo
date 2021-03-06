
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __java_awt_font_GlyphMetrics__
#define __java_awt_font_GlyphMetrics__

#pragma interface

#include <java/lang/Object.h>
extern "Java"
{
  namespace java
  {
    namespace awt
    {
      namespace font
      {
          class GlyphMetrics;
      }
      namespace geom
      {
          class Rectangle2D;
      }
    }
  }
}

class java::awt::font::GlyphMetrics : public ::java::lang::Object
{

public:
  GlyphMetrics(jboolean, jfloat, jfloat, ::java::awt::geom::Rectangle2D *, jbyte);
  GlyphMetrics(jfloat, ::java::awt::geom::Rectangle2D *, jbyte);
  jfloat getAdvance();
  jfloat getAdvanceX();
  jfloat getAdvanceY();
  ::java::awt::geom::Rectangle2D * getBounds2D();
  jfloat getLSB();
  jfloat getRSB();
  jint getType();
  jboolean isCombining();
  jboolean isComponent();
  jboolean isLigature();
  jboolean isStandard();
  jboolean isWhitespace();
  static const jbyte COMBINING = 2;
  static const jbyte COMPONENT = 3;
  static const jbyte LIGATURE = 1;
  static const jbyte STANDARD = 0;
  static const jbyte WHITESPACE = 4;
private:
  jboolean __attribute__((aligned(__alignof__( ::java::lang::Object)))) horizontal;
  jfloat advanceX;
  jfloat advanceY;
  ::java::awt::geom::Rectangle2D * bounds;
  jbyte glyphType;
public:
  static ::java::lang::Class class$;
};

#endif // __java_awt_font_GlyphMetrics__
