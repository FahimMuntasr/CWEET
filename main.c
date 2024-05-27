#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_POSTS 1000
#define POSTS_PER_PAGE 5

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define PURPLE 5
#define CYAN 6
#define WHITE 7

// fData: Contains all the users along with their posts
FILE *fData;

//These arrays hold the location of the data file
char data[] = "./data/data.txt";

char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!_1234567890";//array of valid characters, will be used later for username validation
char lowerCase[] = "abcdefghijklmnopqrstuvwxyz";
char upperCase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char numbers[] = "1234567890";
char specialChars[] = "!@#$%%^&*()-_+={}[]|\\`~<>,.?/:;";


int postCount = 0;


struct userInfo{
    char name[32],
    email[32],
    password[32],
    confirmPassword[32],
    hashedPassword[32];
};

struct Likes{
    int likeCount;
    char likedBy[100][100];
};

struct Post{
    char postId[64];
    char content[256];
    char date[32];
    struct Likes likes;

};
struct Post posts[MAX_POSTS];

struct userInfo user;

// Functions related to the authentication section
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

// CRUD Post
void loadPosts();
void displayPosts(int);
void addPost();
void loadUserPosts(char []);
void profilePage(char[]);
void deletePost(char postId[]);
void editPost(char postId[]);


// UI functions
void setColor(int color, int background);
void resetColor();
void loadingAnim();


// Search user
void searchUser(char[]);
int getPostCount(char[]);
int getAllPostCount();

void explorePage();
void displayUserPosts(int page ,char[]);

void likePost(char postId[]);

int main(){
    system("cls");

    authenticate();

    return 0;
}



void menu() {
    system("cls");

    char input;
    setColor(CYAN, BLACK);
    printf("[P]rofile\n[E]xplore Page\n[S]earch\n[Q]uit\n");
    scanf(" %c", &input);
    resetColor();
    if (input == 'P' || input == 'p') {
        loadingAnim();
        profilePage( user.name);
    } else if (input == 'q' || input == 'Q') {
        exit(0); // Exit the program
    } else if (input == 'E' || input == 'e') {
        loadingAnim();
        explorePage();
    } else if(input == 'S' || input == 's') {
        char searchName[100];
        setColor(CYAN, BLACK);
        printf("Enter username to search: ");
        resetColor();
        scanf("%99s", searchName);
        searchUser(searchName);
    } else {
        showError("Invalid Input");
        menu();
    }
}


void authenticate() {
    system("cls");

    char input;
    setColor(CYAN, BLACK);
    printf("[L]og In\n[C]reate Account\n[E]xit\n");
    resetColor();
    scanf(" %c", &input);
    if(input == 'L'|| input == 'l'){
        loadingAnim();
        signIn();
    }else if(input == 'e'||input == 'E'){
        exit(0);
    }else if(input == 'C'|| input == 'c'){
        loadingAnim();
        signUp();
    }else{
        showError("Invalid Input");
        authenticate(); //recursively call the main function until the user inputs a valid character
    }
}

void showError(char error[]){
    setColor(RED, BLACK);
    printf("\n%s\n", error);
    system("pause"); //this command pauses the program until user presses any key
    resetColor();
}


