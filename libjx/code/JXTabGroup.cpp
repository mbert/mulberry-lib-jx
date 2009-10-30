/******************************************************************************
 JXTabGroup.cpp

	Displays a list of tabs along any one of the four edges.  The tabs
	select items in a JXCardFile.  The tabs can be scrolled with the mouse
	wheel.

	Please use this class responsibly.  If there are more tabs than will
	fit along the edge, consider using JXStringList or JXTreeListWidget
	instead.

	BASE CLASS = JXWidget

	Copyright © 2002-04 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTabGroup.h>
#include <JXCardFile.h>
#include <JXTextMenu.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <JXFontManager.h>
#include <JXColormap.h>
#include <jGlobals.h>
#include <jStreamUtil.h>
#include <jTime.h>
#include <jAssert.h>

const JCoordinate kBorderWidth = kJXDefaultBorderWidth;
const JCoordinate kTextMargin  = 4;
const JCoordinate kSelMargin   = 2;
const JCoordinate kArrowWidth  = 15;

const JFloat kInitialScrollDelay    = 0.2;
const JFloat kContinuousScrollDelay = 0.05;

// setup information

const JFileVersion kCurrentSetupVersion = 0;
const JCharacter kSetupDataEndDelimiter = '\1';

// Context menu

static const JCharacter* kContextMenuStr =
	"    Show on top"
	"  | Show on bottom"
	"%l| Show at left"
	"  | Show at right";

enum
{
	kContextTopCmd = 1,
	kContextBottomCmd,
	kContextLeftCmd,
	kContextRightCmd
};

// JBroadcaster message types

const JCharacter* JXTabGroup::kAppearanceChanged = "AppearanceChanged::JXTabGroup";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTabGroup::JXTabGroup
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsEdge(kTop),
	itsFontName(JGetDefaultFontName()),
	itsFontSize(kJDefaultFontSize),
	itsCanScrollUpFlag(kJFalse),
	itsCanScrollDownFlag(kJFalse),
	itsFirstDrawIndex(1),
	itsLastDrawIndex(1),
	itsContextMenu(NULL),
	itsDragAction(kInvalidClick),
	itsScrollUpPushedFlag(kJFalse),
	itsScrollDownPushedFlag(kJFalse)
{
	itsTitles = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsTitles != NULL );

	itsTabRects = new JArray<JRect>;
	assert( itsTabRects != NULL );

	itsCardFile = new JXCardFile(this, kHElastic, kVElastic, 0,0, 100,100);
	assert( itsCardFile != NULL );
	PlaceCardFile();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTabGroup::~JXTabGroup()
{
	delete itsTitles;
	delete itsTabRects;
}

/******************************************************************************
 ShowTab

	If the current card is willing to disappear, then we show the requested
	one and return kJTrue.  Otherwise, we return kJFalse.

 ******************************************************************************/

