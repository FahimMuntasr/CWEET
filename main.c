#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

// fData: Contains all the users along with their posts

FILE *fData;

//These arrays hold the location of the data file
char data[] = "./data/data.txt";

char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!_1234567890";//array of valid characters, will be used later for username validation
char lowerCase[] = "abcdefghijklmnopqrstuvwxyz";
char upperCase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char numbers[] = "1234567890";
char specialChars[] = "!@#$%%^&*()-_+={}[]|\\`~<>,.?/:;";

//! TODO: USE STRUCTS!
struct userInfo{
    char name[32], email[32], password[32], confirmPassword[32], hashedPassword[32];
};
struct userInfo user;
void authenticate();

void signUp();
void signIn();

void takePass();
void hash_password(const char str[], char hashed_pass[]);
void takeHiddenInput(char[]);// takes the input array as an argument

unsigned int hashingFunction(char[]); // hashes the password
int isElementOf(char, char[]);//Returns 1 if a char is an element of a string

void showError(char[]);//Takes the error reason as an argument and prints it on the screen
void menu();

int main(){
    system("cls");

    authenticate();

    return 0;
}



void menu(){
    system("cls");

    char input;
    printf("[P]rofile\n[E]xlore Page\n[Q]uit\n");
    scanf(" %c", &input);
    if(input == 'P'|| input == 'p'){
        return;
    }else if(input == 'q'||input == 'Q'){
        exit(0); // Exit the program
    }else if(input == 'E'|| input == 'E'){
        return;
    }else{

        showError("Invalid Input");
        menu();
    }


}


void authenticate(){
    system("cls");

    char input;
    printf("[L]og In\n[C]reate Account\n[E]xit\n");
    scanf("%c", &input);
    if(input == 'L'|| input == 'l'){
        signIn();
    }else if(input == 'e'||input == 'E'){
        return;//exit
    }else if(input == 'C'|| input == 'c'){
        signUp();
    }else{
        showError("Invalid Input");
        authenticate();//recursively call the main function until the user inputs a valid character
    }


}

void showError(char error[]){
    printf("\n%s\n", error);
    system("pause");//this command pauses the program until user presses any key
}
void signUp(){
    system("cls");
    char extractedUsername[50], extractedEmail[50];
    int i;

    char buffer[32], filepath[100];
    printf("Username: ");
    scanf("%31s", user.name);// limit input size to 32 chars
    printf("Email: ");
    scanf("%31s", user.email);

    //printf("%s\n%s\n", userName, userEmail);
    //Check for invalid characters
    for(i=0;user.name[i]!='\0';i++){
        if(!isElementOf(user.name[i], validChars)){
            //if the function isElementOf() returns 0 it means that the character is not a valid character

            showError("Username contains invalid characters");

            //recursively call back the function again
            signUp();
            return;
        }
    }

    //check if username already exists
    fData = fopen(data, "r");

    if(fData == NULL){

        showError("Internal error!");
        return;
    }

    while(fgets(buffer, 32, fData)!=NULL){

        if(strstr(buffer, "User: ") == buffer){
            sscanf(buffer, "User: %s", extractedUsername);

            if(strcmp(extractedUsername, user.name)== 0){

                showError("Username already exists\n");

                signUp();
                fclose(fData); // Close the file before returning
                return;


            }
        }

    }
    // Rewind the file pointer to read from the beginning of the file again
    rewind(fData);

    // Check if the mail already exist

    while(fgets(buffer, 32, fData)!=NULL){
        // Split the line into tokens using ':' as delimiter
        if(strstr(buffer, "Email: ") == buffer){
            sscanf(buffer, "Email: %s", extractedEmail);

            if(strcmp(extractedEmail, user.email) == 0){

                showError("Email already exists\n");

                signUp();
                fclose(fData);
                return;

            }
        }


    }

    fclose(fData);

     if(!isElementOf('@', user.email)||!isElementOf('.', user.email)){
        //if email does not contain '@' or '.' this code will execute
        showError("Invalid email (email must contain '@' and '.')");
        signUp();
        return;
    }

    // Take password

    takePass();
    hash_password(user.password, user.hashedPassword);


    fData = fopen(data, "a");//open the name list text file in the append mode to add a new username at the end

    fprintf(fData, "\nUser: %s", user.name);//add username to name list
    fprintf(fData, "\nEmail: %s", user.email);// add email to mail list
    fprintf(fData, "\nPassword: %s", user.hashedPassword);// add password
    fclose(fData);










}