void signUp() {
    system("cls");
    char extractedUsername[50], extractedEmail[50];
    int i;

    char buffer[256];  // Increase buffer size
    setColor(GREEN, BLACK);
    printf("Username: ");
    resetColor();
    scanf("%31s", user.name); // limit input size to 32 chars
    setColor(GREEN, BLACK);
    printf("Email: ");
    resetColor();
    scanf("%31s", user.email);

    // Check for invalid characters in username
    for (i = 0; user.name[i] != '\0'; i++) {
        if (!isElementOf(user.name[i], validChars)) {
            showError("Username contains invalid characters");
            signUp(); // Call signUp again
            return;
        }
    }

    // Check if username already exists
    fData = fopen(data, "r");
    if (fData == NULL) {
        showError("Internal error!");
        return;
    }

    // Create the search string for username
    char searchName[100];
    snprintf(searchName, sizeof(searchName), "User: %s", user.name);

    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strncmp(buffer, "User: ", 6) == 0) {
            sscanf(buffer, "User: %49s", extractedUsername);
            if (strcmp(extractedUsername, user.name) == 0) {
                showError("Username already exists\n");
                fclose(fData); // Close the file before returning
                signUp(); // Call signUp again
                return;
            }
        }
    }

    // Rewind the file pointer to read from the beginning of the file again
    rewind(fData);

    // Create the search string for email
    char searchEmail[100];
    snprintf(searchEmail, sizeof(searchEmail), "Email: %s", user.email);

    // Check if the email already exists
    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strncmp(buffer, "Email: ", 7) == 0) {
            sscanf(buffer, "Email: %49s", extractedEmail);
            if (strcmp(extractedEmail, user.email) == 0) {
                showError("Email already exists\n");
                fclose(fData); // Close the file before returning
                signUp(); // Call signUp again
                return;
            }
        }
    }

    fclose(fData);

    // Check if email contains '@' and '.'
    if (!isElementOf('@', user.email) || !isElementOf('.', user.email)) {
        showError("Invalid email (email must contain '@' and '.')");
        signUp(); // Call signUp again
        return;
    }

    // Take password
    takePass();
    hash_password(user.password, user.hashedPassword);

    fData = fopen(data, "a"); // Open the file in append mode
    if (fData == NULL) {
        showError("Could not open data file to save user details");
        return;
    }

    fprintf(fData, "\nUser: %s", user.name); // Add username
    fprintf(fData, "\nEmail: %s", user.email); // Add email
    fprintf(fData, "\nPassword: %s", user.hashedPassword); // Add hashed password
    fclose(fData);
    loadingAnim();
    authenticate(); // Call authenticate function
}

void takePass(){
    system("cls");
    int i;
    int upper = 0, lower = 0, number = 0, special = 0;// these 4 variables will be used as flags to validate the password later on
    setColor(GREEN,BLACK);
    printf("Enter password: ");//must be at least 8 chars long and less than 33 chars and must contain lowercase,uppercase,numbers and a special symbol
    resetColor();
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

    char extractedPassword[50];
    char buffer[256];
    int usernameFound = 0;
    setColor(PURPLE, BLACK);
    printf("Enter username: ");
    resetColor();
    scanf("%49s", user.name);

    fData = fopen(data, "r");
    if (fData == NULL) {
        showError("Could not open data file");
        return;
    }

    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strncmp(buffer, "User: ", 6) == 0) {
            char extractedUsername[50];
            sscanf(buffer, "User: %49s", extractedUsername);
            if (strcmp(extractedUsername, user.name) == 0) {
                usernameFound = 1;

                // Username found, prompt for password
                setColor(PURPLE, BLACK);
                printf("Enter password: ");
                resetColor();
                takeHiddenInput(user.password);
                hash_password(user.password, user.hashedPassword);

                // Look for the corresponding password line
                while (fgets(buffer, sizeof(buffer), fData) != NULL) {
                    if (strncmp(buffer, "Password: ", 10) == 0) {
                        sscanf(buffer, "Password: %49s", extractedPassword);
                        if (strcmp(extractedPassword, user.hashedPassword) == 0) {
                            fclose(fData);
                            loadingAnim();
                            menu();
                            return; // Exit the function after successful login
                        } else {
                            showError("Incorrect Password!");
                            fclose(fData);
                            signIn();
                            return;
                        }
                    }
                    // Stop searching if another user is encountered
                    if (strncmp(buffer, "User: ", 6) == 0) break;
                }
            }
        }
    }

    fclose(fData);

    if (!usernameFound) {
        showError("Username not found");
        signIn();
    }
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

