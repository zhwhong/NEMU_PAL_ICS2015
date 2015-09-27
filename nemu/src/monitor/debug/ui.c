#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

bool IsNumber(char *str){
	int i;
	bool flag = true;
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] >= '0' && str[i] <= '9')
			continue;
		else{
			flag = false;
			break;
		}
	}
	return flag;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args){
	char *arg = strtok(NULL, " ");
	int temp;

	if(arg == NULL){
		printf("单步执行1步：\n");
		cpu_exec(1);	
	}
	else{
		if(IsNumber(arg)){
			temp = atoi(arg);
			printf("单步执行%d步：\n",temp);
			cpu_exec(temp);
		}
		else{
			printf("Invalid command!!!\n");
		}
	}	
	return 0;
}

static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	
	if(arg == NULL){
		printf("Input \"info r\" or \"info w\" to print the program state!\n");
	}	
	else if(*arg == 'r'){
		printf("打印寄存器状态：\n");
		printf("\teax = %x\n", cpu.eax);
		printf("\tecx = %x\n", cpu.ecx);
		printf("\tedx = %x\n", cpu.edx);
		printf("\tebx = %x\n", cpu.ebx);
		printf("\tesp = %x\n", cpu.esp);
		printf("\tebp = %x\n", cpu.ebp);
		printf("\tesi = %x\n", cpu.esi);
		printf("\tedi = %x\n", cpu.edi);
		printf("\teip = %x\n", cpu.eip);
	}
	else if(*arg == 'w'){
	    printf("打印监视点信息：\n");
		//printf("");	
	}
	else{
		printf("Invalid command!!!\n");
	}
	return 0;
}

static int cmd_p(char *args){
	char *arg = strtok(NULL, " ");
	if(arg == NULL){
		printf("There is no expression to calculate!\n");
	}
	else{
		printf("表达式求值：\n");
		printf("The value of the expression is \n");
	}
	return 0;
}



static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{"si", "Single execution n step",cmd_si},
	{"info", "Print program state",cmd_info},
	{"p", "Calculate the expression's value", cmd_p}

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1){
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
