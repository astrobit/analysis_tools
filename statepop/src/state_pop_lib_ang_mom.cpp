
unsigned int Ang_Mom_Term_To_L(char i_chTerm)
{
	unsigned int uiL = -1;
	switch(i_chTerm)
	{
	case 's':
	case 'S':
		uiL = 0;
		break;
	case 'p':
	case 'P':
		uiL = 1;
		break;
	case 'd':
	case 'D':
		uiL = 2;
		break;
	default:
		if (i_chTerm >= 'a' && i_chTerm <= 'z')
		{
			uiL = i_chTerm - 'f' + 3;
			if (i_chTerm >= 'j')
				uiL--;
			if (i_chTerm >= 'p')
				uiL--;
			if (i_chTerm >= 's')
				uiL--;
		}
		else if (i_chTerm >= 'A' && i_chTerm <= 'Z')
		{
			uiL = i_chTerm - 'F' + 3;
			if (i_chTerm >= 'J')
				uiL--;
			if (i_chTerm >= 'P')
				uiL--;
			if (i_chTerm >= 'S')
				uiL--;
		}
		break;
	}
	return uiL;
}
