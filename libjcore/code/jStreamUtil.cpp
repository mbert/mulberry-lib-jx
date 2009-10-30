/******************************************************************************
 jStreamUtil.cpp

	Useful functions for dealing with streams.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jStreamUtil.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jMemory.h>
#include <string.h>
#include <limits.h>
#include <jAssert.h>

/******************************************************************************
 JCopyBinaryData

	Copies binary data from input to output.
	Caller must set initial read and write marks.

 ******************************************************************************/

void
JCopyBinaryData
	(
	istream&	input,
	ostream&	output,
	const JSize	byteCount
	)
{
	// allocate transfer space -- we want a big chunk but we don't want it to fail

	JSize chunkSize  = 10000;
	JCharacter* data = JCreateBuffer(&chunkSize);

	// copy the data in chunks

	JSize readCount = 0;
	while (!input.eof() && !input.fail() && readCount < byteCount)
		{
		// don't read past byteCount

		if (readCount + chunkSize > byteCount)
			{
			chunkSize = byteCount - readCount;
			}

		// transfer the chunk

		input.read(data, chunkSize);
		output.write(data, chunkSize);
		readCount += chunkSize;
		}

	delete [] data;

	assert( !input.bad() );

	// since nothing failed, we don't want to leave the eof bit set

	input.clear();
}

/******************************************************************************
 JRead

	Read the specified number of characters from the stream.

 ******************************************************************************/

JString
JRead
	(
	istream&	input,
	const JSize	count
	)
{
	JString str;
	if (count != 0)
		str.Read(input, count);
	return str;
}

/******************************************************************************
 JReadAll

	Read characters until the end of the istream is reached.
	This function takes a JString* because the contents of the stream
	could be very large, and returning a JString requires twice as much
	memory because of the copy constructor.

	Not inline to avoid including JString.h in header file.

 ******************************************************************************/

void
JReadAll
	(
	istream&	input,
	JString*	str
	)
{
	JCharacter c;
	JReadUntil(input, 0, NULL, str, &c);
}

/******************************************************************************
 JReadLine

	Read characters from the istream until newline ('\n', '\r', '\r\n') is
	reached. newline is read in and discarded.

	If foundNewLine is not NULL, it tells whether or not the end of a line
	was actually encountered.

 ******************************************************************************/

JString
JReadLine
	(
	istream&	input,
	JBoolean*	foundNewLine
	)
{
	JString line;
	JCharacter c;
	const JBoolean foundDelimiter = JReadUntil(input, 2, "\r\n", &line, &c);
	if (foundDelimiter && c == '\r' && !input.eof() && input.peek() == '\n')
		{
		input.ignore();
		}
	if (foundNewLine != NULL)
		{
		*foundNewLine = foundDelimiter;
		}
	return line;
}

/******************************************************************************
 JReadUntil

	Read characters from the istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not NULL, it tells whether or not the delimiter
	was actually found.

 ******************************************************************************/

JString
JReadUntil
	(
	istream&			input,
	const JCharacter	delimiter,
	JBoolean*			foundDelimiter
	)
{
	JString str;
	JCharacter c;
	const JBoolean found = JReadUntil(input, 1, &delimiter, &str, &c);
	if (foundDelimiter != NULL)
		{
		*foundDelimiter = found;
		}
	return str;
}

/******************************************************************************
 JReadUntil

	Read characters from the istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns kJTrue if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns kJFalse if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be NULL.

 ******************************************************************************/

