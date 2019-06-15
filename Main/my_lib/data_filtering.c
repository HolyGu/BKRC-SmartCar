#include "stm32f4xx.h"


/**
*��������: ��ֵ������
*��    ��: ���˲�����
*�� �� ֵ: �˲����
**/
uint16_t Mean_Filter(uint16_t m)
{
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;
  
  if(flag_first == 0)
  {
    flag_first = 1;

    for(i = 0, sum = 0; i < _buff_max; i++)
    {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  }
  else
  {
    sum -= _buff[0];
    for(i = 0; i < (_buff_max - 1); i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];
    
    i = sum / 10.0;
    return i;
  }
}

/*---------------------------------------------------
��������: Filter()	
��������: ƽ���˲�
����    : ���˲�����
����ֵ  ���˲����
-----------------------------------------------------*/

#define FILTER_N 10		     //�������鳤��Ϊ��λ

uint16_t filter_buf[FILTER_N+1];  //��������


uint16_t Smoothing_Filtering(uint16_t value)     
{
	int i;
	uint16_t filter_sum=0;
	filter_buf[FILTER_N] = value;		  //ADת����ֵ������������һ��ֵ
	for(i=0;i<FILTER_N;i++)
	{
		filter_buf[i]=filter_buf[i+1];	  //���е��������ƣ������һ��Ԫ������
		filter_sum+=filter_buf[i];
	}
	return(uint16_t)(filter_sum/FILTER_N);		  //���ض��������Ԫ����õ�ƽ��ֵ
}