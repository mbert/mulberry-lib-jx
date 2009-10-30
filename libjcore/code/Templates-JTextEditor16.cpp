#include <JCoreStdInc.h>
#include <JTextEditor16.h>
#include <JTEUndoBase16.h>

#define JTemplateType JTextEditor16::Font
#include <JArray.tmpl>
#include <JRunArray.tmpls>
#undef JTemplateType

#define JTemplateType JTextEditor16::Font,JArray<JTextEditor16::Font>
#include <JStack.tmpls>
#undef JTemplateType

#define JTemplateType JTextEditor16::LineGeometry
#include <JRunArray.tmpls>
#undef JTemplateType

#define JTemplateType JTEUndoBase16
#include <JPtrArray.tmpls>
#undef JTemplateType

#define JTemplateType JTextEditor16::CRMRule
#include <JArray.tmpls>
#undef JTemplateType
