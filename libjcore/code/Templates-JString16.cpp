#include <JCoreStdInc.h>
#include <JString16.h>

#define JTemplateType JString16

#define JPtrArrayCopy
#include <JPtrArray.tmpls>
#undef JPtrArrayCopy

#ifdef _TODO
#define JString16PtrMapCopy
#include <JString16PtrMap.tmpls>
#undef JString16PtrMapCopy
#endif

#undef JTemplateType

#define JTemplateDataType    JString16
#define JTemplateStorageType JArray<JString16*>

#define JPtrStackCopy
#include <JPtrStack.tmpls>
#undef JPtrStackCopy

#define JPtrQueueCopy
#include <JPtrQueue.tmpls>
#undef JPtrQueueCopy

#undef JTemplateDataType
#undef JTemplateStorageType

#ifdef _TODO
#define JTemplateType JStrValue<JString16*>
#include <JHashTable.tmpls>
#undef JTemplateType
#endif

#define JTemplateType JString16*
#include <JAliasArray.tmpls>
#undef JTemplateType