int getPostCount(char username[]) {

    char buffer[256];
    char searchString[100];
    snprintf(searchString, sizeof(searchString), "User: %s\n", username);

    fData = fopen(data, "r");
    if (fData == NULL) {
        showError("Could not open data file");
        return 0;
    }

    int postCount = 0;
    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strcmp(buffer, searchString) == 0) {
            // Skip the email and password lines
            fgets(buffer, sizeof(buffer), fData);
            fgets(buffer, sizeof(buffer), fData);

            // Read posts for the user
            while (fgets(buffer, sizeof(buffer), fData) != NULL) {
                if (strstr(buffer, "PostID: ") == buffer) {
                    postCount++;
                }
                // Stop reading if another user is encountered
                if (strstr(buffer, "User: ") == buffer) break;
            }
        }
    }

    fclose(fData);
    return postCount;
}

int getAllPostCount()
{
    char buffer[256];
    fData = fopen(data, "r");

    if (fData == NULL) {
        showError("Could not open data file");
        return 0;
    }

    int postCount = 0;
    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strstr(buffer, "PostID: ") == buffer) {
            postCount++;
        }
    }

    fclose(fData);
    return postCount;
}

void explorePage()
{
    
    loadPosts();

    displayPosts(1);
}

void loadPosts(){
    char buffer[256];

    fData = fopen(data, "r");

    if (fData == NULL) {
        showError("Could not open data file");
        return;
    }
    int numOfPost = 0;

    while(fgets(buffer, sizeof(buffer), fData) != NULL){
        if(strstr(buffer, "PostID: ") == buffer){
            sscanf(buffer, "PostID: %63[^\n]", posts[numOfPost].postId);
            fgets(buffer, sizeof(buffer), fData); // Read Content line
            sscanf(buffer, "Content: %255[^\n]", posts[numOfPost].content);
            fgets(buffer, sizeof(buffer), fData); // Read Date line
            sscanf(buffer, "Date: %31[^\n]", posts[numOfPost].date);

            // Read the likes count
            fgets(buffer, sizeof(buffer), fData);
            sscanf(buffer, "Likes: %d\n", &posts[numOfPost].likes.likeCount);

            // Initialize the likedBy array
            for (int i = 0; i < posts[numOfPost].likes.likeCount; i++) {
                fgets(buffer, sizeof(buffer), fData);
                sscanf(buffer, "Liked: %s\n", posts[numOfPost].likes.likedBy[i]);
            }

            numOfPost++;
        }
    }
    fclose(fData);
}

// Displays the post in explore page with pagination
void displayPosts(int page) {
    system("cls");
    int numOfPost = getAllPostCount();
    int start = (page - 1) * POSTS_PER_PAGE;
    int end = start + POSTS_PER_PAGE;
    if (start >= numOfPost) {
        showError("No more posts to display.");
        explorePage();
        return;
    }
    if (end > numOfPost) end = numOfPost;

    for (int i = start; i < end; i++) {
        setColor(CYAN, BLACK);
        printf("PostID: %s\n", posts[i].postId);
        resetColor();
        setColor(BLACK, CYAN);
        printf("Content: %s\n", posts[i].content);
        resetColor();
        setColor(CYAN, BLACK);
        printf("Date: %s\n\n", posts[i].date);
        printf("Likes: %d\n", posts[i].likes.likeCount);

        // Print the likedBy list
        for (int j = 0; j < posts[i].likes.likeCount; j++) {
            printf("Liked: %s\n", posts[i].likes.likedBy[j]);
        }

        printf("[L]ike\n");
        printf("=====================================\n\n");
        resetColor();
    }
    setColor(CYAN, BLACK);
    printf("< [P]revious | [N]ext >\n");
    printf("[Q]uit\n");
    resetColor();

    char input;
    scanf(" %c", &input);
    if (input == 'P' || input == 'p') {
        if (page > 1) {
            displayPosts(page - 1);
        } else {
            showError("No previous page.");
            displayPosts(page);
        }
    } else if (input == 'N' || input == 'n') {
        if (end < numOfPost) {
            displayPosts(page + 1);
        } else {
            showError("No next page.");
            displayPosts(page);
        }
    } else if (input == 'Q' || input == 'q') {
        menu();
    } else if(input == 'L' || input == 'l'){
        setColor(CYAN, BLACK);
        printf("Enter PostID to like: ");
        resetColor();
        char postId[64];
        scanf("%63s", postId);
        likePost(postId);
        explorePage();
    } else {
        showError("Invalid Input");
        displayPosts(page);
    }
}




