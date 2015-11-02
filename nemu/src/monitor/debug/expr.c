#include "nemu.h"
#include <inttypes.h>
#include <elf.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

extern char *strtab;
extern Elf32_Sym *symtab;
extern int nr_symtab_entry;

enum {
	NOTYPE = 256, EQ, NEQ, AND, OR, NOT, HEX, DEC, REG, NEGTIVE, VISIT, LL, LE, RR, RE, VARIABLE

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"-",  '-'},					// minus
	{"\\*", '*'},					// multiply
	{"/", '/'},						// divide
	{"==", EQ},						// equal
	{"!=", NEQ},					// not equal
	{"<=", LE},						// less or equal
	{"<", LL},						// less than
	{">=", RE},						// more or equal
	{">", RR},						// more than
	{"\\&{2}", AND},				// logic and
	{"\\|{2}", OR},					// logic or
	{"!", NOT},						// not
	{"\\(", '('},					// left parenthesis
	{")", ')'},						// right parenthesis
	{"0x[0-9a-fA-F]+",HEX},			// hexadecimal
	{"[0-9]+",DEC},					// decimal
	{"\\$[a-zA-Z]{2,3}",REG},		// register
	{"[a-zA-Z_]+[0-9a-zA-Z_]+", VARIABLE}	// variable
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )
#define op_num (NR_REGEX-4+3)        //去除 HEX, DEC, REG, NOTYPE, 加上 -(负号), *(取地址), #(辅助使用)

static struct reg_rule{
	char *name;
	int subscript;
	int offset;
	uint32_t and_num;
}reg_rules[] = {
	{"eax", 0, 0, 0xFFFFFFFF},
	{"ecx", 1, 0, 0xFFFFFFFF},
	{"edx", 2, 0, 0xFFFFFFFF},
	{"ebx", 3, 0, 0xFFFFFFFF},
	{"esp", 4, 0, 0xFFFFFFFF},
	{"ebp", 5, 0, 0xFFFFFFFF},
	{"esi", 6, 0, 0xFFFFFFFF},
	{"edi", 7, 0, 0xFFFFFFFF},
	{"ax", 0, 0, 0x0000FFFF},
	{"cx", 1, 0, 0x0000FFFF},
	{"dx", 2, 0, 0x0000FFFF},
	{"bx", 3, 0, 0x0000FFFF},
	{"sp", 4, 0, 0x0000FFFF},
	{"bp", 5, 0, 0x0000FFFF},
	{"si", 6, 0, 0x0000FFFF},
	{"di", 7, 0, 0x0000FFFF},
	{"ah", 0, 8, 0x000000FF},
	{"al", 0, 0, 0x000000FF},
	{"ch", 1, 8, 0x000000FF},
	{"cl", 1, 0, 0x000000FF},
	{"dh", 2, 8, 0x000000FF},
	{"dl", 2, 0, 0x000000FF},
	{"bh", 3, 8, 0x000000FF},
	{"bl", 3, 0, 0x000000FF},
	{"eip",0, 0, 0x00000000}
};
int nreg_rule = 25;
/*
//'+', '-', '*', '/', AND, OR, NOT, VISIT, NEGTIVE, EQ, NEQ, '(', ')', '#'
char priority_table[][op_num] = {
	{'>', '>', '<', '<', '>', '>', '<', '<', '<', '>', '>', '<', '>', '>'},		//'+'
	{'>', '>', '<', '<', '>', '>', '<', '<', '<', '>', '>', '<', '>', '>'},		//'-'
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '<', '>', '>'},		//'*'
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '<', '>', '>'},		//'/'
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>'},		//AND
	{'<', '<', '<', '<', '<', '>', '<', '<', '<', '<', '<', '<', '>', '>'},		//OR
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '<', '>', '>'},		//NOT
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '<', '>', '>'},		//VISIT
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '<', '>', '>'},		//NEGTIVE
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '>', '>', '<', '>', '>'},		//EQ
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '>', '>', '<', '>', '>'},		//NEQ
	{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '0'},		//'('
	{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '0', '>', '>'},		//')'
	{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '0', '='}		//'#'
};
*/

