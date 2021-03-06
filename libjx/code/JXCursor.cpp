/******************************************************************************
 JXCursor.cpp

	Useful cursors that are not provided by X.

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXCursor.h>
#include <JXDisplay.h>

/******************************************************************************
 Invisible cursor

 ******************************************************************************/

static const JCharacter* kInvisibleCursorName = "JXInvisible";

static char invisible_cursor_bits[]      = {0x00};
static char invisible_cursor_mask_bits[] = {0x00};

static const JXCursor kInvisibleCursor =
{
	1,1, 0,0, invisible_cursor_bits, invisible_cursor_mask_bits
};	

JCursorIndex
JXGetInvisibleCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kInvisibleCursorName, kInvisibleCursor);
}

/******************************************************************************
 Hand cursor

 ******************************************************************************/

#ifdef _MSC_VER
#pragma warning ( disable : 4305 4309 )
#endif

static const JCharacter* kHandCursorName = "JXHand";

static char hand_cursor_bits[] =
{
   0x80, 0x01, 0x70, 0x1A, 0x48, 0x26, 0x4A, 0x26,
   0x4D, 0x12, 0x49, 0x12, 0x09, 0x68, 0x01, 0x98,
   0x02, 0x88, 0x02, 0x40, 0x02, 0x20, 0x04, 0x20,
   0x04, 0x10, 0x08, 0x08, 0x08, 0x04, 0x08, 0x04
};

static char hand_cursor_mask_bits[] =
{
   0x80, 0x01, 0xF0, 0x1B, 0xF8, 0x3F, 0xFA, 0x3F,
   0xFF, 0x1F, 0xFF, 0x1F, 0xFF, 0x6F, 0xFF, 0xFF,
   0xFE, 0xFF, 0xFE, 0x7F, 0xFE, 0x3F, 0xFC, 0x3F,
   0xFC, 0x1F, 0xF8, 0x0F, 0xF8, 0x07, 0xF8, 0x07
};

static const JXCursor kHandCursor =
{
	16,16, 7,8, hand_cursor_bits, hand_cursor_mask_bits
};

JCursorIndex
JXGetHandCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kHandCursorName, kHandCursor);
}

/******************************************************************************
 Drag horiz line cursor

 ******************************************************************************/

static const JCharacter* kDragHorizLineCursorName = "JXDragHorizLine";

static char drag_horiz_line_cursor_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x80, 0x03, 0x00, 0x40, 0x05, 0x00,
   0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
   0xfe, 0xff, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
   0x00, 0x01, 0x00, 0x40, 0x05, 0x00, 0x80, 0x03, 0x00, 0x00, 0x01, 0x00,
   0x00, 0x00, 0x00};

static char drag_horiz_line_cursor_mask_bits[] = {
   0x80, 0x03, 0x00, 0xc0, 0x07, 0x00, 0xe0, 0x0f, 0x00, 0xe0, 0x0f, 0x00,
   0xe0, 0x0f, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0xff, 0xff, 0x01,
   0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00,
   0xe0, 0x0f, 0x00, 0xe0, 0x0f, 0x00, 0xe0, 0x0f, 0x00, 0xc0, 0x07, 0x00,
   0x80, 0x03, 0x00};

static const JXCursor kDragHorizLineCursor =
{
	17,17, 8,8, drag_horiz_line_cursor_bits, drag_horiz_line_cursor_mask_bits
};	

JCursorIndex
JXGetDragHorizLineCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragHorizLineCursorName,
									   kDragHorizLineCursor);
}

/******************************************************************************
 Drag all horiz line cursor

 ******************************************************************************/

static const JCharacter* kDragAllHorizLineCursorName = "JXDragAllHorizLine";

static char drag_all_horiz_line_cursor_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x80, 0x03, 0x00, 0x40, 0x05, 0x00,
   0x00, 0x01, 0x00, 0xfe, 0xff, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
   0xfe, 0xff, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0xfe, 0xff, 0x00,
   0x00, 0x01, 0x00, 0x40, 0x05, 0x00, 0x80, 0x03, 0x00, 0x00, 0x01, 0x00,
   0x00, 0x00, 0x00};

