#include <statepop.h>

statepop::vector statepop::Get_Populations(void)
{
	////////////////////////////////////////////////////////////////////////////////////////
	//
	// Output best eigenvector to console and file
	//
	////////////////////////////////////////////////////////////////////////////////////////
	vector vEig = smMatrixB.Get_Eigenvector(1.0);
	vector vEigTest = smMatrixB * vEig;
	bool bEigenvector_Fault = false;
	statepop::floattype ldMax_Error = 0.0;
	for (size_t tI = 0; tI < vEigTest.size(); tI++)
	{
		statepop::floattype dErr = std::fabs(vEigTest[tI] - vEig[tI]) / vEig[tI];
		if (dErr > ldMax_Error)
			ldMax_Error = dErr;
		if (dErr > 1.0e-2) // 1% error
		{
			bEigenvector_Fault = true;
			std::cerr << "Eignevector fault at index " << tI << "( " << vEigTest[tI] << " , " << vEig[tI] << " )" << std::endl;
		}
	}
	if (!bEigenvector_Fault)
		std::cout << "Eigenvector check pass. Maximum Error is " << ldMax_Error * 100.0 << "%." << std::endl;
	return vEig;
}
