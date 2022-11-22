#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <signal.h>

#define MEMORY 10

	
typedef struct node
{
    struct node *next;
    char *word;
}node;

typedef struct list
{
	int len;
    struct node *first, *last;
}list;


list *init(void) {
	list *tmp = (list*)malloc(sizeof(list));
	tmp->last = tmp->first = NULL;
	tmp->len = 0;
	return tmp;
	}

void add_l(list *ls, char *word) {
	node *tmp = (node*)malloc(sizeof(node));
	tmp->next = NULL;
	tmp->word = word;
	if (ls->last){
		ls->last->next = tmp;
	}
	ls->last = tmp;
	if (ls->first == NULL){
		ls->first = tmp;
	}
	ls->len++;
	}
	
void print_l(list *sp) {
	node *tmp = sp->first;
	while (tmp) {
		printf("%s\n", tmp->word);
		tmp = tmp->next;
		}
	}
	
char *get_s(void){
	char *tmp = malloc(MEMORY);
	int n, k = 0;
	while (fgets(tmp+k, MEMORY, stdin)) {
		n = strlen(tmp);
		if (tmp == NULL){
			free(tmp);
			return NULL;
		}
		if (tmp[n-1] != '\n') {
			k += MEMORY-1;
			tmp = realloc(tmp, MEMORY+k);
		}
		else {
			tmp[n-1] = '\0';
			return tmp;
		}
		
		}
	free(tmp);
	return NULL;
	}

char *append_s(char *str, char sym){
	int wordlen = strlen(str);
	str = realloc(str, wordlen + 2);
	str[wordlen] = sym;
	str[wordlen+1] = '\0';
	return str;
	}
	
char *init_buf(void) {
	char *buf = (char*)malloc(sizeof(char));
	strcpy(buf, "\0");
	return buf;
	}
	
void get_words(list *ls, char *str){
	int symb = 0, symbst, symben;
	char *buf = (char*)malloc(sizeof(char));
	int i = 0, wordlen = 0, spacec = 0;
	strcpy(buf, "\0");
	while(str[i] != '\0'){
		if (str[i] == ' ') {
			while (str[i] == ' ') i++;
			if(strlen(buf) != 0){ 
				add_l(ls, buf);
				buf = init_buf();
			}
			wordlen = 0;
		}
		else {
			if (str[i] == '"'){
				symbst = i+1;
				i++;
				while((str[i] != '\0') && (str[i] != '"')){
					i++;
					}
				if (str[i] == '"') {
					for (int j = symbst; j < i; j++){
						if (str[j] == ' ') {
							buf = append_s(buf, str[j]);
							while (str[j] == ' ') j++;
							}
						if (str[j] != '"') buf = append_s(buf, str[j]);
						}
						i++;
				}
				else {
					buf = append_s(buf, str[symbst - 1]);
					i = symbst;
				}
			}
				
			else {
			if ((str[i] == '&') || (str[i] == '|') || (str[i] == '(') || (str[i] == ';') 
				|| (str[i] == '>') || (str[i] == '<') || (str[i] == ')')) {
				if (str[i] == '&') {
					if (str[i+1] == '&') {
						if(strlen(buf) != 0){
							add_l(ls, buf);
							buf = init_buf();
						}
						buf = append_s(buf, str[i]);
						buf = append_s(buf, str[i+1]);
						add_l(ls, buf);
						buf = init_buf();
						i+=2;
					}
					else{
					if(strlen(buf) != 0) {
						add_l(ls, buf);
						buf = init_buf();
					}
					buf = append_s(buf, str[i]);
					add_l(ls, buf);
					buf = init_buf();
					i++;
				}
				} 
				
				if (str[i] == '|') {
					if (str[i+1] == '|') {
						if(strlen(buf) != 0){
							add_l(ls, buf);
							buf = init_buf();
						}
						buf = append_s(buf, str[i]);
						buf = append_s(buf, str[i+1]);
						add_l(ls, buf);
						buf = init_buf();
						i+=2;
					}
					else{
					if(strlen(buf) != 0) {
						add_l(ls, buf);
						buf = init_buf();
						}
					buf = append_s(buf, str[i]);
					add_l(ls, buf);
					buf = init_buf();
					i++;
				}
				}
			
				if (str[i] == '(') {
					if(strlen(buf) != 0) {
						add_l(ls, buf);
						buf = init_buf();
					}
					buf = append_s(buf, str[i]);
					add_l(ls, buf);
					buf = init_buf();
					i++;
				} 
				if (str[i] == ')') {
					if(strlen(buf) != 0) {
						add_l(ls, buf);
						buf = init_buf();
					}
					buf = append_s(buf, str[i]);
					add_l(ls, buf);
					buf = init_buf();
					i++;
				
				}
				if (str[i] == ';') {
					if(strlen(buf) != 0) {
						add_l(ls, buf);
						buf = init_buf();
					}
					buf = append_s(buf, str[i]);
					add_l(ls, buf);
					buf = init_buf();
					i++;	
				}
				if (str[i] == '<') {
					if (str[i+1] == '<') {
						if(strlen(buf) != 0){
							add_l(ls, buf);
							buf = init_buf();
						}
						buf = append_s(buf, str[i]);
						buf = append_s(buf, str[i+1]);
						add_l(ls, buf);
						buf = init_buf();
						i+=2;
					}
					else{
					if(strlen(buf) != 0) {
						add_l(ls, buf);
						buf = init_buf();
					}
					buf = append_s(buf, str[i]);
					add_l(ls, buf);
					buf = init_buf();
					i++;
				    }
				}
				
				if (str[i] == '>') {
					if (str[i+1] == '>') {
						if(strlen(buf) != 0){
							add_l(ls, buf);
							buf = init_buf();
						}
						buf = append_s(buf, str[i]);
						buf = append_s(buf, str[i+1]);
						add_l(ls, buf);
						buf = init_buf();
						i+=2;
					}
					else{
					if(strlen(buf) != 0) {
						add_l(ls, buf);
						buf = init_buf();
					}
					buf = append_s(buf, str[i]);
					add_l(ls, buf);
					buf = init_buf();
					i++;
				    }
				}    
			}
			else {	
			buf = append_s(buf, str[i]);
			i++;
			}	
				}
			}
		}
	if (strlen(buf) != 0){
		add_l(ls, buf);
	}
	else 
	free(buf);
	}
	