//'+', '-', '*', '/', AND, OR, NOT, VISIT, NEGTIVE, EQ, NEQ, LL, LE, RR, RE, '(', ')', '#'
char priority_table[][op_num] = {
	{'>', '>', '<', '<', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//'+'
	{'>', '>', '<', '<', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//'-'
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//'*'
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//'/'
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>'},		//AND
	{'<', '<', '<', '<', '<', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>'},		//OR
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//NOT
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//VISIT
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//NEGTIVE
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '>', '>'},		//EQ
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '>', '>'},		//NEQ
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//LL
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//LE
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//RR
	{'<', '<', '<', '<', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '>'},		//RE
	{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '0'},		//'('
	{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '0', '>', '>'},		//')'
	{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '0', '='}		//'#'
};

int get_subscript(int op)
{
	int n = 0;
	switch(op)
	{
		case '+': n = 0; break;
		case '-': n = 1; break;
		case '*': n = 2; break;
		case '/': n = 3; break;
		case AND: n = 4; break;
		case OR: n = 5; break;
		case NOT: n = 6; break;
		case VISIT: n = 7; break;
		case NEGTIVE: n = 8; break;
		case EQ: n = 9; break;
		case NEQ: n = 10; break;
		case LL: n = 11; break;
		case LE: n = 12; break;
		case RR: n = 13; break;
		case RE: n = 14; break;
		case '(': n = 15; break;
		case ')': n = 16; break;
		case '#': n = 17; break;
		default: break;
	}
	return n;
}

void strdown(char *str);

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret != 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case '+':
						if(nr_token == 0 || !(tokens[nr_token-1].type == DEC || tokens[nr_token-1].type == HEX || tokens[nr_token-1].type == REG
								|| tokens[nr_token-1].type == VARIABLE || tokens[nr_token-1].type == ')'))
							//tokens[nr_token].type = POSITIVE;
							break;
						else{
							tokens[nr_token].type = '+';
							strcpy(tokens[nr_token].str, "+");
							nr_token++;
							break;
						}
					case '-':
						if(nr_token == 0 || !(tokens[nr_token-1].type == DEC || tokens[nr_token-1].type == HEX || tokens[nr_token-1].type == REG
								|| tokens[nr_token-1].type == VARIABLE || tokens[nr_token-1].type == ')'))
                            tokens[nr_token].type = NEGTIVE; 
						else{ 
							tokens[nr_token].type = '-';	
                            strcpy(tokens[nr_token].str, "-"); 
				        }	
				        nr_token++; 
					    break;       
					case '*':
						if(nr_token == 0 || !(tokens[nr_token-1].type == DEC || tokens[nr_token-1].type == HEX || tokens[nr_token-1].type == REG
								|| tokens[nr_token-1].type == VARIABLE || tokens[nr_token-1].type == ')'))
							tokens[nr_token].type = VISIT;
						else{
							tokens[nr_token].type = '*';
							strcpy(tokens[nr_token].str, "*");
						}
						nr_token++;
						break;
					case '/': case '(': case ')': case EQ: case NEQ: case LL: case LE: case RR: case RE: 
					case AND: case OR: case NOT: case HEX: case DEC: case VARIABLE:
						tokens[nr_token].type = rules[i].token_type;
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						*(tokens[nr_token].str+substr_len) = '\0';
						nr_token++;
						break;
					case REG:
						tokens[nr_token].type = REG;
						strncpy(tokens[nr_token].str, substr_start+1, substr_len-1);
						*(tokens[nr_token].str + substr_len-1) = '\0';
						strdown(tokens[nr_token].str);
						nr_token++;
						break;
					case NOTYPE:
						break;
					default: panic("please implement me");
				}//switch
				break;
			}//if
		}//for

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}//while
	
	/*
	int num;
	for(num = 0; num<nr_token; num++)
	{
		printf("%s\n", tokens[num].str);
	}
	*/

	return true; 
}

//比较栈顶运算符和待入栈运算符的优先级，op1为栈顶运算符，op2为待入栈运算符
char compare(int op1, int op2){
	return priority_table[get_subscript(op1)][get_subscript(op2)];
}

