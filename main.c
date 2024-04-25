#include <stdio.h>
FILE *fNameList, *fMailList;

char nameList[] = "data/UserList/usr.txt";
char mailList[] = "data/UserList/mail.txt";

void signUp();
void signIn();

void matchString(char[], char[]);

void showError(char[]);

int main(){
    char input;
    printf("[L]og In\n[C]reate Account\n[E]xit\n");
    scanf("%c", &input);
    if(input == 'L'|| input == 'l'){
        signIn();
    }else if(input == 'e'||input == 'E'){
        //exit
    }else if(input == 'C'|| input == 'c'){
        signUp();
    }else{
        showError("Invalid Input");
        main();
    }
    return 0;
}
void showError(char error[]){
    printf("%s\n", error);
    system("pause");
}
void signIn(){

}
void signUp(){

}
