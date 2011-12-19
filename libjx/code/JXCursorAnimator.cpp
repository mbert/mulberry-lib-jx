/******************************************************************************
 JXCursorAnimator.cpp

	This class simplifies the animation of cursors.

	During a long process, call NextFrame() regularly, and this class will
	insure that the cursor twirls smoothly.  The default animation is the
	a beach ball.  You can change this with SetFrameSequence().

	We are owned by a particular JXWidget.  This object must activate us
	when it controls the cursor, and must deactivate us otherwise.  This
	allows an unlimited number of simultaneously animated cursors, with
	at most one visible at any one time.

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXCursorAnimator.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCursorAnimator::JXCursorAnimator
	(
	JXWindow* window
	)
{
	itsWindow     = window;
	itsActiveFlag = kJFalse;

	InitFrames(itsWindow->GetDisplay());
	ResetFrameCounter();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCursorAnimator::~JXCursorAnimator()
{
	delete itsFrames;
}

/******************************************************************************
 SetFrameSequence

 ******************************************************************************/

void
JXCursorAnimator::SetFrameSequence
	(
	const JArray<JCursorIndex>& frames
	)
{
	*itsFrames = frames;
	ResetFrameCounter();
}

/******************************************************************************
 NextFrame

	Show the next frame of the cursor -if enough time has elapsed-.

 ******************************************************************************/

void
JXCursorAnimator::NextFrame()
{
	const Time currentTime = (JXGetApplication())->GetCurrentTime();
	if (itsActiveFlag && currentTime - itsLastUpdateTime >= kmsecPerFrame)
		{
		itsFrameCounter++;
		if (itsFrameCounter > itsFrames->GetElementCount())
			{
			ResetFrameCounter();
			}

		itsWindow->DisplayXCursor(itsFrames->GetElement(itsFrameCounter));
		itsLastUpdateTime = currentTime;
		}
}

/******************************************************************************
 InitFrames (private)

	We start with the Macintosh spinning beachball.

 ******************************************************************************/

#ifdef _MSC_VER
#pragma warning ( disable : 4305 4309 )
#endif

static const JCharacter* kBeachballFrameName[] =
{
	"JXBeachballFrame1", "JXBeachballFrame2",
	"JXBeachballFrame3", "JXBeachballFrame4"
};

static char beachball_1_bits[] = {
   0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x30, 0x1f, 0x00, 0x08, 0x3f, 0x00,
   0x04, 0x7f, 0x00, 0x04, 0x7f, 0x00, 0x02, 0xff, 0x00, 0x02, 0xff, 0x00,
   0xfe, 0xff, 0x00, 0xfe, 0x81, 0x00, 0xfe, 0x81, 0x00, 0xfc, 0x41, 0x00,
   0xfc, 0x41, 0x00, 0xf8, 0x21, 0x00, 0xf0, 0x19, 0x00, 0xc0, 0x07, 0x00,
   0x00, 0x00, 0x00};

static char beachball_2_bits[] = {
   0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x30, 0x18, 0x00, 0x08, 0x20, 0x00,
   0x1c, 0x70, 0x00, 0x3c, 0x78, 0x00, 0x7e, 0xfc, 0x00, 0xfe, 0xfe, 0x00,
   0xfe, 0xff, 0x00, 0xfe, 0xfe, 0x00, 0x7e, 0xfc, 0x00, 0x3c, 0x78, 0x00,
   0x1c, 0x70, 0x00, 0x08, 0x20, 0x00, 0x30, 0x18, 0x00, 0xc0, 0x07, 0x00,
   0x00, 0x00, 0x00};

static char beachball_3_bits[] = {
   0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0xf0, 0x19, 0x00, 0xf8, 0x21, 0x00,
   0xfc, 0x41, 0x00, 0xfc, 0x41, 0x00, 0xfe, 0x81, 0x00, 0xfe, 0x81, 0x00,
   0xfe, 0xff, 0x00, 0x02, 0xff, 0x00, 0x02, 0xff, 0x00, 0x04, 0x7f, 0x00,
   0x04, 0x7f, 0x00, 0x08, 0x3f, 0x00, 0x30, 0x1f, 0x00, 0xc0, 0x07, 0x00,
   0x00, 0x00, 0x00};

static char beachball_4_bits[] = {
   0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0xf0, 0x1f, 0x00, 0xf8, 0x3f, 0x00,
   0xf4, 0x5f, 0x00, 0xe4, 0x4f, 0x00, 0xc2, 0x87, 0x00, 0x82, 0x83, 0x00,
   0x02, 0x81, 0x00, 0x82, 0x83, 0x00, 0xc2, 0x87, 0x00, 0xe4, 0x4f, 0x00,
   0xf4, 0x5f, 0x00, 0xf8, 0x3f, 0x00, 0xf0, 0x1f, 0x00, 0xc0, 0x07, 0x00,
   0x00, 0x00, 0x00};

static char beachball_mask_bits[] = {
   0xc0, 0x07, 0x00, 0xf0, 0x1f, 0x00, 0xf8, 0x3f, 0x00, 0xfc, 0x7f, 0x00,
   0xfe, 0xff, 0x00, 0xfe, 0xff, 0x00, 0xff, 0xff, 0x01, 0xff, 0xff, 0x01,
   0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0xfe, 0xff, 0x00,
   0xfe, 0xff, 0x00, 0xfc, 0x7f, 0x00, 0xf8, 0x3f, 0x00, 0xf0, 0x1f, 0x00,
   0xc0, 0x07, 0x00};

static const JXCursor kBeachBallFrames[] =
{
	{ 17,17, 8,8, beachball_1_bits, beachball_mask_bits },
	{ 17,17, 8,8, beachball_2_bits, beachball_mask_bits },
	{ 17,17, 8,8, beachball_3_bits, beachball_mask_bits },
	{ 17,17, 8,8, beachball_4_bits, beachball_mask_bits }
};	

const JSize kBeachBallFrameCount = sizeof(kBeachBallFrames) / sizeof(JXCursor);

void
JXCursorAnimator::InitFrames
	(
	JXDisplay* display
	)
{
	itsFrames = new JArray<JCursorIndex>(kBeachBallFrameCount);
	assert( itsFrames != NULL );

	for (JIndex i=0; i<kBeachBallFrameCount; i++)
		{
		const JCursorIndex cursIndex =
			display->CreateCustomCursor(kBeachballFrameName[i],
										kBeachBallFrames[i]);
		itsFrames->AppendElement(cursIndex);
		}
}
