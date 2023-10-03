#include <JControl.h>

float MAF_Filter(MAF* dst, float rawdata)
{
	dst->OutputBuffer[dst->count] = rawdata;

	float sum = 0.0;

	for(int i = 0; i < MAF_SIZE; i++)
	{
		sum += dst->OutputBuffer[i];
	}

	dst->FilteredData = sum / MAF_SIZE;

	dst->count++;

	if(dst->count >= MAF_SIZE)
	{
		dst->count = 0;
	}

	return dst->FilteredData;
}

void Get_Motor_Status(ENCODER* dst, TIM_TypeDef* TIMx)
{
	dst->past = dst->now;
	dst->now = TIMx->CNT;
	dst->m1 = dst->now - dst->past;

	if(dst->m1 < -60000)
	{
		dst->m1 += 65535;
	}
	else if(dst->m1 > 60000)
	{
		dst->m1 -= 65535;
	}

	dst->RPM = (60.0 * dst->m1) / (Tc * CPR);
	dst->DEGREE += (dst->m1 / (CPR / 360.0));
}

void Duty_Control_Velocity(DUTY* dst, DIRECTION* DIRx, GPIO_TypeDef* GPIOx, uint16_t PINx, TIM_TypeDef* TIMx, uint8_t CHx, int target)
{
	dst->duty = target;

	if(dst->duty > dst->dutylimit)
	{
		dst->duty = dst->dutylimit;
	}
	else if(dst->duty < -dst->dutylimit)
	{
		dst->duty = -dst->dutylimit;
	}

	if(dst->duty < 0)
	{
		if(DIRx->FrontMotorDirection == RESET)
		{
			LL_GPIO_SetOutputPin(GPIOx, PINx);
			//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_SET);

			switch(CHx)
			{
			case 1:
			{
				LL_TIM_OC_SetCompareCH1(TIMx, -dst->duty);
				//TIMx->CCR1 = -dst->duty;
				break;
			}
			case 2:
			{
				LL_TIM_OC_SetCompareCH2(TIMx, -dst->duty);
				//TIMx->CCR2 = -dst->duty;
				break;
			}
			case 3:
			{
				LL_TIM_OC_SetCompareCH3(TIMx, -dst->duty);
				//TIMx->CCR3 = -dst->duty;
				break;
			}
			case 4:
			{
				LL_TIM_OC_SetCompareCH4(TIMx, -dst->duty);
				//TIMx->CCR4 = -dst->duty;
				break;
			}
			}
		}
		else
		{
			LL_GPIO_ResetOutputPin(GPIOx, PINx);
			//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_RESET);

			switch(CHx)
			{
			case 1:
			{
				LL_TIM_OC_SetCompareCH1(TIMx, -dst->duty);
				//TIMx->CCR1 = -dst->duty;
				break;
			}
			case 2:
			{
				LL_TIM_OC_SetCompareCH2(TIMx, -dst->duty);
				//TIMx->CCR2 = -dst->duty;
				break;
			}
			case 3:
			{
				LL_TIM_OC_SetCompareCH3(TIMx, -dst->duty);
				//TIMx->CCR3 = -dst->duty;
				break;
			}
			case 4:
			{
				LL_TIM_OC_SetCompareCH4(TIMx, -dst->duty);
				//TIMx->CCR4 = -dst->duty;
				break;
			}
			}
		}
	}
	else
	{
		if(DIRx->FrontMotorDirection == RESET)
		{
			LL_GPIO_ResetOutputPin(GPIOx, PINx);
			//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_RESET);

			switch(CHx)
			{
			case 1:
			{
				LL_TIM_OC_SetCompareCH1(TIMx, dst->duty);
				//TIMx->CCR1 = dst->duty;
				break;
			}
			case 2:
			{
				LL_TIM_OC_SetCompareCH2(TIMx, dst->duty);
				//TIMx->CCR2 = dst->duty;
				break;
			}
			case 3:
			{
				LL_TIM_OC_SetCompareCH3(TIMx, dst->duty);
				//TIMx->CCR3 = dst->duty;
				break;
			}
			case 4:
			{
				LL_TIM_OC_SetCompareCH4(TIMx, dst->duty);
				//TIMx->CCR4 = dst->duty;
				break;
			}
			}
		}
		else
		{
			LL_GPIO_SetOutputPin(GPIOx, PINx);
			//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_SET);

			switch(CHx)
			{
			case 1:
			{
				LL_TIM_OC_SetCompareCH1(TIMx, dst->duty);
				//TIMx->CCR1 = dst->duty;
				break;
			}
			case 2:
			{
				LL_TIM_OC_SetCompareCH2(TIMx, dst->duty);
				//TIMx->CCR2 = dst->duty;
				break;
			}
			case 3:
			{
				LL_TIM_OC_SetCompareCH3(TIMx, dst->duty);
				//TIMx->CCR3 = dst->duty;
				break;
			}
			case 4:
			{
				LL_TIM_OC_SetCompareCH4(TIMx, dst->duty);
				//TIMx->CCR4 = dst->duty;
				break;
			}
			}
		}
	}
}