void displayUserPosts(int page, char username[32]) {
    system("cls");
    if(strcmp(username, user.name) == 0) {
        if (postCount == 0) {
            setColor(CYAN, BLACK);
            printf("No posts available.\n");
            printf("[A]dd Post\n");
            printf("[Q]uit to Menu\n");
            resetColor();
            char input;
            scanf(" %c", &input);
            if (input == 'A' || input == 'a') {
                addPost();
                displayUserPosts(1, username); // Go back to the first page to see the new post
            } else if (input == 'Q' || input == 'q') {
                menu();
            } else {
                showError("Invalid Input");
                displayUserPosts(page, username);
            }
            return;
        }

        int start = (page - 1) * POSTS_PER_PAGE;
        int end = start + POSTS_PER_PAGE;
        if (start >= postCount) {
            showError("No more posts to display.");
            return;
        }
        if (end > postCount) end = postCount;

        for (int i = start; i < end; i++) {
            int index = postCount - 1 - i;
            if (index < 0) break;
            setColor(CYAN, BLACK);
            printf("PostID: %s\n", posts[index].postId);
            resetColor();
            setColor(BLACK, CYAN);
            printf("Content: %s\n", posts[index].content);
            resetColor();
            setColor(CYAN, BLACK);
            printf("Date: %s\n", posts[index].date);
            printf("Likes: %d\n", posts[index].likes.likeCount);

            printf("====================================\n\n");
            printf("\t[L]ike [D]elete   [E]dit\n\n");
            printf("====================================\n\n");
            resetColor();
        }
        setColor(CYAN, BLACK);
        printf("< [P]revious | [N]ext >\n");
        printf("[A]dd Post\n");
        printf("[Q]uit to Menu\n");
        resetColor();
        char input;
        char postId[64];
        scanf(" %c", &input);
        if (input == 'P' || input == 'p') {
            if (page > 1) {
                displayUserPosts(page - 1, username);
            } else {
                showError("No previous page.");
                displayUserPosts(page, username);
            }
        } else if (input == 'N' || input == 'n') {
            if (end < postCount) {
                displayUserPosts(page + 1, username);
            } else {
                showError("No next page.");
                displayUserPosts(page, username);
            }
        } else if (input == 'A' || input == 'a') {
            addPost();
            displayUserPosts(1, username); // Go back to the first page to see the new post
        } else if (input == 'Q' || input == 'q') {
            menu();
        } else if (input == 'D' || input == 'd') {
            setColor(CYAN, BLACK);
            printf("Enter PostID to delete: ");
            resetColor();
            scanf("%63s", postId);
            deletePost(postId);
            displayUserPosts(page, username);
        } else if (input == 'E' || input == 'e') {
            setColor(CYAN, BLACK);
            printf("Enter PostID to edit: ");
            resetColor();
            scanf("%63s", postId);
            editPost(postId);
            displayUserPosts(page, username);
        } else if (input == 'L' || input == 'l') {
            setColor(CYAN, BLACK);
            printf("Enter PostID to like: ");
            resetColor();
            scanf("%63s", postId);
            likePost(postId);
            profilePage(username);
        } else {
            showError("Invalid Input");
            displayUserPosts(page, username);
        }
    } else {
        int num = getPostCount(username);

        if (num == 0) {
            setColor(CYAN, BLACK);
            printf("No posts available.\n");
            printf("[Q]uit to Menu\n");
            resetColor();
            char input;
            scanf(" %c", &input);
            if (input == 'Q' || input == 'q') {
                menu();
            } else {
                showError("Invalid Input");
                displayUserPosts(page, username);
            }
            return;
        }

        int start = (page - 1) * POSTS_PER_PAGE;
        int end = start + POSTS_PER_PAGE;
        if (start >= num) {
            showError("No more posts to display.");
            return;
        }
        if (end > num) end = num;

        for (int i = start; i < end; i++) {
            int index = num - 1 - i;
            if (index < 0) break;
            setColor(CYAN, BLACK);
            printf("PostID: %s\n", posts[index].postId);
            resetColor();
            setColor(BLACK, CYAN);
            printf("Content: %s\n", posts[index].content);
            resetColor();
            setColor(CYAN, BLACK);
            printf("Date: %s\n", posts[index].date);
            printf("Likes: %d\n", posts[index].likes.likeCount);

            printf("====================================\n\n");
            printf("\t[L]ike\n\n");
            printf("====================================\n\n");
            resetColor();
        }
        setColor(CYAN, BLACK);
        printf("< [P]revious | [N]ext >\n");
        printf("[Q]uit to Menu\n");
        resetColor();
        char input;
        char postId[64];
        scanf(" %c", &input);

        if (input == 'P' || input == 'p') {
            if (page > 1) {
                displayUserPosts(page - 1, username);
            } else {
                showError("No previous page.");
                displayUserPosts(page, username);
            }
        } else if (input == 'N' || input == 'n') {
            if (end < num) {
                displayUserPosts(page + 1, username);
            } else {
                showError("No next page.");
                displayUserPosts(page, username);
            }
        } else if (input == 'Q' || input == 'q') {
            menu();
        } else if (input == 'L' || input == 'l') {
            setColor(CYAN, BLACK);
            printf("Enter PostID to like: ");
            resetColor();
            char postId[64];
            scanf("%63s", postId);
            likePost(postId);
            profilePage(username);
        } else {
            showError("Invalid Input");
            displayUserPosts(page, username);
        }
    }
}

