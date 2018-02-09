#include <kurucz_data.h>

std::string Kurucz_Line_Data::mysubstr(const std::string & i_szStr, size_t &tStart, size_t tLen) const
{
	std::string szRet = i_szStr.substr(tStart,tLen);
	tStart += tLen;
	return szRet;
}

void Kurucz_Line_Data::Fix_Label(std::string & io_szLabel) const
{
	return;
// fixes problems with the kurucz configurations
	if (io_szLabel[0] >= 'a' && io_szLabel[0] <= 'z' &&
		io_szLabel[1] >= '0' && io_szLabel[1] <= '9' &&
		io_szLabel[2] >= 'a' && io_szLabel[2] <= 'z')
	{
		std::string szFixed_Label = io_szLabel.substr(1,m_cLevel_Lower.m_szLabel.size() - 1);
		szFixed_Label.push_back(' ');
		io_szLabel = szFixed_Label;
	}
}

