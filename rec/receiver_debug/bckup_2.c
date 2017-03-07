	if(i_count == 1)
	{
		TR0 = 0;
		Timer_reset();
		i_count = -1;
		if(time_meas>1200)
		{
			RX_char[b_count] = '0';
		}
		else if (time_meas < 370)
		{
			RX_char[b_count] = '2';
		}
		else
		{
			RX_char[b_count] = '1';
		}
		b_count = (b_count +1)%10 ;
	}
	i_count++ ;