JBoolean
JXTabGroup::ShowTab
	(
	JXWidgetSet* card
	)
{
	if (itsCardFile->ShowCard(card))
		{
		JIndex i;
		if (GetCurrentTabIndex(&i) && itsFirstDrawIndex > i)
			{
			itsFirstDrawIndex = i;
			}

		Refresh();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JXTabGroup::ShowTab
	(
	const JIndex index
	)
{
	if (itsCardFile->ShowCard(index))
		{
		JIndex i;
		const JBoolean hasIndex = GetCurrentTabIndex(&i);
		if (hasIndex && i < itsFirstDrawIndex)
			{
			itsFirstDrawIndex = i;
			}
		else if (hasIndex && i > itsFirstDrawIndex)
			{
			ScrollUpToTab(i);
			}

		Refresh();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 RemoveTab

	Removes the specified tab and returns a pointer to the card so you can
	keep track of it.

 ******************************************************************************/

JXWidgetSet*
JXTabGroup::RemoveTab
	(
	const JIndex index
	)
{
	JIndex selIndex;
	const JBoolean hasSelection = itsCardFile->GetCurrentCardIndex(&selIndex);
	if (hasSelection && index == selIndex && index > 1)
		{
		ShowTab(index-1);
		}
	else if (hasSelection && index == selIndex && index < GetTabCount())
		{
		ShowTab(index+1);
		}

	Refresh();
	itsTitles->DeleteElement(index);
	return itsCardFile->RemoveCard(index);
}

/******************************************************************************
 PlaceCardFile (private)

 ******************************************************************************/

void
JXTabGroup::PlaceCardFile()
{
	const JSize h = kSelMargin + kBorderWidth + 2*kTextMargin +
					(GetFontManager())->GetLineHeight(itsFontName, itsFontSize, itsFontStyle);

	JRect r = GetAperture();
	if (itsEdge == kTop)
		{
		r.top += h;
		}
	else if (itsEdge == kLeft)
		{
		r.left += h;
		}
	else if (itsEdge == kBottom)
		{
		r.bottom -= h;
		}
	else if (itsEdge == kRight)
		{
		r.right -= h;
		}
	else
		{
		assert( 0 );
		}

	r.Shrink(kBorderWidth, kBorderWidth);
	itsCardFile->Place(r.left, r.top);
	itsCardFile->SetSize(r.width(), r.height());
}

/******************************************************************************
 ScrollUpToTab (private)

 ******************************************************************************/

void
JXTabGroup::ScrollUpToTab
	(
	const JIndex index
	)
{
	assert( itsTitles->IndexValid(index) );
	assert( index > itsFirstDrawIndex );

	const JFontManager* fontMgr = GetFontManager();
	const JFontID fontID        = fontMgr->GetFontID(itsFontName, itsFontSize, itsFontStyle);

	const JCoordinate scrollArrowWidth = 2*(kArrowWidth + kBorderWidth);

	const JRect ap        = GetAperture();
	const JCoordinate min = (itsEdge == kTop || itsEdge == kBottom ? ap.left : ap.top);
	const JCoordinate max = (itsEdge == kTop || itsEdge == kBottom ? ap.right : ap.bottom);
	JCoordinate left      = min + kSelMargin;
	JCoordinate right     = left;
	JArray<JCoordinate> widthList;

	const JSize count  = itsTitles->GetElementCount();
	JBoolean offScreen = kJFalse;
	for (JIndex i=itsFirstDrawIndex; i<=index; i++)
		{
		right += 2*(kBorderWidth + kTextMargin) +
				 fontMgr->GetStringWidth(fontID, itsFontSize, itsFontStyle,
										 *(itsTitles->NthElement(i)));
		widthList.AppendElement(right - left);

		if (!offScreen &&
			right >= max - scrollArrowWidth &&
			!(itsFirstDrawIndex == 1 && i == count && right <= max))
			{
			offScreen = kJTrue;
			}

		left = right;
		}

	if (offScreen)
		{
		JIndex i = 1;
		while (right > max - scrollArrowWidth && itsFirstDrawIndex < index)
			{
			right -= widthList.GetElement(i);
			itsFirstDrawIndex++;
			i++;
			}
		}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXTabGroup::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect ap = GetAperture();

	p.SetFont(itsFontName, itsFontSize, itsFontStyle);
	const JSize lineHeight = p.GetLineHeight();
	const JSize tabHeight  = 2*(kBorderWidth + kTextMargin) + lineHeight;

	JIndex selIndex;
	JRect selRect;
	const JBoolean hasSelection = itsCardFile->GetCurrentCardIndex(&selIndex);

	itsTabRects->RemoveAll();
	itsCanScrollUpFlag   = JI2B(itsFirstDrawIndex > 1);
	itsCanScrollDownFlag = kJFalse;

	const JCoordinate scrollArrowWidth = 2*(kArrowWidth + kBorderWidth);

	const JSize count = itsTitles->GetElementCount();
	itsLastDrawIndex  = JMax(count, itsFirstDrawIndex);

	if (itsEdge == kTop)
		{
		JRect r(ap.top + kSelMargin,             ap.left + kSelMargin,
				ap.top + kSelMargin + tabHeight, ap.left + kSelMargin);

		for (JIndex i=itsFirstDrawIndex; i<=count; i++)
			{
			const JString* title = itsTitles->NthElement(i);
			const JBoolean isSel = JI2B(hasSelection && i == selIndex);

			r.right += 2*(kBorderWidth + kTextMargin) + p.GetStringWidth(*title);
			JPoint titlePt(r.left + kBorderWidth + kTextMargin,
						   r.top  + kBorderWidth + kTextMargin);
			if (isSel)
				{
//				titlePt.y -= kSelMargin;
				r.top     -= kSelMargin;
				r.Expand(kSelMargin, 0);

				selRect = r;
				}

			if (!isSel)
				{
				DrawTabBorder(p, r, kJFalse);
				}
			p.JPainter::String(titlePt, *title);

			itsTabRects->AppendElement(r);
			if (isSel)
				{
				r.top += kSelMargin;
				r.Shrink(kSelMargin, 0);
				}

			if (r.right >= ap.right - scrollArrowWidth)
				{
				if (itsFirstDrawIndex == 1 && i == count && r.right <= ap.right)
					{
					break;
					}
				itsCanScrollDownFlag = JI2B( itsFirstDrawIndex < count );
				itsLastDrawIndex     = i;
				if (r.right > ap.right - scrollArrowWidth && i > itsFirstDrawIndex)
					{
					itsLastDrawIndex--;
					}
				break;
				}

			r.left = r.right;
			}
		}

	else if (itsEdge == kLeft)
		{
		JRect r(ap.bottom - kSelMargin, ap.left + kSelMargin,
				ap.bottom - kSelMargin, ap.left + kSelMargin + tabHeight);

		for (JIndex i=itsFirstDrawIndex; i<=count; i++)
			{
			const JString* title = itsTitles->NthElement(i);
			const JBoolean isSel = JI2B(hasSelection && i == selIndex);

			r.top -= 2*(kBorderWidth + kTextMargin) + p.GetStringWidth(*title);
			JPoint titlePt(r.left   + kBorderWidth + kTextMargin,
						   r.bottom - kBorderWidth - kTextMargin);
			if (isSel)
				{
//				titlePt.x -= kSelMargin;
				r.left    -= kSelMargin;
				r.Expand(0, kSelMargin);

				selRect = r;
				}

			if (!isSel)
				{
				DrawTabBorder(p, r, kJFalse);
				}
			p.JPainter::String(90, titlePt, *title);

			itsTabRects->AppendElement(r);
			if (isSel)
				{
				r.left += kSelMargin;
				r.Shrink(0, kSelMargin);
				}

			if (r.top <= ap.top + scrollArrowWidth)
				{
				if (itsFirstDrawIndex == 1 && i == count && r.top >= ap.top)
					{
					break;
					}
				itsCanScrollDownFlag = JI2B( itsFirstDrawIndex < count );
				itsLastDrawIndex     = i;
				if (r.top < ap.top + scrollArrowWidth && i > itsFirstDrawIndex)
					{
					itsLastDrawIndex--;
					}
				break;
				}

			r.bottom = r.top;
			}
		}

	else if (itsEdge == kBottom)
		{
		JRect r(ap.bottom - kSelMargin - tabHeight, ap.left + kSelMargin,
				ap.bottom - kSelMargin,             ap.left + kSelMargin);

		for (JIndex i=itsFirstDrawIndex; i<=count; i++)
			{
			const JString* title = itsTitles->NthElement(i);
			const JBoolean isSel = JI2B(hasSelection && i == selIndex);

			r.right += 2*(kBorderWidth + kTextMargin) + p.GetStringWidth(*title);
			JPoint titlePt(r.left + kBorderWidth + kTextMargin,
						   r.top  + kBorderWidth + kTextMargin);
			if (isSel)
				{
//				titlePt.y += kSelMargin;
				r.bottom  += kSelMargin;
				r.Expand(kSelMargin, 0);

				selRect = r;
				}

			if (!isSel)
				{
				DrawTabBorder(p, r, kJFalse);
				}
			p.JPainter::String(titlePt, *title);

			itsTabRects->AppendElement(r);
			if (isSel)
				{
				r.bottom -= kSelMargin;
				r.Shrink(kSelMargin, 0);
				}

			if (r.right >= ap.right - scrollArrowWidth)
				{
				if (itsFirstDrawIndex == 1 && i == count && r.right <= ap.right)
					{
					break;
					}
				itsCanScrollDownFlag = JI2B( itsFirstDrawIndex < count );
				itsLastDrawIndex     = i;
				if (r.right > ap.right - scrollArrowWidth && i > itsFirstDrawIndex)
					{
					itsLastDrawIndex--;
					}
				break;
				}

			r.left = r.right;
			}
		}

	else if (itsEdge == kRight)
		{
		JRect r(ap.top + kSelMargin, ap.right - kSelMargin - tabHeight,
				ap.top + kSelMargin, ap.right - kSelMargin);

		for (JIndex i=itsFirstDrawIndex; i<=count; i++)
			{
			const JString* title = itsTitles->NthElement(i);
			const JBoolean isSel = JI2B(hasSelection && i == selIndex);

			r.bottom += 2*(kBorderWidth + kTextMargin) + p.GetStringWidth(*title);
			JPoint titlePt(r.right - kBorderWidth - kTextMargin,
						   r.top   + kBorderWidth + kTextMargin);
			if (isSel)
				{
//				titlePt.x += kSelMargin;
				r.right   += kSelMargin;
				r.Expand(0, kSelMargin);

				selRect = r;
				}

			if (!isSel)
				{
				DrawTabBorder(p, r, kJFalse);
				}
			p.JPainter::String(-90, titlePt, *title);

			itsTabRects->AppendElement(r);
			if (isSel)
				{
				r.right -= kSelMargin;
				r.Shrink(0, kSelMargin);
				}

			if (r.bottom >= ap.bottom - scrollArrowWidth)
				{
				if (itsFirstDrawIndex == 1 && i == count && r.bottom <= ap.bottom)
					{
					break;
					}
				itsCanScrollDownFlag = JI2B( itsFirstDrawIndex < count );
				itsLastDrawIndex     = i;
				if (r.bottom > ap.bottom - scrollArrowWidth && i > itsFirstDrawIndex)
					{
					itsLastDrawIndex--;
					}
				break;
				}

			r.top = r.bottom;
			}
		}

	JRect r = itsCardFile->GetFrame();
	r.Expand(kBorderWidth, kBorderWidth);
	JXDrawUpFrame(p, r, kBorderWidth);

	if (!selRect.IsEmpty())
		{
		DrawTabBorder(p, selRect, kJTrue);
		}

	DrawScrollButtons(p, lineHeight);
}

/******************************************************************************
 DrawTabBorder (private)

 ******************************************************************************/

void
JXTabGroup::DrawTabBorder
	(
	JXWindowPainter&	p,
	const JRect&		rect,
	const JBoolean		isSelected
	)
{
	JXDrawUpFrame(p, rect, kBorderWidth);

	const JColormap* cmap = p.GetColormap();
	if (itsEdge == kTop)
		{
		p.SetPenColor(cmap->GetDefaultBackColor());
		p.JPainter::Point(rect.topLeft());
		p.JPainter::Point(rect.topRight() + JPoint(-1,0));
		p.JPainter::Point(rect.topRight() + JPoint(-2,0));
		p.JPainter::Point(rect.topRight() + JPoint(-1,1));
		p.JPainter::Point(rect.topRight() + JPoint(-1,2));
		p.SetPenColor(cmap->Get3DLightColor());
		p.JPainter::Point(rect.topLeft() + JPoint(kBorderWidth, kBorderWidth));
		p.SetPenColor(cmap->Get3DShadeColor());
		p.JPainter::Point(rect.topRight() + JPoint(-kBorderWidth-1, kBorderWidth));

		if (isSelected)
			{
			JRect r(rect.bottom - kBorderWidth, rect.left  + kBorderWidth,
					rect.bottom,                rect.right - kBorderWidth);
			p.SetPenColor(cmap->GetDefaultBackColor());
			p.JPainter::Rect(r);
			p.JPainter::Point(rect.topLeft() + JPoint(-1,kSelMargin+kBorderWidth));
			p.SetPenColor(cmap->Get3DLightColor());
			p.JPainter::Point(rect.bottomLeft()  + JPoint(1,-1));
			p.JPainter::Point(rect.bottomRight() + JPoint(-2,-1));
			p.JPainter::Point(rect.bottomRight() + JPoint(-1,-2));
			p.JPainter::Point(rect.bottomRight() + JPoint(-1,-1));
			}
		}

	else if (itsEdge == kLeft)
		{
		p.SetPenColor(cmap->GetDefaultBackColor());
		p.JPainter::Point(rect.topLeft());
		p.JPainter::Point(rect.bottomLeft() + JPoint(0,-1));
		p.JPainter::Point(rect.bottomLeft() + JPoint(0,-2));
		p.JPainter::Point(rect.bottomLeft() + JPoint(1,-1));
		p.JPainter::Point(rect.bottomLeft() + JPoint(2,-1));
		p.SetPenColor(cmap->Get3DLightColor());
		p.JPainter::Point(rect.topLeft() + JPoint(kBorderWidth, kBorderWidth));
		p.SetPenColor(cmap->Get3DShadeColor());
		p.JPainter::Point(rect.bottomLeft() + JPoint(kBorderWidth, -kBorderWidth-1));

		if (isSelected)
			{
			JRect r(rect.top    + kBorderWidth, rect.right - kBorderWidth,
					rect.bottom - kBorderWidth, rect.right);
			p.SetPenColor(cmap->GetDefaultBackColor());
			p.JPainter::Rect(r);
			p.JPainter::Point(rect.topLeft() + JPoint(kSelMargin+kBorderWidth,-1));
			p.SetPenColor(cmap->Get3DLightColor());
			p.JPainter::Point(rect.topRight() + JPoint(-1,1));
			if (rect.bottom < (GetAperture()).bottom)
				{
				p.JPainter::Point(rect.bottomRight() + JPoint(-1,-1));
				p.JPainter::Point(rect.bottomRight() + JPoint(-2,-1));
				p.JPainter::Point(rect.bottomRight() + JPoint(-1,-2));
				}
			}
		}

	else if (itsEdge == kBottom)
		{
		p.SetPenColor(cmap->GetDefaultBackColor());
		p.JPainter::Point(rect.bottomLeft()  + JPoint(0,-1));
		p.JPainter::Point(rect.bottomLeft()  + JPoint(0,-2));
		p.JPainter::Point(rect.bottomLeft()  + JPoint(1,-1));
		p.JPainter::Point(rect.bottomLeft()  + JPoint(2,-1));
		p.JPainter::Point(rect.bottomRight() + JPoint(-1,-1));
		p.SetPenColor(cmap->Get3DLightColor());
		p.JPainter::Point(rect.bottomLeft() + JPoint(kBorderWidth, -kBorderWidth-1));
		p.SetPenColor(cmap->Get3DShadeColor());
		p.JPainter::Point(rect.bottomRight() + JPoint(-kBorderWidth-1, -kBorderWidth-1));

		if (isSelected)
			{
			JRect r(rect.top,                rect.left  + kBorderWidth,
					rect.top + kBorderWidth, rect.right - kBorderWidth);
			p.SetPenColor(cmap->GetDefaultBackColor());
			p.JPainter::Rect(r);
			p.SetPenColor(cmap->Get3DShadeColor());
			if (rect.left > (GetAperture()).left)
				{
				p.JPainter::Point(rect.topLeft());
				}
			p.JPainter::Point(rect.topRight() + JPoint(-1,0));
			p.JPainter::Point(rect.topRight() + JPoint(-2,0));
			p.JPainter::Point(rect.topRight() + JPoint(-2,1));
			}
		}

	else if (itsEdge == kRight)
		{
		p.SetPenColor(cmap->GetDefaultBackColor());
		p.JPainter::Point(rect.topRight() + JPoint(-1,0));
		p.JPainter::Point(rect.topRight() + JPoint(-2,0));
		p.JPainter::Point(rect.topRight() + JPoint(-1,1));
		p.JPainter::Point(rect.topRight() + JPoint(-1,2));
		p.JPainter::Point(rect.bottomRight() + JPoint(-1,-1));
		p.SetPenColor(cmap->Get3DLightColor());
		p.JPainter::Point(rect.topRight() + JPoint(-kBorderWidth-1, kBorderWidth));
		p.SetPenColor(cmap->Get3DShadeColor());
		p.JPainter::Point(rect.bottomRight() + JPoint(-kBorderWidth-1, -kBorderWidth-1));

		if (isSelected)
			{
			JRect r(rect.top    + kBorderWidth, rect.left,
					rect.bottom - kBorderWidth, rect.left + kBorderWidth);
			p.SetPenColor(cmap->GetDefaultBackColor());
			p.JPainter::Rect(r);
			p.SetPenColor(cmap->Get3DShadeColor());
			if (rect.top > (GetAperture()).top)
				{
				p.JPainter::Point(rect.topLeft());
				}
			p.JPainter::Point(rect.bottomLeft() + JPoint(0,-1));
			p.JPainter::Point(rect.bottomLeft() + JPoint(0,-2));
			p.JPainter::Point(rect.bottomLeft() + JPoint(1,-2));
			}
		}
}

/******************************************************************************
 DrawScrollButtons (private)

 ******************************************************************************/

void
JXTabGroup::DrawScrollButtons
	(
	JXWindowPainter&	p,
	const JCoordinate	lineHeight
	)
{
	itsScrollUpRect = itsScrollDownRect = JRect(0,0,0,0);
	if (!itsCanScrollUpFlag && !itsCanScrollDownFlag)
		{
		return;
		}

	const JCoordinate w = 2*(kArrowWidth + kBorderWidth);
	const JCoordinate h = kSelMargin + kBorderWidth + 2*kTextMargin + lineHeight;

	const JRect ap        = GetAperture();
	const JColormap* cmap = p.GetColormap();
	if (itsEdge == kTop)
		{
		JRect r(ap.top,     ap.right - w,
				ap.top + h, ap.right);

		p.SetPenColor(cmap->GetDefaultBackColor());
		p.SetFilling(kJTrue);
		p.JPainter::Rect(r);
		p.SetFilling(kJFalse);

		r.Shrink(kBorderWidth, 0);
		r.top = r.bottom = r.ycenter();
		r.top    -= kArrowWidth/2;
		r.bottom += kArrowWidth/2 + kArrowWidth%2;

		r.right = r.left + kArrowWidth;
		if (itsCanScrollUpFlag && itsScrollUpPushedFlag)
			{
			JXDrawDownArrowLeft(p, r, kBorderWidth);
			}
		else if (itsCanScrollUpFlag)
			{
			JXDrawUpArrowLeft(p, r, kBorderWidth);
			}
		else
			{
			JXFillArrowLeft(p, r, cmap->GetInactiveLabelColor());
			}
		itsScrollUpRect = r;

		r.left  += kArrowWidth;
		r.right += kArrowWidth;
		if (itsCanScrollDownFlag && itsScrollDownPushedFlag)
			{
			JXDrawDownArrowRight(p, r, kBorderWidth);
			}
		else if (itsCanScrollDownFlag)
			{
			JXDrawUpArrowRight(p, r, kBorderWidth);
			}
		else
			{
			JXFillArrowRight(p, r, cmap->GetInactiveLabelColor());
			}
		itsScrollDownRect = r;
		}

	else if (itsEdge == kLeft)
		{
		JRect r(ap.top,     ap.left,
				ap.top + w, ap.left + h);

		p.SetPenColor(cmap->GetDefaultBackColor());
		p.SetFilling(kJTrue);
		p.JPainter::Rect(r);
		p.SetFilling(kJFalse);

		r.Shrink(0, kBorderWidth);
		r.left = r.right = r.xcenter();
		r.left  -= kArrowWidth/2;
		r.right += kArrowWidth/2 + kArrowWidth%2;

		r.bottom = r.top + kArrowWidth;
		if (itsCanScrollDownFlag && itsScrollDownPushedFlag)
			{
			JXDrawDownArrowUp(p, r, kBorderWidth);
			}
		else if (itsCanScrollDownFlag)
			{
			JXDrawUpArrowUp(p, r, kBorderWidth);
			}
		else
			{
			JXFillArrowUp(p, r, cmap->GetInactiveLabelColor());
			}
		itsScrollDownRect = r;

		r.top    += kArrowWidth;
		r.bottom += kArrowWidth;
		if (itsCanScrollUpFlag && itsScrollUpPushedFlag)
			{
			JXDrawDownArrowDown(p, r, kBorderWidth);
			}
		else if (itsCanScrollUpFlag)
			{
			JXDrawUpArrowDown(p, r, kBorderWidth);
			}
		else
			{
			JXFillArrowDown(p, r, cmap->GetInactiveLabelColor());
			}
		itsScrollUpRect = r;
		}

	else if (itsEdge == kBottom)
		{
		JRect r(ap.bottom - h, ap.right - w,
				ap.bottom,     ap.right);

		p.SetPenColor(cmap->GetDefaultBackColor());
		p.SetFilling(kJTrue);
		p.JPainter::Rect(r);
		p.SetFilling(kJFalse);

		r.Shrink(kBorderWidth, 0);
		r.top = r.bottom = r.ycenter();
		r.top    -= kArrowWidth/2;
		r.bottom += kArrowWidth/2 + kArrowWidth%2;

		r.right = r.left + kArrowWidth;
		if (itsCanScrollUpFlag && itsScrollUpPushedFlag)
			{
			JXDrawDownArrowLeft(p, r, kBorderWidth);
			}
		else if (itsCanScrollUpFlag)
			{
			JXDrawUpArrowLeft(p, r, kBorderWidth);
			}
		else
			{
			JXFillArrowLeft(p, r, cmap->GetInactiveLabelColor());
			}
		itsScrollUpRect = r;

		r.left  += kArrowWidth;
		r.right += kArrowWidth;
		if (itsCanScrollDownFlag && itsScrollDownPushedFlag)
			{
			JXDrawDownArrowRight(p, r, kBorderWidth);
			}
		else if (itsCanScrollDownFlag)
			{
			JXDrawUpArrowRight(p, r, kBorderWidth);
			}
		else
			{
			JXFillArrowRight(p, r, cmap->GetInactiveLabelColor());
			}
		itsScrollDownRect = r;
		}

	else if (itsEdge == kRight)
		{
		JRect r(ap.bottom - w, ap.right - h,
				ap.bottom,     ap.right);

		p.SetPenColor(cmap->GetDefaultBackColor());
		p.SetFilling(kJTrue);
		p.JPainter::Rect(r);
		p.SetFilling(kJFalse);

		r.Shrink(0, kBorderWidth);
		r.left = r.right = r.xcenter();
		r.left  -= kArrowWidth/2;
		r.right += kArrowWidth/2 + kArrowWidth%2;

		r.bottom = r.top + kArrowWidth;
		if (itsCanScrollUpFlag && itsScrollUpPushedFlag)
			{
			JXDrawDownArrowUp(p, r, kBorderWidth);
			}
		else if (itsCanScrollUpFlag)
			{
			JXDrawUpArrowUp(p, r, kBorderWidth);
			}
		else
			{
			JXFillArrowUp(p, r, cmap->GetInactiveLabelColor());
			}
		itsScrollUpRect = r;

		r.top    += kArrowWidth;
		r.bottom += kArrowWidth;
		if (itsCanScrollDownFlag && itsScrollDownPushedFlag)
			{
			JXDrawDownArrowDown(p, r, kBorderWidth);
			}
		else if (itsCanScrollDownFlag)
			{
			JXDrawUpArrowDown(p, r, kBorderWidth);
			}
		else
			{
			JXFillArrowDown(p, r, cmap->GetInactiveLabelColor());
			}
		itsScrollDownRect = r;
		}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXTabGroup::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXTabGroup::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsDragAction = kInvalidClick;

	if (button == kJXLeftButton && itsScrollUpRect.Contains(pt))
		{
		if (itsCanScrollUpFlag && itsFirstDrawIndex > 1)	// avoid left click when arrow disabled
			{
			itsDragAction         = kScrollUp;
			itsScrollUpPushedFlag = kJTrue;
			itsFirstDrawIndex--;
			Refresh();
			ScrollWait(kInitialScrollDelay);
			itsScrollUpPushedFlag = kJFalse;		// ignore first HandleMouseDrag()
			}
		}
	else if (button == kJXLeftButton && itsScrollDownRect.Contains(pt))
		{
		if (itsCanScrollDownFlag && itsFirstDrawIndex < GetTabCount())	// avoid left click when arrow disabled
			{
			itsDragAction           = kScrollDown;
			itsScrollDownPushedFlag = kJTrue;
			itsFirstDrawIndex++;
			Refresh();
			ScrollWait(kInitialScrollDelay);
			itsScrollDownPushedFlag = kJFalse;		// ignore first HandleMouseDrag()
			}
		}
	else if (button == kJXLeftButton)
		{
		const JSize count = itsTabRects->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JRect r = itsTabRects->GetElement(i);
			if (r.Contains(pt))
				{
				ShowTab(itsFirstDrawIndex + i-1);
				break;
				}
			}
		}
	else if (button == kJXRightButton)
		{
		CreateContextMenu();
		itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
		}
	else if (button == kJXButton4 && itsCanScrollUpFlag && itsFirstDrawIndex > 1)
		{
		itsFirstDrawIndex--;
		Refresh();
		}
	else if (button == kJXButton5 && itsCanScrollDownFlag && itsFirstDrawIndex < GetTabCount())
		{
		itsFirstDrawIndex++;
		Refresh();
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXTabGroup::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragAction == kScrollUp)
		{
		const JBoolean newScrollUpPushedFlag = JI2B( itsScrollUpRect.Contains(pt) );
		if (itsScrollUpPushedFlag != newScrollUpPushedFlag)
			{
			itsScrollUpPushedFlag = newScrollUpPushedFlag;
			Redraw();
			}
		else if (itsScrollUpPushedFlag &&
				 itsCanScrollUpFlag && itsFirstDrawIndex > 1)
			{
			itsFirstDrawIndex--;
			Refresh();
			ScrollWait(kContinuousScrollDelay);
			}
		}
	else if (itsDragAction == kScrollDown)
		{
		const JBoolean newScrollDownPushedFlag = JI2B( itsScrollDownRect.Contains(pt) );
		if (itsScrollDownPushedFlag != newScrollDownPushedFlag)
			{
			itsScrollDownPushedFlag = newScrollDownPushedFlag;
			Redraw();
			}
		else if (itsScrollDownPushedFlag &&
				 itsCanScrollDownFlag && itsFirstDrawIndex < GetTabCount())
			{
			itsFirstDrawIndex++;
			Refresh();
			ScrollWait(kContinuousScrollDelay);
			}
		}
}

/******************************************************************************
 HandleMouseUp

 ******************************************************************************/

void
JXTabGroup::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsScrollUpPushedFlag = itsScrollDownPushedFlag = kJFalse;
	Refresh();
}

/******************************************************************************
 ScrollWait

 ******************************************************************************/

void
JXTabGroup::ScrollWait
	(
	const JFloat delta
	)
	const
{
	(GetWindow())->Update();
	JWait(delta);
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Mouseover a tab displays the tab.

 ******************************************************************************/

JBoolean
JXTabGroup::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const Time			time,
	const JXWidget*		source
	)
{
	*action = (GetDNDManager())->GetDNDActionPrivateXAtom();
	return kJTrue;
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
JXTabGroup::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	const JSize count = itsTabRects->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JRect r = itsTabRects->GetElement(i);
		if (r.Contains(pt))
			{
			ShowTab(itsFirstDrawIndex + i-1);
			break;
			}
		}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXTabGroup::Activate()
{
	JXWidget::Activate();

	JIndex i;
	if (IsActive() && GetTabCount() > 0 && !GetCurrentTabIndex(&i))
		{
		ShowTab(1);
		}
}

/******************************************************************************
 Set font info

 ******************************************************************************/

void
JXTabGroup::SetFontName
	(
	const JCharacter* name
	)
{
	if (name != itsFontName)
		{
		itsFontName = name;
		UpdateAppearance();
		}
}

void
JXTabGroup::SetFontSize
	(
	const JSize size
	)
{
	if (size != itsFontSize)
		{
		itsFontSize = size;
		UpdateAppearance();
		}
}

void
JXTabGroup::SetFontStyle
	(
	const JFontStyle& style
	)
{
	if (style != itsFontStyle)
		{
		itsFontStyle = style;
		UpdateAppearance();
		}
}

void
JXTabGroup::SetFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style
	)
{
	itsFontName  = name;
	itsFontSize  = size;
	itsFontStyle = style;
	UpdateAppearance();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXTabGroup::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsContextMenu && message.Is(JXTextMenu::kNeedsUpdate))
		{
		UpdateContextMenu();
		}
	else if (sender == itsContextMenu && message.Is(JXTextMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleContextMenu(selection->GetIndex());
		}

	else
		{
		JXWidget::Receive(sender, message);
		}
}

/******************************************************************************
 CreateContextMenu (private)

 ******************************************************************************/

void
JXTabGroup::CreateContextMenu()
{
	if (itsContextMenu == NULL)
		{
		itsContextMenu = new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsContextMenu != NULL );
		itsContextMenu->SetMenuItems(kContextMenuStr, "JXTabGroup");
		itsContextMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsContextMenu->SetToHiddenPopupMenu();
		ListenTo(itsContextMenu);
		}
}

/******************************************************************************
 UpdateContextMenu (private)

 ******************************************************************************/

void
JXTabGroup::UpdateContextMenu()
{
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
JXTabGroup::HandleContextMenu
	(
	const JIndex index
	)
{
	if (index == kContextTopCmd)
		{
		SetTabEdge(kTop);
		}
	else if (index == kContextBottomCmd)
		{
		SetTabEdge(kBottom);
		}
	else if (index == kContextLeftCmd)
		{
		SetTabEdge(kLeft);
		}
	else if (index == kContextRightCmd)
		{
		SetTabEdge(kRight);
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
JXTabGroup::ReadSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
		{
		long edge;
		input >> edge;
		SetTabEdge((Edge) edge);
		}

	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 SkipSetup (static)

 ******************************************************************************/

void
JXTabGroup::SkipSetup
	(
	istream& input
	)
{
	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXTabGroup::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << (long) itsEdge;
	output << kSetupDataEndDelimiter;
}
