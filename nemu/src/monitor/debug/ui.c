#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <inttypes.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

char* rl_gets();
bool IsNumber(char *str);
static int cmd_help(char *args);
static int cmd_c(char *args);
static int cmd_q(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_p(char *args);
static int cmd_x(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);
static int cmd_bt(char *args);

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
//判断一个字符串是不是数字串
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
//继续运行
static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}
//退出
static int cmd_q(char *args) {
	return -1;
}
//单步执行
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
//打印寄存器状态和监视点信息
static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	
	if(arg == NULL){
		printf("Input \"info r\" or \"info w\" to print the program state!\n");
	}	
	else if(strcmp(arg,"r") == 0){
		printf("打印寄存器状态：\n");
		printf("\tHEX: eax = 0x%-16xDEC: eax = %u\n", cpu.eax, cpu.eax);
		printf("\tHEX: ecx = 0x%-16xDEC: ecx = %u\n", cpu.ecx, cpu.ecx);
		printf("\tHEX: edx = 0x%-16xDEC: edx = %u\n", cpu.edx, cpu.edx);
		printf("\tHEX: ebx = 0x%-16xDEC: ebx = %u\n", cpu.ebx, cpu.ebx);
		printf("\tHEX: esp = 0x%-16xDEC: esp = %u\n", cpu.esp, cpu.esp);
		printf("\tHEX: ebp = 0x%-16xDEC: ebp = %u\n", cpu.ebp, cpu.ebp);
		printf("\tHEX: esi = 0x%-16xDEC: esi = %u\n", cpu.esi, cpu.esi);
		printf("\tHEX: edi = 0x%-16xDEC: edi = %u\n", cpu.edi, cpu.edi);
		printf("\tHEX: eip = 0x%-16xDEC: eip = %u\n", cpu.eip, cpu.eip);
	}
	else if(strcmp(arg,"w") == 0){
	    printf("打印监视点信息：\n");
		WP *temp = GetHead();
		if(temp == NULL)
			printf("The watchpoint list is empyty!!!\n");
		while(temp)
		{
			printf("watchpoint %d: %s\tresult = %u\n", temp->NO, temp->expr, temp->result);
			temp = temp->next;
		}
	}
	else{
		printf("Invalid command!!!\n");
	}
	return 0;
}
//表达式求值
static int cmd_p(char *args){
	bool success = true;
	uint32_t temp;
	if(args == NULL){
		printf("There is no expression to calculate!\n");
	}
	else{
		temp = expr(args, &success);
		if(success){
			printf("表达式求值：\n");
			printf("The value of the expression is %"PRIu32"(十进制) or 0x%x(十六进制)\n",temp,temp); 
		}
		else{
			printf("Invalid expression!!!\n");
		}
	}
	return 0;
}
//设置监视点
static int cmd_w(char *args){
	if(args == NULL)
	{
		printf("There is no expression to set a new watchpoint!!!\n");
		return 0;
	}
	uint32_t result;
	bool flag;
	result = expr(args, &flag);
	if(flag == false)
	{
		printf("The expression is invalid!\n");
		return 0;
	}
	WP *temp = new_wp(args, result);
	if(temp)
		printf("成功创建序号为%d的监视点\n",temp->NO);
	else
		printf("监视点创建失败!!!\n");
	return 0;
}
//删除监视点
static int cmd_d(char *args){
	if(args == NULL)
	{
		printf("Please input the number of the watchpoint you want to delete!\n");
		return 0;
	}	
	int n = atoi(args);
	if(n<1 || n>32)
	{
		printf("The number you input is out of range!!!\n");
		return 0;
	}
	if(free_wp(n))
		printf("成功删除序号为%d的监视点\n",n);
	return 0;
}
//扫描内存
static int cmd_x(char *args){
	size_t n = 1;
	char *str_expr, *str_num;
	swaddr_t addr;
	int i;
	bool flag;

	str_num = strtok(NULL, " ");
	if(str_num == NULL)
	{
		printf("your input is invalid!!!\n");
		return 0;
	}
	str_expr = strtok(NULL, " ");
	if(str_expr == NULL)
	{
		printf("your input is invalid!!!\n");
		return 0;
	}

	n = atoi(str_num);
	addr = expr(str_expr, &flag);
	if(flag == false)
		return 0;
	printf("扫描内存起始地址: 0x%x\n以十六进制输出连续%d个四字节如下：\n", addr, n);
	
	for(i = 1; i <= n; i++)
	{
		printf("0x%.8x\t", swaddr_read(addr, 4));
		addr += 4;
		if(i%4 == 0)
			printf("\n");
	}
	if(i%4 != 1)
		printf("\n");
	return 0;
}
//打印栈帧链
static int cmd_bt(char *args){

	return 0;
}

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
	{"p", "Calculate the expression's value", cmd_p},
	{"x", "Scan the memory", cmd_x},
	{"w", "Set the watchpoint", cmd_w},
	{"d", "Delete the watchpoint", cmd_d},
	{"bt", "Print the stack list", cmd_bt}

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))
//帮助信息
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
//主循环
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
