#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum TypeVar {
    none,
    intType,
	floatType
};

typedef struct {
	int type;
	int iVal;
	float fVal;
	char name[10];
} var32;

var32 *varArr = NULL;
int varArrSize = 0;
FILE *in;
int isdigit_str(char *str)
{
	while (*str != 0 && *str != '\n') {
		if (!isdigit(*str) && *str != '-') {
			return 0;
		}
		str++;
	}
	return 1;
}

int isfloat_str(char *str)
{
	int check = 0;
	while (*str != 0 && *str != '\n') {
		if (!isdigit(*str)) {
			if (*str == '.') {
				check++;
			} else {
				return 0;
			}
		}
		str++;
	}
	if (check == 1) {
	return 1;
	} else {
		return 0;
	}
}

var32 *findVar(char *name_var) {
	if (isdigit_str(name_var)) {
		return NULL;
	}
	if (name_var[strlen(name_var) - 1] == '\n') {
		name_var[strlen(name_var) - 1] = 0;
	}
	
	if (varArr == NULL) {
		varArr = calloc(1, sizeof(var32));
		varArrSize = 1;
		var32 new_var;
		new_var.type = 1;
		new_var.iVal = 0;
		strncpy(new_var.name, name_var, 10);
		varArr[0] = new_var;
		return &(varArr[0]);
	}
	var32 *var_p = NULL;
	for (int i = 0; i < varArrSize; i++) {
		if(!strcmp(varArr[i].name, name_var)) {
			var_p = &(varArr[i]);
			break;
		}
	}
	if (var_p == NULL) {
		varArr = realloc(varArr, (varArrSize + 1) * sizeof(var32));
		var32 new_var;
		new_var.type = 1;
		new_var.iVal = 0;
		strncpy(new_var.name, name_var, 10);
		varArr[varArrSize] = new_var;
		var_p = &(varArr[varArrSize]);
		varArrSize++;
	}
	return var_p;
}

void setValVar(var32 *var, float val, int type)
{
	var->type = type;
	switch(type) {
	case intType:
		var->iVal = (int)val;
	case floatType:
		var->fVal = val;
	default:
		return;
	}
}

int getTypeVar(var32 *var)
{
	if (var == NULL)
	{
		return none;
	}
	return var->type;
}

void *getValVar(var32 *var)
{
	switch(getTypeVar(var)) {
	case intType:
		return &(var->iVal);
	case floatType:
		return &(var->fVal);
	default:
		return NULL;
	}
}

void freeArrVar()
{
	free(varArr);
}

char **tokenize(char *str, int *token_num)
{
	char **tokens = calloc(10, sizeof(char*));
	int tok_p = 0;
	tokens[tok_p] = strtok(str, " ");
	tok_p++;
	while (tokens[tok_p - 1] != NULL) {
		//printf("%s\n", tokens[tok_p - 1]);
		tokens[tok_p] = strtok (NULL, " ");
		tok_p++;
	}
	*token_num = tok_p - 1;
	return tokens;
}

int condition(char *var1, char *cond, char *var2) 
{
	float left;
	float right;
	if (isdigit_str(var1)) {
		left = atoi(var1);
	} else {
		if (getTypeVar(findVar(var1)) == intType) {
			left = (float)*(int*)getValVar(findVar(var1));
		} else {
			left = *(float*)getValVar(findVar(var1));
		}
	}
	if (isdigit_str(var2)) {
		right = atoi(var2);
	} else {
		if (getTypeVar(findVar(var2)) == intType) {
			right = (float)*(int*)getValVar(findVar(var2));
		} else {
			right = *(float*)getValVar(findVar(var2));
		}
	}
	
	if (!strcmp(cond, ">")) {
		return left > right;
	} else if (!strcmp(cond, "<")) {
		return left < right;
	} else if (!strcmp(cond, "==")) {
		return left == right;
	} else if (!strcmp(cond, ">=")) {
		return left >= right;
	} else if (!strcmp(cond, "<=")) {
		return left <= right;
	}
	return 0;
}