JBoolean
JReadUntil
	(
	istream&			input,
	const JSize			delimiterCount,
	const JCharacter*	delimiters,
	JString*			str,
	JCharacter*			delimiter
	)
{
	str->Clear();
	JBoolean isDelimiter = kJFalse;

	const JSize bufSize = 1024;
	JCharacter buf[ bufSize ];

	JIndex i = 0;
	while (1)
		{
		JCharacter c;
		input.get(c);
		if (input.fail())
			{
			break;
			}

		for (JIndex j=0; j<delimiterCount; j++)
			{
			if (c == delimiters[j])
				{
				isDelimiter = kJTrue;
				if (delimiter != NULL)
					{
					*delimiter = c;
					}
				break;
				}
			}

		if (isDelimiter || input.eof())
			{
			break;
			}
		else
			{
			buf[i] = c;
			i++;
			if (i == bufSize)
				{
				str->Append(buf, bufSize);
				i=0;
				}
			}
		}

	str->Append(buf, i);
	return isDelimiter;
}

/******************************************************************************
 JReadUntilws

	Read characters from the istream until white-space is reached.
	white-space is read in and discarded.

	If foundws is not NULL, it tells whether or not whitespace
	was actually encountered.

 ******************************************************************************/

JString
JReadUntilws
	(
	istream&	input,
	JBoolean*	foundws
	)
{
	JCharacter delimiters[] = { ' ', '\t', '\n' };
	const JSize delimiterCount = sizeof(delimiters)/sizeof(JCharacter);

	JString str;
	JCharacter c;
	const JBoolean found = JReadUntil(input, delimiterCount, delimiters, &str, &c);
	if (foundws != NULL)
		{
		*foundws = found;
		}
	input >> ws;
	return str;
}

/******************************************************************************
 JIgnoreLine

	Toss characters from the istream until newline ('\n', '\r', '\r\n') is
	reached. newline is read in and discarded.

	If foundNewLine is not NULL, it tells whether or not the end of a line
	was actually encountered.

 ******************************************************************************/

void
JIgnoreLine
	(
	istream&	input,
	JBoolean*	foundNewLine
	)
{
	JCharacter c;
	const JBoolean foundDelimiter = JIgnoreUntil(input, 2, "\r\n", &c);
	if (foundDelimiter && c == '\r' && !input.eof() && input.peek() == '\n')
		{
		input.ignore();
		}
	if (foundNewLine != NULL)
		{
		*foundNewLine = foundDelimiter;
		}
}

/******************************************************************************
 JIgnoreUntil

	Discard characters from the istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not NULL, it tells whether or not the delimiter
	was actually found.

	To keep the prototype as close to JReadUntil() as possible,
	foundDelimiter is not the return value.

 ******************************************************************************/

void
JIgnoreUntil
	(
	istream&			input,
	const JCharacter	delimiter,
	JBoolean*			foundDelimiter
	)
{
	JCharacter c;
	const JBoolean found = JIgnoreUntil(input, 1, &delimiter, &c);
	if (foundDelimiter != NULL)
		{
		*foundDelimiter = found;
		}
}

void 
JIgnoreUntil
	(
	istream&			input, 
	const JCharacter*	delimiter,
	JBoolean*			foundDelimiter
	)
{
	const JSize delimLength = strlen(delimiter);
	assert( delimLength > 0 );

	if (delimLength == 1)
		{
		JIgnoreUntil(input, delimiter[0], foundDelimiter);
		return;
		}

	if (foundDelimiter != NULL)
		{
		*foundDelimiter = kJFalse;
		}

	JString window;
	window.SetBlockSize(delimLength);
	window.Read(input, delimLength);

	while (!input.eof() && !input.fail() && window != delimiter)
		{
		window.RemoveSubstring(1,1);
		window.AppendCharacter(input.get());
		}

	if (window == delimiter && foundDelimiter != NULL)
		{
		*foundDelimiter = kJTrue;
		}
}

/******************************************************************************
 JIgnoreUntil

	Toss characters from the istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns kJTrue if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns kJFalse if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be NULL.

 ******************************************************************************/