int length(char **arr){
	int i = 0;
	while (arr[i] != NULL){
		i++;
		}
	return i;
	}
	
void free_l(list *ls){
	node *del = ls->first, *tmp;
	while(del){
		tmp = del->next;
		free(del->word);
		free(del);
		del = tmp;
		}
	free(ls);
	}

char **to_array(list *words, int len){
	char **wordsm = malloc(sizeof(char*)*(len+1));
	int i = 0;
	node *tmp = words->first;
	while(tmp){
		wordsm[i] = tmp->word;
		i++;
		tmp = tmp->next;
		}
	wordsm[i] = (char*)0;
	return wordsm;
	}

void print_m(char **m){ 
	int i = 0;
	while (m[i] != NULL){
		printf("%s\n", m[i++]);
		}
	} 
	
void print_c(char ***coms, int len){
	for(int i = 0; i < len; i++){
		print_m(coms[i]);
		printf("\n");
		}
	}


char ***to_commands(char **words, int *len1){
	char ***commands = (char***)malloc(sizeof(char**));
	int i = 0, j = 0, k = 0, len = *len1, l = 0, rein[4] = {};
	
	while (k < len){
		if (!strcmp(words[k], "|")) {
			k++;
			if (!rein[0] && !rein[1] && !rein[2] && !rein[3]){
				commands[i] = realloc(commands[i], sizeof(char**) * (j + 1));
				commands[i][j] = NULL;
				j=0;
				i++;
				commands = realloc(commands, sizeof(char**) * (i + 1));
			}
			else rein[0] = rein[1] = rein[2] = rein[3] = 0;
			
		}
		else {
			if (j == 0) {
				l++;
				commands[i] = (char**)malloc(sizeof(char*));
			}
			else commands[i] = realloc(commands[i], sizeof(char**) * (j + 1));
			
			if ((rein[0] = !strcmp(words[k], "<")) || (rein[1] = !strcmp(words[k], ">")) ||
			   (rein[2] = !strcmp(words[k], ">>")) || (rein[3] = !strcmp(words[k], "&"))) {
					if (rein[3] == 0) {
					if (j != 0) {
						commands[i] = realloc(commands[i], sizeof(char**) * (j + 1));
						commands[i][j] = NULL;
						l++;
						j=0;
						i++;
						commands = realloc(commands, sizeof(char**) * (i + 1));
					}
					commands[i] = realloc(commands[i], sizeof(char**) * 3);
					commands[i][0] = words[k]; 
					commands[i][1] = words[k+1];
					commands[i][2] = NULL; 
					k += 2;
					if (k != len) {
						i++;
						commands = realloc(commands, sizeof(char**) * (i + 1));
						}
					else{
						j+=2;
						}
					}
					
					if ((j >= 1) && rein[3]){
						commands[i] = realloc(commands[i], sizeof(char**) * (j + 1));
						commands[i][j] = words[k];
						if ((k+1) != len){
							commands[i] = realloc(commands[i], sizeof(char**) * (j + 1));
							commands[i][j+1] = NULL;
						}
						else j++;						
						if ((k+1) != len) {
							j=0;
							i++;
							commands = realloc(commands, sizeof(char**) * (i + 1));
						}
						k++;
				    }
			}
				
			else {	
				commands[i][j] = words[k];
				k++;
				j++;
			}
		}
		}
		commands[i] = realloc(commands[i], sizeof(char**) * (j + 1));
		commands[i][j] = NULL;
		*len1 = l;
		return commands;	
	}