void loadUserPosts(char username[32]) {
    char buffer[256];
    char searchString[100];
    snprintf(searchString, sizeof(searchString), "User: %s", username);

    fData = fopen(data, "r"); // Assuming the file name is "data.txt"

    if (fData == NULL) {
        showError("Could not open data file");
        return;
    }

    if(strcmp(username, user.name) == 0){
        postCount = 0; // Reset post count

    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strstr(buffer, searchString) == buffer) {
            // Skip the email and password lines
            fgets(buffer, sizeof(buffer), fData);
            fgets(buffer, sizeof(buffer), fData);

            // Read posts for the user
            while (fgets(buffer, sizeof(buffer), fData) != NULL) {
                if (strstr(buffer, "PostID: ") == buffer) {
                    sscanf(buffer, "PostID: %63[^\n]", posts[postCount].postId);
                    fgets(buffer, sizeof(buffer), fData); // Read Content line
                    sscanf(buffer, "Content: %255[^\n]", posts[postCount].content);
                    fgets(buffer, sizeof(buffer), fData); // Read Date line
                    sscanf(buffer, "Date: %31[^\n]", posts[postCount].date);

                    // Read the likes count
                    fgets(buffer, sizeof(buffer), fData);
                    sscanf(buffer, "Likes: %d\n", &posts[postCount].likes.likeCount);

                    // Read likedBy list

                    for (int i = 0; i < posts[postCount].likes.likeCount; i++) {
                        fgets(buffer, sizeof(buffer), fData);
                        sscanf(buffer, "Liked: %s\n", posts[postCount].likes.likedBy[i]);
                    }




                    postCount++;
                }
                // Stop reading if another user is encountered
                if (strstr(buffer, "User: ") == buffer) break;
            }
        }
    }

    } else {
        int num = 0; // Reset post count

        while (fgets(buffer, sizeof(buffer), fData) != NULL) {
            if (strstr(buffer, searchString) == buffer) {
                // Skip the email and password lines
                fgets(buffer, sizeof(buffer), fData);
                fgets(buffer, sizeof(buffer), fData);

                // Read posts for the user
                while (fgets(buffer, sizeof(buffer), fData) != NULL) {
                    if (strstr(buffer, "PostID: ") == buffer) {
                        sscanf(buffer, "PostID: %63[^\n]", posts[num].postId);
                        fgets(buffer, sizeof(buffer), fData); // Read Content line
                        sscanf(buffer, "Content: %255[^\n]", posts[num].content);
                        fgets(buffer, sizeof(buffer), fData); // Read Date line
                        sscanf(buffer, "Date: %31[^\n]", posts[num].date);

                        // Read the likes count
                        fgets(buffer, sizeof(buffer), fData);
                        sscanf(buffer, "Likes: %d\n", &posts[num].likes.likeCount);

                        // read likedBy list
                        for (int i = 0; i < posts[num].likes.likeCount; i++) {
                            fgets(buffer, sizeof(buffer), fData);
                            sscanf(buffer, "Liked: %s\n", posts[num].likes.likedBy[i]);
                        }

                        num++;
                    }
                    // Stop reading if another user is encountered
                    if (strstr(buffer, "User: ") == buffer) break;
                }
            }
        }


    }
    fclose(fData);
}

