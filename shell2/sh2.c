#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define M 10

	
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
	char *tmp = malloc(M);
	int n, k = 0;
	while (fgets(tmp+k, M, stdin)) {
		n = strlen(tmp);
		if (tmp == NULL){
			free(tmp);
			return NULL;
			}
		if (tmp[n-1] != '\n') {
			k += M-1;
			tmp = realloc(tmp, M+k);
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
	int symbst;
	char *buf = (char*)malloc(sizeof(char));
	int i = 0;
	strcpy(buf, "\0");
	while(str[i] != '\0'){
		if (str[i] == ' ') {
			while (str[i] == ' ') i++;
			if(strlen(buf) != 0){ 
				add_l(ls, buf);
				buf = init_buf();
				}
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

void print_m(char **m, int len){
	for (int i = 0; i < len+1; i++){
		if (m[i] != NULL)
		printf("%s\n", m[i]);
		else printf("N");
		}
	} 

	
int execute(list *words, char *s){
	char **wordsm = to_array(words, words->len);
	int status,rez;
	if (words->len != 0) {
	if (!strcmp(wordsm[0], "cd")) {
			if (words->len <= 2)
			if (words->len == 1) chdir(getenv("HOME"));
			else {
                rez = chdir(wordsm[1]);
                if (rez!=0) printf("directory not found \n");
            }
			else printf("cd has more then 1 arg \n");
            free(wordsm);
		}
	else {
		if (fork() > 0){
			wait(&status);
			free(wordsm);
			return status;
			}
		else {
			signal(SIGINT, SIG_DFL);
			execvp(wordsm[0], wordsm);
			printf("No such command %s\n", wordsm[0]);
			free_l(words);
			free(wordsm);
			free(s);
			exit(1);
			
			}
		}
		}
		return 0;
	}
	 
int main(void){
	signal(SIGINT, SIG_IGN);
	list *words;
	char *s;
	printf(">> ");
    s = get_s();
	while (s!=NULL){
		words = init();
		get_words(words, s);
		execute(words, s);
		free_l(words);
		free(s);
		printf(">> ");
        s = get_s();
	}
	return 0;
	}