static char drag_all_horiz_line_cursor_mask_bits[] = {
   0x80, 0x03, 0x00, 0xc0, 0x07, 0x00, 0xe0, 0x0f, 0x00, 0xe0, 0x0f, 0x00,
   0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0xff, 0xff, 0x01,
   0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0xff, 0xff, 0x01,
   0xff, 0xff, 0x01, 0xe0, 0x0f, 0x00, 0xe0, 0x0f, 0x00, 0xc0, 0x07, 0x00,
   0x80, 0x03, 0x00};

static const JXCursor kDragAllHorizLineCursor =
{
	17,17, 8,8, drag_all_horiz_line_cursor_bits, drag_all_horiz_line_cursor_mask_bits
};	

JCursorIndex
JXGetDragAllHorizLineCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragAllHorizLineCursorName,
									   kDragAllHorizLineCursor);
}

/******************************************************************************
 Drag vert line cursor

 ******************************************************************************/

static const JCharacter* kDragVertLineCursorName = "JXDragVertLine";

static char drag_vert_line_cursor_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
   0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x08, 0x21, 0x00, 0x04, 0x41, 0x00,
   0xfe, 0xff, 0x00, 0x04, 0x41, 0x00, 0x08, 0x21, 0x00, 0x00, 0x01, 0x00,
   0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
   0x00, 0x00, 0x00};

static char drag_vert_line_cursor_mask_bits[] = {
   0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00,
   0x80, 0x03, 0x00, 0x9c, 0x73, 0x00, 0x9e, 0xf3, 0x00, 0xff, 0xff, 0x01,
   0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0x9e, 0xf3, 0x00, 0x9c, 0x73, 0x00,
   0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00,
   0x80, 0x03, 0x00};

static const JXCursor kDragVertLineCursor =
{
	17,17, 8,8, drag_vert_line_cursor_bits, drag_vert_line_cursor_mask_bits
};	

JCursorIndex
JXGetDragVertLineCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragVertLineCursorName,
									   kDragVertLineCursor);
}

/******************************************************************************
 Drag all vert line cursor

 ******************************************************************************/

static const JCharacter* kDragAllVertLineCursorName = "JXDragAllVertLine";

static char drag_all_vert_line_cursor_bits[] = {
   0x00, 0x00, 0x00, 0x20, 0x09, 0x00, 0x20, 0x09, 0x00, 0x20, 0x09, 0x00,
   0x20, 0x09, 0x00, 0x20, 0x09, 0x00, 0x28, 0x29, 0x00, 0x24, 0x49, 0x00,
   0xfe, 0xff, 0x00, 0x24, 0x49, 0x00, 0x28, 0x29, 0x00, 0x20, 0x09, 0x00,
   0x20, 0x09, 0x00, 0x20, 0x09, 0x00, 0x20, 0x09, 0x00, 0x20, 0x09, 0x00,
   0x00, 0x00, 0x00};

static char drag_all_vert_line_cursor_mask_bits[] = {
   0xf0, 0x1f, 0x00, 0xf0, 0x1f, 0x00, 0xf0, 0x1f, 0x00, 0xf0, 0x1f, 0x00,
   0xf0, 0x1f, 0x00, 0xfc, 0x7f, 0x00, 0xfe, 0xff, 0x00, 0xff, 0xff, 0x01,
   0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0xfe, 0xff, 0x00, 0xfc, 0x7f, 0x00,
   0xf0, 0x1f, 0x00, 0xf0, 0x1f, 0x00, 0xf0, 0x1f, 0x00, 0xf0, 0x1f, 0x00,
   0xf0, 0x1f, 0x00};

static const JXCursor kDragAllVertLineCursor =
{
	17,17, 8,8, drag_all_vert_line_cursor_bits, drag_all_vert_line_cursor_mask_bits
};	

JCursorIndex
JXGetDragAllVertLineCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragAllVertLineCursorName,
									   kDragAllVertLineCursor);
}

/******************************************************************************
 Drag file

 ******************************************************************************/

static const JCharacter* kDragFileCursorName = "JXDragFile";

static char drag_file_cursor_bits[] = {
   0x00, 0x00, 0xfe, 0x01, 0x02, 0x03, 0x02, 0x05, 0x02, 0x0f, 0x02, 0x08,
   0x02, 0x08, 0x42, 0x08, 0xc2, 0x08, 0xc2, 0x09, 0xc2, 0x0b, 0xc2, 0x07,
   0xc2, 0x0f, 0xde, 0x1f, 0xc0, 0x3f, 0xc0, 0x07, 0xc0, 0x06, 0x40, 0x0c,
   0x00, 0x0c, 0x00, 0x18, 0x00, 0x18, 0x00, 0x00};