void profilePage(char username[32]) {
    system("cls");
    setColor(CYAN, BLACK);

    resetColor();

    loadUserPosts(username);

    displayUserPosts(1, username);
}

void searchUser(char searchName[32]) {
    char buffer[256];
    char searchString[100];
    snprintf(searchString, sizeof(searchString), "%s", searchName);

    fData = fopen(data, "r"); // Assuming the file name is "data.txt"
    if (fData == NULL) {
        showError("Could not open data file");
        return;
    }

    int matchCount = 0;
    char matchedUsers[100][100]; // Assuming a maximum of 100 users for simplicity

    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strstr(buffer, "User: ") == buffer) {
            char username[100];
            sscanf(buffer, "User: %s", username);
            if (strstr(username, searchString) != NULL) {
                setColor(CYAN, BLACK);
                printf("User found: %s\n", username);
                resetColor();
                strcpy(matchedUsers[matchCount], username);
                matchCount++;
            }
        }
    }

    if (matchCount == 0) {
        showError("No matching users found");
        fclose(fData);
        menu();
        return;
    }

    setColor(CYAN, BLACK);
    printf("Enter the exact username to view profile: ");
    resetColor();
    char exactName[32];
    fflush(stdin);
    scanf("%31s", exactName);
    exactName[strcspn(exactName, "\n")] = 0; // remove newline character


    for (int i = 0; i < matchCount; i++) {
        // For case-insensitive comparison, use strcasecmp or _stricmp (Windows-specific)
        if (strcmp(exactName, matchedUsers[i]) == 0) {
            fclose(fData);
            profilePage(exactName);
            return;
        }
    }

    showError("Exact username not found");
    fclose(fData);
}


