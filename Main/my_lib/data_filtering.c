#include "stm32f4xx.h"


/**
*函数功能: 均值过滤器
*参    数: 带滤波样本
*返 回 值: 滤波结果
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
函数名称: Filter()	
函数功能: 平滑滤波
参数    : 带滤波样本
返回值  ：滤波结果
-----------------------------------------------------*/

#define FILTER_N 10		     //定义数组长度为八位

uint16_t filter_buf[FILTER_N+1];  //定义数组


uint16_t Smoothing_Filtering(uint16_t value)     
{
	int i;
	uint16_t filter_sum=0;
	filter_buf[FILTER_N] = value;		  //AD转换的值赋给数组的最后一个值
	for(i=0;i<FILTER_N;i++)
	{
		filter_buf[i]=filter_buf[i+1];	  //所有的数据左移，数组第一个元素摒弃
		filter_sum+=filter_buf[i];
	}
	return(uint16_t)(filter_sum/FILTER_N);		  //返回对数组里的元素求得的平均值
}