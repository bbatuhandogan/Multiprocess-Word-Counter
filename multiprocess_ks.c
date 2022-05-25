#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdint.h>

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"

#define KEND "\e[0m"
#define PAGESIZE 8192

int main(int argc, char* argv[]) {

	uint32_t *shared_parent_id = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*shared_parent_id = getpid();
	
	uint32_t *shared_karakter_32 = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	uint32_t *shared_kelime_32 = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	uint32_t *shared_satir_32 = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	uint32_t *shared_line_32 = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	
	char *fileArr[20] = {0};
	char *argArr[20] = {0};
	char *filePath = 0;
	
	int indexFile = 0;
	for(int argcnt = 0; argcnt < argc; argcnt++) {
		if (strstr(argv[argcnt], ".txt") != NULL || strstr(argv[argcnt], "+") != NULL) {
			fileArr[indexFile++] = argv[argcnt];
		}
	}
		
	int indexArg = 0;
	for(int argcnt = 0; argcnt < argc; argcnt++) {
		if (strstr(argv[argcnt], "-") != NULL) {
			argArr[indexArg++] = argv[argcnt];
		}
	}
	
	FILE *fp;
	
	int res = 0;
	int boyut;
	if (*fileArr == NULL) boyut = 0;
	else if (*fileArr != NULL && *(fileArr +1) == NULL) boyut = 1;
	else boyut = 2;

	if(boyut > 1)
		res = fork();
	else if (boyut == 1)
		res = -1;
	else if (boyut == 0) {
		fp = stdin;
		if(fp == NULL) {
			fprintf(stderr, "%sHATA: sitdin'de bulunan dosya açılamadı\n%s", KRED, KEND);
			return 0;
		}
		res = -2;
	}
	if (res != -2) {
		filePath = fileArr[0];
		if (strcmp(filePath, "+") == 0) {
			fp = stdin;
		}
		if (strcmp(filePath, "+") == 0 && fp == NULL){
			fprintf(stderr, "%sHATA: stdin'deki dosya açılamadı\n%s", KRED, KEND);
			*shared_parent_id = res;
			wait(NULL);
			_exit(0);
		}
		if(strcmp(filePath, "+") != 0) {
			if(fp = fopen(fileArr[0],"r"))
				;
			else {
				fprintf(stderr, "%sHATA: %s dosyası açılamadı\n%s", KRED, filePath, KEND);
				*shared_parent_id = res;
				wait(NULL);
				_exit(0);

			}
		}
	}
	for (int i = 1; i<(indexFile); i++){
		if(res == 0){
			filePath = fileArr[i];
			if (strcmp(filePath, "+") == 0){
				fp = stdin;
			}
			if(strcmp(filePath, "+") == 0 && fp == NULL) {
				if ((indexFile-i) > 1) {
					res = fork();
				}
				if(res != 0 || indexFile == 2){
					fprintf(stderr, "%sHATA: sitdin'de bulunan dosya açılamadı\n%s", KRED, KEND);
					if (*shared_parent_id != getppid()) {
						*shared_parent_id = res;
					}
					wait(NULL);
				}
				continue;
			}
			
			if (strcmp(filePath, "+") != 0){
				if(fp = fopen(fileArr[i],"r")) {
					;
				}
				else {
					if ((indexFile-i) > 1) {
						res = fork();
					}

					if (res != 0 || indexFile == 2) {
						fprintf(stderr, "%sHATA: %s dosyası açılamadı\n%s", KRED, filePath, KEND);
						if (*shared_parent_id != getppid()) {
							*shared_parent_id = res;
						}
						wait(NULL);
					}
					continue;
				}
			}
			if ((indexFile-i) != 1)
				res = fork();
		}	
	}
	int karakter = 0, kelime = 0, satir = 0;
	char cTemp;
	
	do{
		char c = fgetc(fp);
		if (feof(fp))
			break;
		
		if((c=='\t' || c == ' ' || c=='\n') && !(cTemp=='\t' || cTemp == ' ' || cTemp=='\n') ) { kelime++; *shared_kelime_32 += 1;}
		
		if(c == '\n') { satir++; *shared_satir_32 += 1; *shared_line_32 += 1;}
		
		karakter++;
		*shared_karakter_32 += 1;
		
		cTemp = c;
	} while(1);

	if (*shared_parent_id == getpid()) wait(NULL);

	if (*argArr == NULL){
		if (filePath != NULL && strcmp(filePath, "+") != 0) {
			printf("[%d] %s(satir/kelime/karakter): %d, %d, %d\n", getpid(), filePath, satir, kelime, karakter);
		}
		else {
			printf("[%d] %s-stdin-%s(satir/kelime/karakter): %d, %d, %d\n", getpid(), KGRN, KEND, satir, kelime, karakter);
			
		}
		if (*shared_parent_id == getpid()) {
			printf("%sToplam(satir/kelime/karakter): %d, %d, %d\n%s", KYEL, *shared_satir_32, *shared_kelime_32, *shared_karakter_32, KEND);
		}
	}
	
	
	else {
		for(int i = 0; i<3; i++) {
			if (argArr[i] == NULL) {
				continue;
			}
			if (strcmp(argArr[i], "-l") == 0) {
				if (filePath != NULL && strcmp(filePath, "+") != 0) 
					printf("[%d] %s(satir): %d\n", getpid(), filePath, satir);
				else 
					printf("[%d] %s-stdin-%s(satir): %d\n", getpid(), KGRN, KEND, satir);
			
				if (*shared_parent_id == getpid())
					printf("%sToplam(satir): %d\n%s", KYEL, *shared_satir_32, KEND);
				continue;
			}
			if (strcmp(argArr[i], "-w") == 0) {
				if (filePath != NULL && strcmp(filePath, "+") != 0) 
					printf("[%d] %s(kelime): %d\n", getpid(), filePath, kelime);
				else 
					printf("[%d] %s-stdin-%s(kelime): %d\n", getpid(), KGRN, KEND, kelime);
		
				if (*shared_parent_id == getpid())
					printf("%sToplam(kelime): %d\n%s", KYEL, *shared_kelime_32, KEND);
				continue;
			}
			if (strcmp(argArr[i], "-c") == 0) {
				if (filePath != NULL && strcmp(filePath, "+") != 0) 
					printf("[%d] %s(karakter): %d\n", getpid(), filePath, karakter);
				else 
					printf("[%d] %s-stdin-%s(karakter): %d\n", getpid(), KGRN, KEND, karakter);
					
				if (*shared_parent_id == getpid())
					printf("%sToplam(karakter): %d\n%s", KYEL, *shared_karakter_32, KEND);
				continue;
			}
			
			fprintf(stderr, "%sHATA: Bilinmeyen seçenek '%s'\n%s", KRED, argArr[i], KEND);
			return 2;
		}
		printf("------------------------------\n");
	}
	
	fclose(fp);
	wait(NULL);
	
	return 0;
}