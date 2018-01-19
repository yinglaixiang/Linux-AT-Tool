#ifndef __AT_H_
#define __AT_H_


#define QUEC_AT_PORT						"/dev/smd8"
#define QUECTEL_AT_CMD_FILE					"/data/at_cmd_list"
#define MAX_AT_CMD_SIZE						20

typedef struct AT_CMD
{
	int number;
	char info[30];
	char cmd[100];
}at_cmd;



void Ql_InitAT();
void Ql_CloseAT();
int Ql_SendAT(char* atCmd, char* finalRsp, long timeout_ms);

void at_get_all_cmd(void);
void at_input_cmd(void);
#endif