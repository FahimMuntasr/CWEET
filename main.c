#include <stdio.h>
#include <conio.h>

//fNameList will contain all the usernames and fMailList will contain all email adresses.
FILE *fNameList, *fMailList, *fUserInfo;
//Theses arrays hold the location of their respective files
char nameList[] = "data/UserList/usr.txt";
char mailList[] = "data/UserList/email.txt";

char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!_1234567890";//array of valid characters, will be used later for username validation
char lowerCase[] = "abcdefghijklmnopqrstuvwxyz";
char upperCase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char numbers[] = "1234567890";
char specialChars[] = "!@#$%^&*()-_+={}[]|\`~<>,.?/:;";

char userName[32], userEmail[32], password[32], tempPass[32];

void signUp();
void takePass();
void signIn();

void takeHiddenInput(char[]);// takes the input array as an argument

int isElementOf(char, char[]);//Returns 1 if a char is an element of a string
int matchString(char[], char[]);//Returns 1 if two strings are same

void showError(char[]);//Takes the error reason as an argument and prints it on the screen

int main(){
    system("cls");
    /*this commands clears the screen, this means that when the main function is recursively
    called later on inside the if statement the screen will refresh*/

    char input;
    printf("[L]og In\n[C]reate Account\n[E]xit\n");
    scanf("%c", &input);
    if(input == 'L'|| input == 'l'){
        signIn();
    }else if(input == 'e'||input == 'E'){
        return 0;//exit
    }else if(input == 'C'|| input == 'c'){
        signUp();
    }else{
        showError("Invalid Input");
        main();//recursively call the main function until the user inputs a valid character
    }
    return 0;
}
void showError(char error[]){
    printf("%s\n", error);
    system("pause");//this command pauses the program until user presses any key
}
void signUp(){
    system("cls");//clear screen
    int i;
    char buffer[32], filepath[100];
    printf("Username: ");
    scanf("%31s", userName);// limit input size to 64 chars
    printf("Email: ");
    scanf("%31s", userEmail);
    //printf("%s\n%s\n", userName, userEmail);
    //Check for invalid characters
    for(i=0;userName[i]!='\0';i++){
        if(!isElementOf(userName[i], validChars)){
            //if the function isElementOf() returns 0 it means that the character is not a valid character
            showError("Username contains invalid characters");
            signUp();//recursively call back the function again
            return;
        }
    }
    //check if username already exists
    fNameList = fopen(nameList, "r");
    // opens the text file containing all usernames in the read ("r") mode
    if(fNameList == NULL){
        //error handling where a file cannot be opened for some reason
        showError("Error in opening nameList");
        return;
    }
    while(fgets(buffer, 32, fNameList)!=NULL){
        /*The fgets() function has 3 arguments, an array, the size of the array, and a source file.
        For each loop the buffer array gets a line of text put inside it, until the end of the file is reached where it will be NULL.
        As the usernames will not be more than 100 characters the size of the buffer array is set to 100*/
        if(matchString(userName, buffer)){
            showError("Username already exists");
            fclose(fNameList);
            signUp();
            return;
        }

    }
    fclose(fNameList);
    fMailList = fopen(mailList, "r");
    if(fMailList == NULL){
        showError("Error in opening mailList");
        return;
    }
    while(fgets(buffer, 32, fMailList)!=NULL){
        if(matchString(userEmail, buffer)){
            showError("Email already exists");
            fclose(fMailList);
            signUp();
            return;
        }
    }
    fclose(fMailList);
    //Good practice to close files after using them
    if(!isElementOf('@', userEmail)||!isElementOf('.', userEmail)){
        //if email does not contain '@' or '.' this code will execute
        showError("Invalid email (email must contain '@' and '.')");
        signUp();
        return;
    }
    // Email and username are now validated, we can move on to validating the password
    takePass();
    // Now that the password is also validated we can store this information

    fNameList = fopen(nameList, "a");//open the name list text file in the append mode to add a new username at the end
    fMailList = fopen(mailList, "a");
    fprintf(fNameList, userName);//add username to name list
    fprintf(fMailList, userEmail);// add email to mail list
    fclose(fNameList);
    fclose(fMailList);
    //good practice to close files after using them to free up memory

    sprintf(filepath, "data/UserInfo/%s.txt", userName);// this function writes a filepath to a text file that will now be created, the name of the text file is same as the username
    fUserInfo = fopen(filepath, "a"); // Open file in append mode
    if (fUserInfo != NULL) { // Check if file opened successfully
        fprintf(fUserInfo, "%s\n%s\n%s\n", userName, userEmail, password); // Write info followed by newline
        fclose(fUserInfo); // Close the file
    } else {
        printf("Error opening file.\n"); // Notify if there's an error opening the file
    }
}
void takePass(){
    system("cls");//clear screen
    int i;
    int upper = 0, lower = 0, number = 0, special = 0;// these 4 variables will be used as flags to validate the password later on
    printf("Enter password: ");//must be more than 12 chars long and less than 33 chars and must contain lowercase,uppercase,numbers and a special symbol
    takeHiddenInput(password);
    //scanf("%31s", password);
    for(i=0;password[i]!='\0';i++){
        if(isElementOf(password[i],lowerCase)){
            lower = 1;
        }
        if(isElementOf(password[i],upperCase)){
            upper = 1;
        }
        if(isElementOf(password[i],numbers)){
            number = 1;
        }
        if(isElementOf(password[i],specialChars)){
            special = 1;
        }
    }
    if(i<12){
        showError("Password must be more than 12 characters");
        takePass();
        return;
    }else if(lower == 0){
        showError("Password must contain atleast one lower case character");
        takePass();
        return;
    }else if(upper == 0){
        showError("Password must conatin atleast one upper case character");
        takePass();
        return;
    }else if(number == 0){
        showError("Password must contain atleast one number");
        takePass();
        return;
    }else if(special == 0){
        showError("Password must contain atleast one special character");
        takePass();
        return;
    }
    printf("\nEnter password again: ");
    takeHiddenInput(tempPass);
    printf("\nFirst password : %s\nSecond password: %s\nSAME? %d\n", password, tempPass, matchString(password, tempPass));
    if(!matchString(password, tempPass)){
        showError("Passwords do not match");
        takePass();
        return;
    }
}
void signIn(){

}
int matchString(char ref[], char input[]){
    int i, flag=1;//Assume strings are equal at first
    for(i=0;ref[i]!='\0';i++){
        if(ref[i] != input[i]){
            flag = 0;
            break;
        }
    }
    return flag;//Returns 1 if equal and 0 if not equal
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
    password[i] = '\0';
    //printf("%s", password);
}