void likePost(char postId[]) {
    FILE *tempFile;
    char buffer[256];
    tempFile = fopen("temp.txt", "w");

    if (tempFile == NULL) {
        showError("Could not open temp file for writing");
        return;
    }

    fData = fopen(data, "r");
    if (fData == NULL) {
        showError("Could not open data file");
        fclose(tempFile);
        return;
    }

    int foundPost = 0;
    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        fputs(buffer, tempFile);

        if (strncmp(buffer, "PostID: ", 8) == 0) {
            char extractedPostId[64];
            sscanf(buffer, "PostID: %63[^\n]", extractedPostId);
            if (strcmp(extractedPostId, postId) == 0) {
                foundPost = 1;

                // Skip the content and date lines
                for (int i = 0; i < 2; i++) {
                    fgets(buffer, sizeof(buffer), fData);
                    fputs(buffer, tempFile);
                }

                // Read the likes count
                int likes;
                fgets(buffer, sizeof(buffer), fData);
                sscanf(buffer, "Likes: %d\n", &likes);

                // Read the likedBy list
                char likedBy[100][100];
                for (int i = 0; i < likes; i++) {
                    fgets(buffer, sizeof(buffer), fData);
                    sscanf(buffer, "Liked: %s\n", likedBy[i]);
                }

                // Check if the user has already liked the post
                int liked = 0;
                for (int i = 0; i < likes; i++) {
                    if (strcmp(likedBy[i], user.name) == 0) {
                        liked = 1;
                        break;
                    }
                }

                if (liked) {
                    fprintf(tempFile, "Likes: %d\n", likes - 1);

                    // Write back all users who liked the post except the current user
                    for (int i = 0; i < likes; i++) {
                        if (strcmp(likedBy[i], user.name) != 0) {
                            fprintf(tempFile, "Liked: %s\n", likedBy[i]);
                        }
                    }
                } else {
                    fprintf(tempFile, "Likes: %d\n", likes + 1);

                    // Write back all users who liked the post
                    for (int i = 0; i < likes; i++) {
                        fprintf(tempFile, "Liked: %s\n", likedBy[i]);
                    }

                    // Add the current user to the likedBy list
                    fprintf(tempFile, "Liked: %s\n", user.name);

                    setColor(GREEN, BLACK);
                    printf("Post liked successfully.\n");
                    resetColor();
                }  

                // Continue reading and writing the rest of the file
                while (fgets(buffer, sizeof(buffer), fData) != NULL) {
                    fputs(buffer, tempFile);
                }

                fclose(fData);
                fclose(tempFile);
                remove(data);
                rename("temp.txt", data);
                return;
            }
        }
    }

    fclose(fData);
    fclose(tempFile);

    remove("temp.txt");

    if (!foundPost) {
        showError("Post not found.");
    }
}


void deletePost(char postId[]) {
    FILE *tempFile;
    char buffer[256];
    char searchString[100];
    snprintf(searchString, sizeof(searchString), "PostID: %s", postId);

    fData = fopen(data, "r");
    tempFile = fopen("temp.txt", "w");

    if (fData == NULL || tempFile == NULL) {
        showError("Could not open data file");
        return;
    }

    int skip = 0;
    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strstr(buffer, searchString) == buffer) {
            skip = 1;
        }
        if (!skip) {
            fputs(buffer, tempFile);
        } else {
            // Skip next two lines (Content and Date)
            fgets(buffer, sizeof(buffer), fData);
            fgets(buffer, sizeof(buffer), fData);

            // Skip the likes and liked lines
            fgets(buffer, sizeof(buffer), fData);
            fgets(buffer, sizeof(buffer), fData);

            skip = 0;
        }
    }

    fclose(fData);
    fclose(tempFile);
    remove(data);
    rename("temp.txt", data);
    setColor(GREEN,BLACK);
    printf("Post deleted successfully.");
    resetColor();
    profilePage( user.name);
    return;
}


void editPost(char postId[]) {
    FILE *tempFile;
    char buffer[256];
    char searchString[100];
    snprintf(searchString, sizeof(searchString), "PostID: %s", postId);

    fData = fopen(data, "r");
    tempFile = fopen("temp.txt", "w");

    if (fData == NULL || tempFile == NULL) {
        showError("Could not open data file");
        return;
    }

    int editing = 0;
    char newContent[256];

    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        if (strstr(buffer, searchString) == buffer) {
            fputs(buffer, tempFile);
            fgets(buffer, sizeof(buffer), fData);
            setColor(CYAN,BLACK);
            printf("Enter new content: ");
            resetColor();
            getchar(); // consume newline left by previous input
            fgets(newContent, sizeof(newContent), stdin);
            newContent[strcspn(newContent, "\n")] = 0; // remove newline character
            fprintf(tempFile, "Content: %s\n", newContent);


            // Update the date to the current time
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            fprintf(tempFile, "Date: %04d-%02d-%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

            // Skip the likes and liked lines
            fgets(buffer, sizeof(buffer), fData);
            fgets(buffer, sizeof(buffer), fData);


            fputs(buffer, tempFile);
            editing = 1;
        } else {
            fputs(buffer, tempFile);
        }
    }

    fclose(fData);
    fclose(tempFile);
    remove(data);
    rename("temp.txt", data);

    if (editing) {
        profilePage(user.name);
        return;
        showError("Post edited successfully.");
    } else {
        showError("PostID not found.");
    }
}