int nest_num = 0;
int interpritate_str(FILE *in) 
{
	char buff[60];
	while (fgets(buff, 60, in) != NULL) {
		if (buff[strlen(buff) - 1] == '\n') {
			buff[strlen(buff) - 1] = 0;
		}
		while (buff[0] == '\t') {
			for (int i = 1; i < strlen(buff); i++) {
				buff[i - 1] = buff[i];
			}
			buff[strlen(buff) - 1] = 0;
		}
		//puts(buff);
		int token_num;
		char **tokens = tokenize(buff, &token_num);
		//printf("num token:%d\n", token_num);
		if (token_num == 0) {
			return 0;
		}
		if (!strcmp(tokens[0], "done")) {
			//printf("!%d!\n", nest_num);
			return 2;
		}
		if (!strcmp(tokens[0], "fi")) {
			//printf("!%d!\n", nest_num);
			return 3;
		}
		if (!strcmp(tokens[0], "print")) {
			if (isdigit_str(tokens[1])) {
				printf("%s\n", tokens[1]);
			} else {
				var32 *tmp = findVar(tokens[1]);
				//printf("%d\n", tmp->type);
				if (tmp->type == intType) {
					printf("%d\n", tmp->iVal);
				} else if (tmp->type == floatType) {
					printf("%g\n", tmp->fVal);
				}
			}
		}
		if (!strcmp(tokens[0], "read")) {
			int type = none;
			char *var_name;
			if (token_num == 3) {
				if (strcmp(tokens[1], "-i")) {
					type = intType;
				} 
				if (strcmp(tokens[1], "-f")) {
					type = floatType;
				}
				var_name = tokens[2];
			} else {
				var_name = tokens[1];
			}
			char var_buff[60];
			printf("%s = ", var_name);
			scanf("%s", var_buff);
			if (type == none) {
				if (isdigit_str(var_buff)) {
					setValVar(findVar(var_name), atoi(var_buff), intType);
				} else if (isfloat_str(var_buff)) {
					setValVar(findVar(var_name), atof(var_buff), floatType);
				} else {
					return 1;
				}
			} else {
				setValVar(findVar(var_name), atof(var_buff), type);
			}
			
		}
		if (!strcmp(tokens[0], "if")) {
			nest_num++;
			if (condition(tokens[1], tokens[2], tokens[3])) {
				if (interpritate_str(in) == 3) {
					
				}
			} else {
				int need_skip = 1;
				while (fgets(buff, 60, in) != NULL) {
					if (buff[strlen(buff) - 1] == '\n') {
						buff[strlen(buff) - 1] = 0;
					}
					while (buff[0] == '\t') {
						for (int i = 1; i < strlen(buff); i++) {
							buff[i - 1] = buff[i];
						}
						buff[strlen(buff) - 1] = 0;
					}
					tokens = tokenize(buff, &token_num);
					//puts(buff);
					if (!strcmp(tokens[0], "if")) {
						need_skip++;
					}
					tokens = tokenize(buff, &token_num);
					if (!strcmp(tokens[0], "fi")) {
						need_skip--;
					}
					if (need_skip == 0) {
						break;
					}
				} 
			}
			nest_num--;
			continue;
		}
		if (!strcmp(tokens[0], "while")) {
			nest_num++;
			long int num = ftell(in);
			if (condition(tokens[1], tokens[2], tokens[3])) {
				while (condition(tokens[1], tokens[2], tokens[3])) {
					if (interpritate_str(in) == 2) {
						//printf("!!%ld!!\n", num);
						fseek(in, num, SEEK_SET); 
					}
				}
			}
			int need_skip = 1;
			while (fgets(buff, 60, in) != NULL) {
				if (buff[strlen(buff) - 1] == '\n') {
					buff[strlen(buff) - 1] = 0;
				}
				while (buff[0] == '\t') {
					for (int i = 1; i < strlen(buff); i++) {
						buff[i - 1] = buff[i];
					}
					buff[strlen(buff) - 1] = 0;
				}
				tokens = tokenize(buff, &token_num);
				//puts(buff);
				if (!strcmp(tokens[0], "while")) {
					need_skip++;
				}
				tokens = tokenize(buff, &token_num);
				if (!strcmp(tokens[0], "done")) {
					need_skip--;
				}
				if (need_skip == 0) {
					break;
				}
			} 
			nest_num--;
			continue;
		}
		if (!strcmp(tokens[1], "=") && !isdigit_str(tokens[0])) {
			if (token_num == 3) {
				if (isdigit_str(tokens[2])) {
					setValVar(findVar(tokens[0]), atoi(tokens[2]), intType);
				} else if (isfloat_str(tokens[2])) {
					setValVar(findVar(tokens[0]), atof(tokens[2]), floatType);
				} else {
					if (getTypeVar(findVar(tokens[2])) == intType) {
						setValVar(findVar(tokens[0]), *(int*)getValVar(findVar(tokens[2])), intType);
					} else {
						setValVar(findVar(tokens[0]), *(float*)getValVar(findVar(tokens[2])), floatType);
					}
				}
			}
			if (token_num == 5) {
				float left;
				float right;
				if (isdigit_str(tokens[2])) {
					left = atoi(tokens[2]);
				} else {
					if (getTypeVar(findVar(tokens[2])) == intType) {
						left = (float)*(int*)getValVar(findVar(tokens[2]));
					} else {
						left = *(float*)getValVar(findVar(tokens[2]));
					}
				}
				if (isdigit_str(tokens[4])) {
					right = atoi(tokens[4]);
				} else {
					if (getTypeVar(findVar(tokens[4])) == intType) {
						right = (float)*(int*)getValVar(findVar(tokens[4]));
					} else {
						right = *(float*)getValVar(findVar(tokens[4]));
					}
				}
				//printf("!%f %f!\n", left, right);
				if (strstr(tokens[3], "+")) {
					int Type_tmp;
					if (getTypeVar(findVar(tokens[2])) == floatType || getTypeVar(findVar(tokens[4])) == floatType) {
						Type_tmp = floatType;
					} else {
						Type_tmp = intType;
					}
					setValVar(findVar(tokens[0]), left + right, Type_tmp);
				}
			}
		}
	}
	return 0;
}

int main()
{
	in = fopen("test.txt", "r");
	if (in == NULL) {
		return 1;
	}
	if (interpritate_str(in)) {
		return 0;
	}
	freeArrVar();
	fclose(in);
	return 0;
}