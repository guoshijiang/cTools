/****************************************************************************
 *文件名称: 			base_station.h     
 *创建日期：			2017-9-22      
 *作    者:       郭世江          
 *修改记录：			2017-9-22  首次创建
 *功能描述：      公共数据处理文件
 *****************************************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
char *trim(char *word)
{
	if(word)
	{
		char c;
		char chword[4098];
		c = word[0];
		while(c == ' ' || c == '\t' || c == '\n')
		{
			word = word + 1;
			c = word[0];
			if(c == '\0')
			{
				return NULL;	
			}
		}	
		c = word[strlen(word)-1];
		while(c == ' ' || c == '\t' || c == '\n')
		{
			memset(chword, 0x00, sizeof(chword));
			memcpy(chword, word, strlen(word)-1);
			strcpy(word, chword);
			c = word[strlen(word)-1];
		}
	}
	return word;
}
*/

#endif