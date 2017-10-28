

const char * Pass_Integer(const char * i_lpszCursor)
{
	while (i_lpszCursor != nullptr && i_lpszCursor[0] >= '0' && i_lpszCursor[0] <= '9')
		i_lpszCursor++;
	return i_lpszCursor;
}
const char * Pass_Whitespace(const char * i_lpszCursor)
{
	while (i_lpszCursor != nullptr && (i_lpszCursor[0] == ' ' || i_lpszCursor[0] == '\t'))
		i_lpszCursor++;
	return i_lpszCursor;
}