JBoolean
JIgnoreUntil
	(
	istream&			input,
	const JSize			delimiterCount,
	const JCharacter*	delimiters,
	JCharacter*			delimiter
	)
{
	JBoolean isDelimiter = kJFalse;

	while (1)
		{
		JCharacter c;
		input.get(c);
		if (input.fail())
			{
			break;
			}

		for (JIndex i=0; i<delimiterCount; i++)
			{
			if (c == delimiters[i])
				{
				isDelimiter = kJTrue;
				if (delimiter != NULL)
					{
					*delimiter = c;
					}
				break;
				}
			}

		if (isDelimiter || input.eof())
			{
			break;
			}
		}

	return isDelimiter;
}

/******************************************************************************
 jReadN

	Stolen from ACE library because we don't want to use ACE_HANDLE.  This
	is a HANDLE instead of an int on Windows.

 ******************************************************************************/

inline ssize_t
jRead
	(
	int		handle,
	void*	buf,
	size_t	len
	)
{
	# if defined (ACE_HAS_CHARPTR_SOCKOPT)
	return (ssize_t) ::read(handle, (char *) buf, len);
	# else
	return (ssize_t) ::read(handle, buf, len);
	# endif /* ACE_HAS_CHARPTR_SOCKOPT */
}

ssize_t
jReadN
	(
	int		handle,
    void*	buf,
    size_t	len,
    size_t*	bt
    )
{
	size_t temp;
	size_t &bytes_transferred = (bt == 0 ? temp : *bt);
	ssize_t n;

	for (bytes_transferred = 0;
		 bytes_transferred < len;
		 bytes_transferred += n)
		{
		n = jRead(handle, (char *) buf + bytes_transferred, len - bytes_transferred);

		if (n == -1 || n == 0)
			{
			return n;
			}
		}

	return bytes_transferred;
}

/******************************************************************************
 JRead

	Read the specified number of characters from the stream.

 ******************************************************************************/

JString
JRead
	(
	int			input,
	const JSize	count
	)
{
	char* buf = new char[ count ];
	assert( buf != NULL );

	size_t dataLength;
	ssize_t result = jReadN(input, buf, count, &dataLength);

	JString str(buf, dataLength);

	delete [] buf;

	return str;
}

/******************************************************************************
 JReadAll

	Read characters until the end of the data stream is reached.
	This function takes a JString* because the contents of the stream
	could be very large, and returning a JString requires twice as much
	memory because of the copy constructor.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

JBoolean
JReadAll
	(
	int				input,
	JString*		str,
	const JBoolean	closeInput
	)
{
	str->Clear();

	const JSize bufLength = 1024;

	char buf[ bufLength ];
	while (1)
		{
		size_t dataLength;
		ssize_t result = jReadN(input, buf, bufLength, &dataLength);

		if (dataLength > 0)
			{
			str->Append(buf, dataLength);
			}

		if (result == -1)
			{
			if (closeInput)
				{
				::close(input);
				}
			return kJFalse;
			}
		else if (result == 0)
			{
			if (closeInput)
				{
				::close(input);
				}
			return kJTrue;
			}
		// else, keep reading
		}
}

/******************************************************************************
 JReadUntil

	Read characters from the istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not NULL, it tells whether or not the delimiter
	was actually found.

 ******************************************************************************/

JString
JReadUntil
	(
	int					input,
	const JCharacter	delimiter,
	JBoolean*			foundDelimiter
	)
{
	JString str;
	JCharacter c;
	const JBoolean found = JReadUntil(input, 1, &delimiter, &str, &c);
	if (foundDelimiter != NULL)
		{
		*foundDelimiter = found;
		}
	return str;
}

