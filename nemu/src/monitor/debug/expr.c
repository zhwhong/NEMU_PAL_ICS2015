#include "nemu.h"
#include <inttypes.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, HEX, DEC, REG, EQ, POSITIVE, NEGTIVE, VISIT

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
	{"\\(", '('},					// left parenthesis
	{")", ')'},						// right parenthesis
	{"0x[0-9a-fA-F]+",HEX},			// hexadecimal
	{"[0-9]+",DEC},					// decimal
	{"\\$[a-zA-Z]{2,3}",REG},			// register
	{"==", EQ}						// equal

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

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
	{"bl", 3, 0, 0x000000FF}
};
int nreg_rule = 24;

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
						if(nr_token == 0 || tokens[nr_token-1].type == '+' || tokens[nr_token-1].type == '-' || tokens[nr_token-1].type == '*'
								|| tokens[nr_token-1].type == '/' || tokens[nr_token-1].type == '(')
							tokens[nr_token].type = POSITIVE;
						else{
							tokens[nr_token].type = '+';
							strcpy(tokens[nr_token].str, "+");
						}
						nr_token++;
						break;
					case '-':
						if(nr_token == 0 || tokens[nr_token-1].type == '+' || tokens[nr_token-1].type == '-' || tokens[nr_token-1].type == '*' 
								|| tokens[nr_token-1].type == '/' || tokens[nr_token-1].type == '(')
                            tokens[nr_token].type = NEGTIVE; 
						else{ 
							tokens[nr_token].type = '-';	
                            strcpy(tokens[nr_token].str, "-"); 
				        }	
				        nr_token++; 
					    break;       
					case '*':
						if(nr_token == 0 || tokens[nr_token-1].type == '+' || tokens[nr_token-1].type == '-' || tokens[nr_token-1].type == '*'
								|| tokens[nr_token-1].type == '/' || tokens[nr_token-1].type == '(')
							tokens[nr_token].type = VISIT;
						else{
							tokens[nr_token].type = '*';
							strcpy(tokens[nr_token].str, "*");
						}
						nr_token++;
						break;
					case '/':
						tokens[nr_token].type = '/';
						strcpy(tokens[nr_token].str, "/");
						nr_token++;
						break;		
					case '(':
						tokens[nr_token].type = '(';
						strcpy(tokens[nr_token].str, "(");
						nr_token++;
						break;
					case ')':
						tokens[nr_token].type = ')';
						strcpy(tokens[nr_token].str, ")");
						nr_token++;
						break;
					case HEX:
					case DEC:
						tokens[nr_token].type = rules[i].token_type;
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						*(tokens[nr_token].str+substr_len) = '\0';
						nr_token++;
						break;
					case REG:
						tokens[nr_token].type = REG;
						strncpy(tokens[nr_token].str, substr_start+1, substr_len-1);
						*(tokens[nr_token].str + substr_len) = '\0';
						strdown(tokens[nr_token].str);
						nr_token++;
						break;
					case EQ:

						break;
					case NOTYPE:

						break;
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	
	int k;
	for(k = 0; k < nr_token; k++){
		printf("%d\t%s\n", tokens[k].type, tokens[k].str);
	}

	return true; 
}

int compare(char op1, char op2){
	switch(op1)
	{
		case '+':
		case '-':
			switch(op2)
			{
				case '+': case '-': case ')': case '#':
					return 1;
				case '*': case '/': case '(':
					return -1;
				default:
					return -2;
			}
		case '*':
		case '/':
			switch(op2)
			{
				case '+': case '-': case '*': case '/': case ')': case '#':
				    return 1;
			    case '(':
					return -1;
				default:
					return -2;		
			}
		case '(':
			switch(op2)
			{
				case '+': case '-': case '*': case '/': case '(':
					return -1;
				case ')':
					return 0;
				default:
					return -2;
			}
		case ')':
			switch(op2)
			{
				case '+': case '-': case '*': case '/': case ')': case '#':
					return 1;
				default:
					return -2;
			}
		case '#':
			switch(op2)
			{
				case '+': case '-': case '*': case '/': case '(':
					return -1;
				case '#':
					return 0;
				default:
					return -2;
			}
		default:
			return -2;
	}
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

uint32_t reg_fetch(int j){
	return (uint32_t)((cpu.gpr[reg_rules[j].subscript]._32 >> reg_rules[j].offset) & reg_rules[j].and_num);
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	
	uint32_t num_stack[16];
	Token op_stack[16];
	Token flag = {'#', "#"};
	int i = 0, j = 0;
	int type;
	tokens[nr_token] = flag;
	op_stack[0] = flag;
	int s1 = 0, s2 = 1;
    uint32_t  op1, op2;

	while(tokens[i].type != '#' || op_stack[s2-1].type != '#')
	{
		switch(tokens[i].type)
		{
			case DEC:
				sscanf(tokens[i].str, "%"SCNu32"" , &op1);
				num_stack[s1++] = op1;
				i++;
				break;
			case HEX:
				sscanf(tokens[i].str, "0x%"SCNx32"", &op1);
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
				num_stack[s1++] = op1;
				i++;
				break;
			default:
				switch(compare(op_stack[s2-1].type, tokens[i].type))
				{
					case -1:
						op_stack[s2++] = tokens[i];
						i++;
						break;
					case 0:
						s2--;
						i++;
						break;
					case 1:
						//int type;
						type = op_stack[--s2].type;
						op2 = num_stack[--s1];
						op1 = num_stack[--s1];
						switch(type){
							case '+': num_stack[s1++] = op1 + op2; break;
							case '-': num_stack[s1++] = op1 - op2; break;
							case '*': num_stack[s1++] = op1*op2; break;
							case '/': num_stack[s1++] = op1/op2; break;
						}
						break;
					default:
						break;
				}
				break;
		}//switch
	}//while
	*success = true;
	return num_stack[s1-1];
	
	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	//return 0;
}

