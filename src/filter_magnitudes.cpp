#include <xstdlib.h>
#include <xio.h>

// Data from Pereira 2014 for SN 2011fe
class PEREIRA_DATA
{
public:
	double m_dEpoch;// estimate of time after explosion based on assumed time from Pereira 2014
	double m_dLog_Luminosity;
	PEREIRA_DATA(const double & i_dEpoch, const double & i_dLog_Luminosity)
	{
		m_dEpoch = i_dEpoch;
		m_dLog_Luminosity = i_dLog_Luminosity;
	}
};

PEREIRA_DATA	g_cPereira_Data[] = {
							PEREIRA_DATA(2.49, 7.9712932209),
							PEREIRA_DATA(3.39, 8.2662490688),
							PEREIRA_DATA(4.39, 8.4905376815),
							PEREIRA_DATA(5.39, 8.6960240873),
							PEREIRA_DATA(6.39, 8.8683090601),
							PEREIRA_DATA(7.39, 9.0071674775),
							PEREIRA_DATA(8.39, 9.1489900066),
							PEREIRA_DATA(9.39, 9.2108707374),
							PEREIRA_DATA(10.49, 9.3104132661),
							PEREIRA_DATA(11.39, 9.3629143399),
							PEREIRA_DATA(12.39, 9.4265611637),
							PEREIRA_DATA(16.39, 9.4857671829),
							PEREIRA_DATA(17.39, 9.4699865816),
							PEREIRA_DATA(18.39, 9.4653704764),
							PEREIRA_DATA(19.39, 9.453610882),
							PEREIRA_DATA(20.39, 9.4324414496),
							PEREIRA_DATA(21.39, 9.399067754),
							PEREIRA_DATA(24.39, 9.3250812657) };


int main(int i_iArg_Count, const char * i_lpszArg_Values[])
{

	return 0;
}