void strdown(char *str)
{
	if(str == NULL)
		return;
	int i = 0;
	while(str[i] != '\0'){
		if(str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 'a' - 'A';
		i++;
	}
}

int64_t reg_fetch(int j){
	if(j == nreg_rule-1)
		return (int64_t)cpu.eip;
	else
		return (int64_t)((cpu.gpr[reg_rules[j].subscript]._32 >> reg_rules[j].offset) & reg_rules[j].and_num);
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	
	int64_t num_stack[16];
	Token op_stack[16];
	Token flag = {'#', "#"};
	int i = 0, j = 0;
	int type;
	tokens[nr_token] = flag;
	op_stack[0] = flag;
	int s1 = 0, s2 = 1;
    int64_t  op1, op2;

	while((tokens[i].type != '#' || op_stack[s2-1].type != '#') && s1>=0 && s2>=1)
	{
		switch(tokens[i].type)
		{
			case DEC:
				//sscanf(tokens[i].str, "%"SCNu32"" , &op1);
				sscanf(tokens[i].str, "%"SCNd64"" , &op1);
				num_stack[s1++] = op1;
				i++;
				break;
			case HEX:
				//sscanf(tokens[i].str, "0x%"SCNx32"", &op1);
				sscanf(tokens[i].str, "0x%"SCNx64"", &op1);
				num_stack[s1++] = op1;
				i++;
				break;
			case REG:
				op1 = 0;
				for(j = 0; j < nreg_rule; j++)
				{
					if(strcmp(tokens[i].str, reg_rules[j].name) == 0){
						op1 = reg_fetch(j);
						break;
					}
				}
				if(j == nreg_rule)
				{
					printf("表达式中出现了不合法的寄存器名!!!\n");
					*success = false;
					return 0;	
				}
				num_stack[s1++] = op1;
				i++;
				break;
			case VARIABLE:
				for(j = 0; j < nr_symtab_entry; j++){
					//if(symtab[j].st_info == STT_OBJECT){
					if(symtab[j].st_info == 1){
						if(strcmp(tokens[i].str, strtab+symtab[j].st_name) == 0){
							op1 = symtab[j].st_value;
							break;
						}
					}
				}	
				printf("Symtab_entry:%d\n", nr_symtab_entry);
				if(j == nr_symtab_entry){
					printf("表达式中出现了不合法的变量名!!!\n");
					*success = false;
					return 0;	
				}
				num_stack[s1++] = op1;
				i++;
				break;
			default:
				switch(compare(op_stack[s2-1].type, tokens[i].type))
				{
					case '<':
						op_stack[s2++] = tokens[i];
						i++;
						break;
					case '=':
						s2--;
						i++;
						break;
					case '>':
						//int type;
						type = op_stack[--s2].type;
						switch(type){
							case '+':
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
							   	num_stack[s1++] = op1 + op2; 
								break;
							case '-':
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
							   	num_stack[s1++] = op1 - op2; 
								break;
							case '*':
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
							   	num_stack[s1++] = op1*op2; 
								break;
							case '/': 
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
								num_stack[s1++] = op1/op2; 
								break;
							case AND:
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
								num_stack[s1++] = (op1 && op2);
								break;
							case OR:
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
								num_stack[s1++] = (op1 || op2);
								break;
							case NOT:								
								op1 = num_stack[--s1];
								num_stack[s1++] = (!op1);
								break;
							case VISIT:
								op1 = num_stack[--s1];
								num_stack[s1++] = swaddr_read(op1,4);
								break;
							case NEGTIVE:
								op1 = num_stack[--s1];
								num_stack[s1++] = -op1;
								break;
							case EQ:
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
								num_stack[s1++] = (op1 == op2); 
								break;
							case NEQ:
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
								num_stack[s1++] = (op1 != op2); 
								break;
							case LL:
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
								num_stack[s1++] = (op1 < op2); 
								break;
							case LE:
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
								num_stack[s1++] = (op1 <= op2); 
								break;
							case RR:
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
								num_stack[s1++] = (op1 > op2); 
								break;
							case RE:
								op2 = num_stack[--s1];
								op1 = num_stack[--s1];
								num_stack[s1++] = (op1 >= op2); 
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
				break;
		}//switch
	}//while
	if(s1 == 1 && s2 == 1){
		*success = true;
		return (uint32_t)num_stack[--s1];
	}
	else
	{
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	//return 0;
}