void takePass(){
    system("cls");
    int i;
    int upper = 0, lower = 0, number = 0, special = 0;// these 4 variables will be used as flags to validate the password later on
    printf("Enter password: ");//must be at least 8 chars long and less than 33 chars and must contain lowercase,uppercase,numbers and a special symbol
    takeHiddenInput(user.password);
    //scanf("%31s", password);
    for(i=0;user.password[i]!='\0';i++){
        if(isElementOf(user.password[i],lowerCase))
            lower = 1;
        if(isElementOf(user.password[i],upperCase))
            upper = 1;
        if(isElementOf(user.password[i],numbers))
            number = 1;
        if(isElementOf(user.password[i],specialChars))
            special = 1;
    }
    if((i<8)||(lower == 0)||(upper == 0)||(number == 0)||(special == 0)){
        showError("Password must have at least 8 characters, lowercase, uppercase, numbers and special characters");
        takePass();
        return;
    }
    printf("\nEnter password again: ");
    takeHiddenInput(user.confirmPassword);
    //printf("\nFirst password : %s\nSecond password: %s\nSAME? %d\n", password, confirmPassword, matchString(password, confirmPassword));
    if(strcmp(user.password, user.confirmPassword)){
        showError("Passwords do not match");
        takePass();
        return;
    }
}

void signIn() {
    system("cls");

    char extractedUsername[50], extractedPassword[50];
    char buffer[256]; // Increase buffer size for reading lines


    printf("Enter username: ");
    scanf("%49s", user.name); // Adjust buffer size to prevent buffer overflow

    fData = fopen(data, "r");
    if (fData == NULL) {
        showError("Could not open data file");
        return;
    }

    // Search for username and password
    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strstr(buffer, "User: ") == buffer) {
            sscanf(buffer, "User: %49s", extractedUsername);
            if (strcmp(extractedUsername, user.name) == 0) {
                // Username found, prompt for password

                printf("Enter password: ");
                takeHiddenInput(user.password);
                hash_password(user.password, user.hashedPassword);

                // Search for password
                while (fgets(buffer, sizeof(buffer), fData) != NULL) {
                    if (strstr(buffer, "Password: ") == buffer) {
                        sscanf(buffer, "Password: %49s", extractedPassword);
                        if (strcmp(extractedPassword, user.hashedPassword) == 0) {
                            fclose(fData);
                            menu();
                        }else {
                            showError("Incorrect Password!");
                            fclose(fData);

                            signIn();
                            return;


                        }
                    }
                }
                break; // Exit the outer loop
            }else{
                showError("Username not found");
                signIn();
            }
        }
    }


    fclose(fData);
}

unsigned int hash_function(const char str[]) {
    unsigned int hash = 0;
    int i = 0;
    while (str[i] != '\0') {
        hash = (hash * 32) + str[i];
        i++;
    }
    return hash;
}

int isElementOf(char key, char arr[]){
    int i, flag=0;//Assume that the key is not and element of the array first
    for(i=0;arr[i]!='\0';i++){
        if(arr[i]== key){
            flag = 1;
            break;
        }
    }
    return flag;//Returns 1 if key is an element of the array and 0 if not
}

void takeHiddenInput(char arr[]){
    int i = 0;
    char ch;
    while(1){
        ch = getch();
        if(ch == '\r')// check if ENTER is pressed
            break;
        else if(ch == '\b' && i > 0){//check if BACKSPACE is pressed
            printf("\b \b");//Move cursor back, print space, move cursor back
            i--;
        }else if(i<31){
            printf("*");//Print asterisk
            arr[i] = ch;
            i++;
        }
    }
    arr[i] = '\0';


}

void hash_password(const char str[], char hashed_pass[]) {
    unsigned int hash = 0;
    int i = 0;
    while (str[i] != '\0') {
        hash = (hash * 32) + str[i];
        i++;
    }
    // Convert the hash value to a string
    sprintf(hashed_pass, "%u", hash);
}