/******************************************************************************
 JReadUntil

	Read characters from the istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns kJTrue if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns kJFalse if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be NULL.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

JBoolean
JReadUntil
	(
	int					input,
	const JSize			delimiterCount,
	const JCharacter*	delimiters,
	JString*			str,
	JCharacter*			delimiter
	)
{
	str->Clear();
	JBoolean isDelimiter = kJFalse;

	const JSize bufSize = 1024;
	JCharacter buf[ bufSize ];

	JIndex i = 0;
	while (1)
		{
		char c;
		size_t dataLength;
		ssize_t result = jReadN(input, &c, 1, &dataLength);

		if (result == -1)
			{
			return kJFalse;
			}

		for (JIndex j=0; j<delimiterCount; j++)
			{
			if (c == delimiters[j])
				{
				isDelimiter = kJTrue;
				if (delimiter != NULL)
					{
					*delimiter = c;
					}
				break;
				}
			}

		if (isDelimiter || result == 0)
			{
			break;
			}
		else
			{
			buf[i] = c;
			i++;
			if (i == bufSize)
				{
				str->Append(buf, bufSize);
				i=0;
				}
			}
		}

	str->Append(buf, i);
	return isDelimiter;
}

/******************************************************************************
 JIgnoreUntil

	Discard characters from the istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not NULL, it tells whether or not the delimiter
	was actually found.

	To keep the prototype as close to JReadUntil() as possible,
	foundDelimiter is not the return value.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

void
JIgnoreUntil
	(
	int					input,
	const JCharacter	delimiter,
	JBoolean*			foundDelimiter
	)
{
	JCharacter c;
	const JBoolean found = JIgnoreUntil(input, 1, &delimiter, &c);
	if (foundDelimiter != NULL)
		{
		*foundDelimiter = found;
		}
}

void 
JIgnoreUntil
	(
	int					input, 
	const JCharacter*	delimiter,
	JBoolean*			foundDelimiter
	)
{
	const JSize delimLength = strlen(delimiter);
	assert( delimLength > 0 );

	if (delimLength == 1)
		{
		JIgnoreUntil(input, delimiter[0], foundDelimiter);
		return;
		}

	if (foundDelimiter != NULL)
		{
		*foundDelimiter = kJFalse;
		}

	char* window = new char[ delimLength ];
	assert( window != NULL );

	size_t dataLength;
	ssize_t result = jReadN(input, window, delimLength, &dataLength);

	while (result > 0 && dataLength > 0 && strcmp(window, delimiter) != 0)
		{
		memmove(window, window+1, delimLength-1);
		result = jReadN(input, window + delimLength-1, 1, &dataLength);
		}

	if (strcmp(window, delimiter) == 0 && foundDelimiter != NULL)
		{
		*foundDelimiter = kJTrue;
		}

	delete [] window;
}

/******************************************************************************
 JIgnoreUntil

	Toss characters from the istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns kJTrue if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns kJFalse if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be NULL.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

JBoolean
JIgnoreUntil
	(
	int					input,
	const JSize			delimiterCount,
	const JCharacter*	delimiters,
	JCharacter*			delimiter
	)
{
	JBoolean isDelimiter = kJFalse;

	while (1)
		{
		char c;
		size_t dataLength;
		ssize_t result = jReadN(input, &c, 1, &dataLength);

		if (result == -1)
			{
			return kJFalse;
			}

		for (JIndex i=0; i<delimiterCount; i++)
			{
			if (c == delimiters[i])
				{
				isDelimiter = kJTrue;
				if (delimiter != NULL)
					{
					*delimiter = c;
					}
				break;
				}
			}

		if (isDelimiter || result == 0)
			{
			break;
			}
		}

	return isDelimiter;
}

/******************************************************************************
 JConvertToStream

	Convert the data from the given file descriptor into an ifstream.
	The location of the file is returned in tempFullName.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

JBoolean
JConvertToStream
	(
	int				input,
	ifstream*		input2,
	JString*		tempFullName,
	const JBoolean	closeInput
	)
{
	JString data;
	if (!JReadAll(input, &data, closeInput))
		{
		return kJFalse;
		}

	if (!(JCreateTempFile(tempFullName)).OK())
		{
		return kJFalse;
		}

	ofstream output(*tempFullName);
	data.Print(output);
	output.close();

	input2->open(*tempFullName);
	return JI2B(input2->good());
}