void PID_Control(PID* dst, float target, float input)
{
	dst->input = input;
	dst->target = target;

	dst->E = dst->target - dst->input;
	dst->sumE += dst->E;
	dst->diffE = dst->E - dst->E_old;

	if(dst->sumE > sumELimit)
	{
		dst->sumE = sumELimit;
	}
	else if(dst->sumE < -sumELimit)
	{
		dst->sumE = -sumELimit;
	}

	dst->output = (dst->kP * dst->E + dst->kI * dst->sumE + dst->kD * dst->diffE) / division;

	dst->E_old = dst->E;

	if(dst->output > dst->outputlimit)
	{
		dst->output = dst->outputlimit;
	}
	else if(dst->output < -dst->outputlimit)
	{
		dst->output = -dst->outputlimit;
	}
}

void PID_Control_Velocity(PID* dst, DIRECTION* DIRx, ENCODER* ENx, GPIO_TypeDef* GPIOx, uint16_t PINx, TIM_TypeDef* TIMx, uint8_t CHx, int target)
{
	if(DIRx->FrontEncoderDirection == CCW)
	{
		PID_Control(dst, -target, ENx->RPM);
	}
	else
	{
		PID_Control(dst, target, ENx->RPM);
	}

	dst->FilteredOutput = MAF_Filter(&dst->filter, dst->output);

	if(dst->output < 0)
	{
		if(DIRx->FrontEncoderDirection == CCW)
		{
			if(DIRx->FrontMotorDirection == RESET)
			{
				LL_GPIO_ResetOutputPin(GPIOx, PINx);
				//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_RESET);
			}
			else
			{
				LL_GPIO_SetOutputPin(GPIOx, PINx);
				//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_SET);
			}
		}
		else
		{
			if(DIRx->FrontMotorDirection == RESET)
			{
				LL_GPIO_SetOutputPin(GPIOx, PINx);
				//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_SET);
			}
			else
			{
				LL_GPIO_ResetOutputPin(GPIOx, PINx);
				//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_RESET);
			}
		}

		switch(CHx)
		{
		case 1:
		{
			LL_TIM_OC_SetCompareCH1(TIMx, -dst->FilteredOutput);
			//TIMx->CCR1 = -dst->FilteredOutput;
			break;
		}
		case 2:
		{
			LL_TIM_OC_SetCompareCH2(TIMx, -dst->FilteredOutput);
			//TIMx->CCR2 = -dst->FilteredOutput;
			break;
		}
		case 3:
		{
			LL_TIM_OC_SetCompareCH3(TIMx, -dst->FilteredOutput);
			//TIMx->CCR3 = -dst->FilteredOutput;
			break;
		}
		case 4:
		{
			LL_TIM_OC_SetCompareCH4(TIMx, -dst->FilteredOutput);
			//TIMx->CCR4 = -dst->FilteredOutput;
			break;
		}
		}
	}
	else
	{
		if(DIRx->FrontEncoderDirection == CCW)
		{
			if(DIRx->FrontMotorDirection == RESET)
			{
				LL_GPIO_SetOutputPin(GPIOx, PINx);
				//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_SET);
			}
			else
			{
				LL_GPIO_ResetOutputPin(GPIOx, PINx);
				//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_RESET);
			}
		}
		else
		{
			if(DIRx->FrontMotorDirection == RESET)
			{
				LL_GPIO_ResetOutputPin(GPIOx, PINx);
				//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_RESET);
			}
			else
			{
				LL_GPIO_SetOutputPin(GPIOx, PINx);
				//HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_SET);
			}
		}

		switch(CHx)
		{
		case 1:
		{
			LL_TIM_OC_SetCompareCH1(TIMx, dst->FilteredOutput);
			//TIMx->CCR1 = dst->FilteredOutput;
			break;
		}
		case 2:
		{
			LL_TIM_OC_SetCompareCH2(TIMx, dst->FilteredOutput);
			//TIMx->CCR2 = dst->FilteredOutput;
			break;
		}
		case 3:
		{
			LL_TIM_OC_SetCompareCH3(TIMx, dst->FilteredOutput);
			//TIMx->CCR3 = dst->FilteredOutput;
			break;
		}
		case 4:
		{
			LL_TIM_OC_SetCompareCH4(TIMx, dst->FilteredOutput);
			//TIMx->CCR4 = dst->FilteredOutput;
			break;
		}
		}
	}
}
