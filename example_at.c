#include "ql_oe.h"
#include "string.h"
#include "at.h"
#include "stdio.h"



static int smd_fd = -1;//AT端口的文件句柄
static at_cmd cmd[MAX_AT_CMD_SIZE];//保存AT命令
static int num_of_cmd = 0;



int main(int argc, char* argv[])
{
	int select;
	char c;
	int index = 0;
	Ql_InitAT();
	at_get_all_cmd();
	while(1)
	{


		while((cmd[index].number != -1) && (index < MAX_AT_CMD_SIZE))
		{
			printf("%d.%s\n",cmd[index].number,cmd[index].info);
			index++;
		}
		printf("%d.退出\n",index);
		index = 0;
		select = 0;
		while((c = getchar()) != '\n')
		{
			if( c > '9' || c< '0' )
			{
				continue;
			}
			select = select*10+(c-'0');
		}
		//scanf("%d",&select);
		//printf("input num is : %d\n",select);
		fflush(stdin);
		if(select > num_of_cmd)
		{
			printf("error input please input again\n");
			continue;
		}else if(select == num_of_cmd)
		{
			goto end;
		}else
		{
			Ql_SendAT(cmd[select].cmd,"OK",1000);
		}

		// switch(select)
		// {
		// 	case '1':
		// 		Ql_SendAT("AT+COPS?","OK",1000);
		// 		break;
		// 	case '2':
		// 		Ql_SendAT("AT+CSQ","OK",1000);
		// 		break;
		// 	case '3':
		// 		Ql_SendAT("AT+ICCID","OK",1000);
		// 		break;
		// 	case '4':
		// 		at_input_cmd();
		// 		break;
		// 	case '5':
		// 		goto end;
		// 	default:
		// 		printf("Unknown cmd %d\n",select);
		// 		break;
		// }
	}
end:
   	Ql_CloseAT();
    printf("< Quectel OpenLinux: AT example end >\n\n");
	return 0;
}


/********************************************************
* Copyright (c) redtea mobile.
*name：at_cmd()
*funcition：
	用于从读取所有的AT指令
*instructions：
********************************************************/
void at_get_all_cmd(void)
{
	FILE *fp;
	char buf[100];
	char *ptr;
	int index = 0;
	int info_len;//CMD描述字符的长度
	if((fp = fopen(AT_CMD_FILE,"r")) == NULL)
	{
		printf("open at_cmd_list error \n");
		exit(0);
	}

	/*初始化 cmd 数组*/
	for(index = 0;index < MAX_AT_CMD_SIZE ;index++)
	{
		cmd[index].number = -1;
		memset(cmd[index].info,0,sizeof(at_cmd));
	}
	num_of_cmd = 0;
	/*从命令文件中读取所有命令*/
	printf("begain read cmd \n");
	index = 0;

	while(fgets(buf,100,fp) != NULL)
	{
		if(strlen(buf) > 2)
		{
			num_of_cmd++;
			cmd[index].number = index;
			ptr = strstr(buf,"#");
			info_len = ptr - buf;
			strncpy(cmd[index].info,buf,info_len);
			cmd[index].info[info_len] = '\0';
			strcpy(cmd[index].cmd,ptr+1);
			info_len = strlen(cmd[index].cmd);
			cmd[index].cmd[info_len-1] = '\0';
//			printf("%d.%s\n",cmd[index].number,cmd[index].info);
			index++;
			memset(buf,0,sizeof(buf));
		}else
		{
//			cmd[index].number = -1;
			break;
		}
	}
	fclose(fp);
}

/********************************************************
* Copyright (c) redtea mobile.
*name：at_cmd()
*funcition：
	用于手动输入AT指令的
*instructions：
********************************************************/
void at_input_cmd(void)
{
	char cmd[100];
	printf("输入q退出\n\n");
	while(1)
	{
		printf("please input at cmd :\n");
		scanf("%s",cmd);
		if(strcmp(cmd,"q") == 0 ||
		   strcmp(cmd,"Q") ==0  ||
		   strcmp(cmd,"exit") ==0 ||
		   strcmp(cmd,"quit") == 0)
			return ;
		Ql_SendAT(cmd,"OK",1000);
	}

}

void Ql_InitAT()
{
	printf("< Quectel OpenLinux: AT example >\n");
    if((smd_fd = open(QUEC_AT_PORT, O_RDWR | O_NONBLOCK | O_NOCTTY)) == -1)
    {
    	printf("Ql_InitAT error exit \n");
    	exit(0);
    }
	printf("< open(\"%s\")=%d >\n", QUEC_AT_PORT, smd_fd);
}

void Ql_CloseAT()
{
	close(smd_fd);
    printf("< Quectel OpenLinux: AT example end >\n\n");
}

int Ql_SendAT(char* atCmd, char* finalRsp, long timeout_ms)
{
    int iRet;
    int iLen;
    fd_set fds;
    int rdLen;
#define lenToRead 100
    char strAT[100];
    char strFinalRsp[100];
    char strResponse[100];
    struct timeval timeout = {0, 0};
    boolean bRcvFinalRsp = FALSE;

    memset(strAT, 0x0, sizeof(strAT));
    iLen = sizeof(atCmd);
    strncpy(strAT, atCmd, iLen);

    sprintf(strFinalRsp, "\r\n%s", finalRsp);

	timeout.tv_sec  = timeout_ms / 1000;
	timeout.tv_usec = timeout_ms % 1000;


    // Add <cr><lf> if needed
    iLen = strlen(atCmd);
    if ((atCmd[iLen-1] != '\r') && (atCmd[iLen-1] != '\n'))
    {
        iLen = sprintf(strAT, "%s\r\n", atCmd);
        strAT[iLen] = 0;
    }

    // Send AT
    iRet = write(smd_fd, strAT, iLen);
    printf(">>Send AT: \"%s\", iRet=%d\n", atCmd, iRet);

    // Wait for the response
	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(smd_fd, &fds);

		printf("timeout.tv_sec=%d, timeout.tv_usec: %d \n", (int)timeout.tv_sec, (int)timeout.tv_usec);
		switch (select(smd_fd + 1, &fds, NULL, NULL, &timeout))
		//switch (select(smd_fd + 1, &fds, NULL, NULL, NULL))	// block mode
		{
		case -1:
			printf("< select error >\n");
			return -1;

		case 0:
			printf("< time out >\n");
			return 1;

		default:
			if (FD_ISSET(smd_fd, &fds))
			{
				do {
					memset(strResponse, 0x0, sizeof(strResponse));
					rdLen = read(smd_fd, strResponse, lenToRead);
					printf(">>Read response/urc, len=%d, content:\n%s\n", rdLen, strResponse);
					//printf("rcv:%s", strResponse);
					//printf("final rsp:%s", strFinalRsp);
					if ((rdLen > 0) && strstr(strResponse, strFinalRsp))
					{
					    if (strstr(strResponse, strFinalRsp)     // final OK response
					       || strstr(strResponse, "+CME ERROR:") // +CME ERROR
					       || strstr(strResponse, "+CMS ERROR:") // +CMS ERROR
					       || strstr(strResponse, "ERROR"))      // Unknown ERROR
					    {
					        //printf("\n< match >\n");
					        bRcvFinalRsp = TRUE;
					    }else{
					        printf("\n< not final rsp >\n");
					    }
					}
				} while ((rdLen > 0) && (lenToRead == rdLen));
			}else{
				printf("FD is missed\n");
			}
			break;
		}

		// Found the final response , return back
		if(bRcvFinalRsp)
		    break;
   	}
   	return 0;
}