static char drag_file_cursor_mask_bits[] = {
   0xff, 0x01, 0xff, 0x03, 0xff, 0x07, 0xff, 0x0f, 0xff, 0x1f, 0xff, 0x1f,
   0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f,
   0xff, 0x3f, 0xff, 0x7f, 0xff, 0x7f, 0xe0, 0x7f, 0xe0, 0x0f, 0xe0, 0x1e,
   0x60, 0x1e, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x18};

static const JXCursor kDragFileCursor =
{
	15,22, 6,7, drag_file_cursor_bits, drag_file_cursor_mask_bits
};	

JCursorIndex
JXGetDragFileCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragFileCursorName,
									   kDragFileCursor);
}

/******************************************************************************
 Drag directory

 ******************************************************************************/

static const JCharacter* kDragDirectoryCursorName = "JXDragDirectory";

static char drag_directory_cursor_bits[] = {
   0x00, 0x00, 0xf8, 0x00, 0x04, 0x01, 0x02, 0x3e, 0xfe, 0x41, 0x02, 0x40,
   0x02, 0x40, 0x42, 0x40, 0xc2, 0x40, 0xc2, 0x41, 0xc2, 0x43, 0xc2, 0x47,
   0xde, 0x6f, 0xc0, 0x1f, 0xc0, 0x3f, 0xc0, 0x07, 0xc0, 0x06, 0x40, 0x0c,
   0x00, 0x0c, 0x00, 0x18, 0x00, 0x18, 0x00, 0x00};

static char drag_directory_cursor_mask_bits[] = {
   0xf8, 0x00, 0xfc, 0x01, 0xfe, 0x3f, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xe0, 0x7f, 0xe0, 0x7f, 0xe0, 0x0f, 0xe0, 0x1e,
   0x60, 0x1e, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x18};

static const JXCursor kDragDirectoryCursor =
{
	16,22, 6,7, drag_directory_cursor_bits, drag_directory_cursor_mask_bits
};	

JCursorIndex
JXGetDragDirectoryCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragDirectoryCursorName,
									   kDragDirectoryCursor);
}

/******************************************************************************
 Drag file and directory

 ******************************************************************************/

static const JCharacter* kDragFileAndDirectoryCursorName = "JXDragFileAndDirectory";

static char drag_file_and_directory_cursor_bits[] = {
   0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x04, 0x01, 0x00, 0x02, 0x3e, 0x00,
   0xfe, 0x41, 0x00, 0x02, 0x40, 0x00, 0xfa, 0x47, 0x00, 0x0a, 0x4c, 0x00,
   0x0a, 0x54, 0x00, 0x0a, 0x7c, 0x00, 0x0a, 0x60, 0x00, 0x0a, 0x60, 0x00,
   0x0e, 0x61, 0x00, 0x08, 0x23, 0x00, 0x08, 0x27, 0x00, 0x08, 0x2f, 0x00,
   0x08, 0x1f, 0x00, 0x08, 0x3f, 0x00, 0x78, 0x7f, 0x00, 0x00, 0xff, 0x00,
   0x00, 0x1f, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x31, 0x00, 0x00, 0x30, 0x00,
   0x00, 0x60, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00};

static char drag_file_and_directory_cursor_mask_bits[] = {
   0xf8, 0x00, 0x00, 0xfc, 0x01, 0x00, 0xfe, 0x7f, 0x00, 0xff, 0xff, 0x00,
   0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00,
   0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00,
   0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xfc, 0x7f, 0x00, 0xfc, 0x7f, 0x00,
   0xfc, 0x7f, 0x00, 0xfc, 0x7f, 0x00, 0xfc, 0xff, 0x00, 0xfc, 0xff, 0x01,
   0x80, 0xff, 0x01, 0x80, 0x3f, 0x00, 0x80, 0x7b, 0x00, 0x80, 0x79, 0x00,
   0x00, 0xf0, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x60, 0x00};

static const JXCursor kDragFileAndDirectoryCursor =
{
	17,27, 6,7, drag_file_and_directory_cursor_bits,
	drag_file_and_directory_cursor_mask_bits
};	

JCursorIndex
JXGetDragFileAndDirectoryCursor
	(
	JXDisplay* display
	)
{
	return display->CreateCustomCursor(kDragFileAndDirectoryCursorName,
									   kDragFileAndDirectoryCursor);
}