void free_arr(char ***arr, int len){
	for (int i = 0; i < len; i++){
		free(arr[i]);
		}
	free(arr);
	}
	
int get_backmode(char ***commands, int len){
	int backmode = 0, j = 0;
	for (int i = 0; i < len; i++){
		while(commands[i][j]){
			if (!strcmp(commands[i][j], "&")) {
				backmode = 1;
				commands[i][j] = NULL;
				}
			j++;
			}
			j=0;
		}
	return backmode;
	}
	
int execute(char ***commands, int len, int backmode){
	signal(SIGINT, SIG_DFL);
    int fd[2];
	int tmp = dup(0);
	char ***ncoms;
	int i, len1 = len;
	for (i = 0; i < len; i++) {
		pipe(fd);
		switch(fork()){
			case -1: break;
			case 0:	
				if ((i+1) != len) dup2(fd[1], 1);
					close(fd[0]);
					close(fd[1]);
				execvp(commands[i][0], commands[i]);
				exit(1);
				break;
			
			default: 
				dup2(fd[0], 0);
				close(fd[0]);
				close(fd[1]);		
			}		
		}
		dup2(tmp, 0);
		close(tmp);	
		if (backmode)
			exit(0);
		while(wait(NULL) != -1);
		free(ncoms);
	return 0;	
	}
	
void find_rein(char ***commands, int *len){
	int nlen = *len, k = 0, status;
	int fd[2], rein[3] = {}, backmode = 0;
	char *input = NULL, *output = NULL;
	
	char ***ncoms = (char***)malloc(sizeof(char**) * (*len));
	for (int i = 0; i < *len; i++){
		if ((rein[0] = !strcmp(commands[i][0], "<")) || (rein[1] = !strcmp(commands[i][0], ">")) ||
		   (rein[2] = !strcmp(commands[i][0], ">>"))){
			if (rein[0]) {
				input = commands[i][1];
				nlen--;
			}
			if (rein[1]) {
				output = commands[i][1];
				nlen--;
			}
			if (rein[2]) {
				output = commands[i][1];
				nlen--;
			}
			
			}
		else {
			ncoms[k] = commands[i];
			k++;
			} 
		}
		backmode = get_backmode(ncoms, nlen);
			switch(fork()) {
			case 0:
				if (input) {
				fd[0] = open(input, O_RDONLY);
				dup2(fd[0], 0);
				close(fd[0]);
			}
			if (output) {
				fd[1] = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				dup2(fd[1], 1);
				close(fd[1]);
			}
			if (output) {
				fd[1] = open(output, O_WRONLY | O_CREAT | O_APPEND, 0666);
				dup2(fd[1], 1);
				close(fd[1]);
			}
			if (backmode){
				fd[0] = open("/dev/null", O_RDONLY);
				dup2(fd[0], 0);
				close(fd[0]);
				close(fd[1]);
			}
			execute(ncoms, nlen, backmode);
			printf("!\n");
			exit(1);
			default: 
				wait(&status);
			}
			free(ncoms);
	}

int main(void){
    signal(SIGINT, SIG_IN);
	list *words;
	int comlen = 0, len1;
	char **wordsm, ***coms, ***ncoms;
	char *s;
	printf(">> ");
	while (s = get_s()){
		words = init();
		get_words(words, s);
		wordsm = to_array(words, words->len);
		len1 = words->len;
		coms = to_commands(wordsm, &len1);
		if (!strcmp(wordsm[0], "cd")){
            if (words->len <= 2)
            if (words->len == 1) chdir(getenv("HOME"));
            else {
               int rez = chdir(wordsm[1]);
                if (rez!=0) printf("directory not found \n");
            }
            else printf("cd has more than 1 argument");      
        }
        else find_rein(coms, &len1);
		free_arr(coms, len1);	
		free(wordsm);
		free_l(words);
		free(s);
		printf(">> ");
	}
	return 0;
	}
