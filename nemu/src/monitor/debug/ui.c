#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <inttypes.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <elf.h>

extern char *strtab;
extern Elf32_Sym *symtab;
extern int nr_symtab_entry;

typedef struct {
	swaddr_t prev_ebp;
	swaddr_t cur_addr;
	char func_name[30];
	swaddr_t begin_addr;
	swaddr_t ret_addr;
	uint32_t args[5];
} PartOfStackFrame;

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
		printf("默认单步执行1步：\n");
		cpu_exec(1);	
		return 0;
	}
	if(IsNumber(arg)){
		temp = atoi(arg);
		printf("单步执行%d步\n",temp);
		cpu_exec(temp);
	}
	else
		printf("Invalid steps!!!\n");	
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
		printf("标志寄存器EFLAGS部分值:\n");
		printf("\tZF = %u\tSF = %u\tOF = %u\tCF = %u\tAF = %u\tPF = %u\n", cpu.ZF, cpu.SF, cpu.OF, cpu.CF, cpu.AF, cpu.PF);
	}
	else if(strcmp(arg,"w") == 0){
	    printf("打印监视点信息：\n");
		WP *temp = GetHead();
		if(temp == NULL)
			printf("The watchpoint list is empyty!!!\n");
		while(temp)
		{
			printf("watchpoint %d: expr=\"%s\"\tresult = %u\n", temp->NO, temp->expr, temp->result);
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
			printf("表达式求值结果：\n");
			printf("\tHEX: 0x%x\t\tDEC: %"PRIu32"\n",temp,temp);
		}
		else{
			printf("The expression you input is invalid!!!\n");
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
		printf("成功创建监视点  序号：%d\t表达式：\"%s\"\n",temp->NO,temp->expr);
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
	if(args != NULL){
		printf("your input is invalid!!! (You can input just like \"bt\"\n");
		return 0;
	}
	int i = nr_symtab_entry;
    if(i <= 0){	
		printf("Stack is not exit!\n");
		return 0;
	}
	int num = 0;
	uint32_t temp_ebp = cpu.ebp;
	PartOfStackFrame temp;
	temp.ret_addr = 0;
	while(temp_ebp != 0)
	{
		temp.func_name[0] = '\0';
		temp.begin_addr = 0;
		temp.prev_ebp = swaddr_read(temp_ebp, 4);
		temp.cur_addr = temp.ret_addr ? temp.ret_addr : cpu.eip;
		for(--i ; i >= 0; i--){
			if(ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC && temp.cur_addr >= symtab[i].st_value && temp.cur_addr <= symtab[i].st_value + symtab[i].st_size){
				strcpy(temp.func_name, (char *)&strtab[symtab[i].st_name]);
				temp.begin_addr = symtab[i].st_value;
			}
		}
		if(temp_ebp + 4 >= 0x80000000)
			break;
		temp.ret_addr = swaddr_read(temp_ebp+4, 4);
		for(i = 0; i < 5; i++){
			if(temp_ebp + 8 + 4*i >= 0x80000000)
				while(i < 5)
					temp.args[i++] = 0;
			else
				temp.args[i] = swaddr_read(temp_ebp + 8 + 4*i, 4);
		}
		temp_ebp = temp.prev_ebp;
		printf("#%d   0x%08x in %s(%d,%d,%d,%d,%d)\n",num, temp.begin_addr, temp.func_name, temp.args[0],temp.args[1],temp.args[2],temp.args[3],temp.args[4]);
		num++;
	}
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
