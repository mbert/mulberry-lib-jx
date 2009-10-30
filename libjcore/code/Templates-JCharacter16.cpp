#include <JCoreStdInc.h>
#include <jTypes.h>
#define JTemplateType JCharacter16
#include <JArray.tmpls>

#undef JTemplateType
#define JTemplateType JCharacter16,JArray<JCharacter16>
#include <JStack.tmpls>