void addPost() {
    char newContent[256];
    char newPostId[64];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char date[32];

    snprintf(date, sizeof(date), "%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    setColor(CYAN,BLACK);
    printf("Enter post content: ");
    resetColor();
    getchar(); // consume newline left by previous input
    fgets(newContent, sizeof(newContent), stdin);
    newContent[strcspn(newContent, "\n")] = 0; // remove newline character

    // Read the entire file content into memory
    fData = fopen(data, "r");
    if (fData == NULL) {
        showError("Could not open data file");
        return;
    }

    char fileContent[10000];
    fileContent[0] = '\0'; // Initialize the fileContent as an empty string
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fData) != NULL) {
        strcat(fileContent, buffer);
    }
    fclose(fData);

    // Find the position to insert the new post
    char* pos = strstr(fileContent, user.name);
    if (pos == NULL) {
        showError("User not found in data file");
        return;
    }

    // Find the latest PostID for the user
    int maxPostNumber = 0;
    char* postPos = pos;
    while ((postPos = strstr(postPos, "PostID: ")) != NULL) {
        if (strncmp(postPos + strlen("PostID: "), user.name, strlen(user.name)) == 0) {
            int postNumber;
            if (sscanf(postPos + strlen("PostID: ") + strlen(user.name) + 1, "%d", &postNumber) == 1) {
                if (postNumber > maxPostNumber) {
                    maxPostNumber = postNumber;
                }
            }
        }
        postPos++;
    }

    // Generate new PostID
    snprintf(newPostId, sizeof(newPostId), "%s_%d", user.name, maxPostNumber + 1);

    // Find the end of the user's posts
    char* endPos = strstr(pos, "User: ");
    if (endPos == NULL) {
        endPos = fileContent + strlen(fileContent); // Point to the end of the file
    } else {
        // Move back to the previous line
        while (endPos > fileContent && *(endPos - 1) != '\n') {
            endPos--;
        }
    }

    // Create the new post entry
    char newPostEntry[512];
    snprintf(newPostEntry, sizeof(newPostEntry), "\nPostID: %s\nContent: %s\nDate: %s\nLikes: 0\n", newPostId, newContent, date);

    // Insert the new post entry
    char updatedContent[20000];
    strncpy(updatedContent, fileContent, endPos - fileContent);
    updatedContent[endPos - fileContent] = '\0';
    strcat(updatedContent, newPostEntry);
    strcat(updatedContent, endPos);

    // Write the updated content back to the file
    fData = fopen(data, "w");
    if (fData == NULL) {
        showError("Could not open data file for writing");
        return;
    }
    fprintf(fData, "%s", updatedContent);
    fclose(fData);

    // Refresh post count and load posts again
    postCount++;
    loadUserPosts(user.name);
    setColor(GREEN,BLACK);
    printf("Post added successfully.\n");
    resetColor();
}
void setColor(int color, int background) {
    printf("\033[%d;%dm", background + 40, color + 30);
}

void resetColor() {
    printf("\033[0m");
}
void loadingAnim() {
    int duration;
    char loading_texts[][15] = {"\\Loading", "|Loading.", "/Loading..", "-Loading..."};
    int n = sizeof(loading_texts) / sizeof(loading_texts[0]);

    // Calculate the duration
    duration = 2;// this is in seconds, change it to anything you want
    time_t end_time = time(NULL) + duration;

    while (time(NULL) < end_time) {
        for (int i = 0; i < n; i++) {
			system("cls");
            setColor(CYAN,BLACK);
            printf("%s\n", loading_texts[i]);
            resetColor();
            _sleep(100);//pause for 500 milliseconds, 0.5 seconds
        }
    }
}
