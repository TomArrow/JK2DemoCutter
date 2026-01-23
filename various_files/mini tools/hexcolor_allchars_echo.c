
#include <stdio.h>

int main(){
	printf("echo \"");
	for(int i=32;i<255;i++){
		if(i == '"' || i == '\n') continue;
		printf("^x%c%c%c%d ",(char)i,(char)i,(char)i,i);
		if(i%20 == 0){
			printf("\"\n");
			printf("echo \"");
		}
	}
	for(int i=32;i<255;i++){
		if(i == '"' || i == '\n') continue;
		printf("^X%c%c%c%c%c%c%d ",(char)i,(char)i,(char)i,(char)i,(char)i,(char)i,i);
		if(i%20 == 0){
			printf("\"\n");
			printf("echo \"");
		}
	}
	for(int i=32;i<255;i++){
		if(i == '"' || i == '\n') continue;
		printf("^X%c%c%c%c%c%c%d ",(char)i,'0',(char)i,'0',(char)i,'0',i);
		if(i%20 == 0){
			printf("\"\n");
			printf("echo \"");
		}
	}
	for(int i=32;i<255;i++){
		if(i == '"' || i == '\n') continue;
		printf("^X%c%c%c%c%c%c%d ",'0',(char)i,'0',(char)i,'0',(char)i,i);
		if(i%20 == 0){
			printf("\"\n");
			printf("echo \"");
		}
	}
	printf("\"\n");
}