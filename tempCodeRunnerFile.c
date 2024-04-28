#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define N 9
#define HASH_SIZE 1000
#define MAX_PUZZLES 1000
#define MAX_WORDS 100
#define MAX_WORD_LENGTH 50
#define MAX_TRIES 7
#define GRID_SIZE 10
#define MAX_GUESSES 5


typedef struct Node
{
    int row;
    int col;
    struct Node *next;
} Node;

typedef struct
{
    int puzzle[N][N];
    int solution[N][N];
} SudokuEntry;

typedef struct
{
    int puzzle[N][N];
    int priority;
} SudokuPuzzle;

SudokuEntry hashTable[HASH_SIZE];
SudokuPuzzle priorityQueue[MAX_PUZZLES];
int queueSize = 0;

// Function prototypes
void printGrid(int grid[N][N]);
int isSafe(int grid[N][N], int row, int col, int num);
int solveSudoku(int grid[N][N]);
Node *findUnassignedLocation(int grid[N][N]);
void generateSudoku(int grid[N][N], int difficulty);
void saveUser(const char *userId, const char *password, const char *firstName, const char *lastName);
int authenticateUser(const char *userId, const char *password);
void printSudokuGameInfo();
void submenu(const char *firstName, const char *lastName);
int hash(int puzzle[N][N]);
void storeSudokuSolution(int puzzle[N][N], int solution[N][N]);
int retrieveSudokuSolution(int puzzle[N][N], int solution[N][N]);
void enqueuePuzzle(int puzzle[N][N], int priority);
void dequeuePuzzle(int puzzle[N][N]);
int isRowValid(int grid[N][N], int row);
int isColumnValid(int grid[N][N], int col);
void hangmanGame();
void printHangmanInfo();
void readHangmanWords(char words[MAX_WORDS][MAX_WORD_LENGTH], char hints[MAX_WORDS][MAX_WORD_LENGTH], int *wordCount,int difficulty);
void drawHangman(int attempts);
void displayWord(char *word, bool *guessedLetters);
void playWordSearch();
void selectRandomWords(char words[MAX_WORDS][MAX_WORD_LENGTH], int *wordCount, char selectedWords[5][MAX_WORD_LENGTH]);
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]);
void fillRandomLetters(char grid[GRID_SIZE][GRID_SIZE]);
void printWordsearchInfo();
void generateWordSearch();
void fillWord(char grid[GRID_SIZE][GRID_SIZE], const char *word, int startRow, int startCol, int vertical);
void showGrid(char grid[GRID_SIZE][GRID_SIZE]);


int main()
{
    printf("\n\n\n**************************************************** INTELLIGENT GAMING SYSTEM  ***************************************************************\n");
    printf("________________________________________________________________________________________________________________________________________________\n\n");
    printf("If you are a new user, then choose Sign up, else login with your credentials..............\n\n");

    int choice;
    char firstName[50], lastName[50];

    while (1)
    {
        printf("\t1. Login\n");
        printf("\t2. Sign Up\n");
        printf("\tEnter your choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            char userId[50], password[50];
            printf("\tEnter User ID: ");
            scanf("%s", userId);
            printf("\tEnter Password: ");
            scanf("%s", password);
            if (authenticateUser(userId, password))
            {
                printf("\n\n");
                printf("\tLOGIN SUCCESSFUL!\n");
                FILE *file = fopen("users.txt", "r");
                char buffer[100];
                while (fgets(buffer, sizeof(buffer), file) != NULL)
                {
                    char storedUserId[50], storedPassword[50];
                    sscanf(buffer, "%s %s %s %s", storedUserId, storedPassword, firstName, lastName);
                    if (strcmp(storedUserId, userId) == 0)
                    {
                        break;
                    }
                }
                fclose(file);
                printf("\n\n************************************************************************************************************************************************\n");
                printf("\n\tWELCOME %s %s to INTELLIGENT GAMING SYSTEM\n\n\n", firstName, lastName);
                submenu(firstName, lastName); // Display submenu
            }
            else
            {
                printf("\n\n\n");
                printf("\tIncorrect User ID or Password. Please try again.\n\n");
            }
        }
        else if (choice == 2)
        {
            char userId[50], password[50], confirmPassword[50];
            printf("\tEnter First Name: ");
            scanf("%s", firstName);
            printf("\tEnter Last Name: ");
            scanf("%s", lastName);
            printf("\tEnter User ID: ");
            scanf("%s", userId);
            printf("\tEnter Password: ");
            scanf("%s", password);
            printf("\tConfirm Password: ");
            scanf("%s", confirmPassword);
            if (strcmp(password, confirmPassword) == 0)
            {
                saveUser(userId, password, firstName, lastName);
                printf("\n\n");
                printf("\tACCOUNT SUCCESSFULLY CREATED..!\n");
                printf("\tNOW LOGIN WITH YOUR CREDENTIALS...\n\n");
            }
            else
            {
                printf("\n\n");
                printf("\tPASSWORDS DO NOT MATCH. PLEASE TRY AGAIN....!\n\n");
            }
        }
        else
        {
            printf("\n\n");
            printf("\tInvalid choice...\n");
        }
    }
    return 0;
}

void generateWordSearch() {
    char selectedWords[5][MAX_WORD_LENGTH];
    char usedWords[MAX_GUESSES][MAX_WORD_LENGTH];
    int wordCount = 0;

    // Accept user input for five words
    printf("\nEnter five words to be included in the Word Search puzzle:\n");
    for (int i = 0; i < 5; i++) {
        printf("Word %d: ", i + 1);
        scanf("%s", selectedWords[i]);
    }

    // Initialize grid
    char grid[GRID_SIZE][GRID_SIZE];
    initializeGrid(grid);

    // Fill grid with user-entered words
    fillWord(grid, selectedWords[0], 1, 1, 0); // First word horizontally in 1st row
    fillWord(grid, selectedWords[1], 3, 2, 0); // Second word horizontally in 3rd row
    fillWord(grid, selectedWords[2], 5, 2, 0); // Third word horizontally in 5th row
    fillWord(grid, selectedWords[3], 7, 1, 0); // Fourth word horizontally in 7th row
    fillWord(grid, selectedWords[4], 9, 2, 0); // Fifth word horizontally in 9th row

    // Fill remaining spaces with random letters
    fillRandomLetters(grid);

    // Print grid
    printf("\nWord Search Grid:\n");
    showGrid(grid);
}

// Function to print the Sudoku grid
void printGrid(int grid[N][N])
{
    printf(" -----------------------\n");
    for (int row = 0; row < N; row++)
    {
        printf("| ");
        for (int col = 0; col < N; col++)
        {
            if (grid[row][col] == 0)
                printf("  ");
            else
                printf("%d ", grid[row][col]);
            if ((col + 1) % 3 == 0)
                printf("| ");
        }
        printf("\n");
        if ((row + 1) % 3 == 0)
            printf(" -----------------------\n");
    }
}

// Function to check if it's safe to place a number in a cell
int isSafe(int grid[N][N], int row, int col, int num)
{
    for (int x = 0; x < N; x++)
        if (grid[row][x] == num || grid[x][col] == num || grid[row - row % 3 + x / 3][col - col % 3 + x % 3] == num)
            return 0;
    return 1;
}

// Function to solve the Sudoku puzzle
int solveSudoku(int grid[N][N])
{
    Node *current = findUnassignedLocation(grid);
    if (current == NULL)
        return 1;
    int row = current->row;
    int col = current->col;
    free(current);
    for (int num = 1; num <= 9; num++)
    {
        if (isSafe(grid, row, col, num))
        {
            grid[row][col] = num;
            if (solveSudoku(grid))
                return 1;
            grid[row][col] = 0;
        }
    }
    return 0;
}

// Function to find the next unassigned location in the Sudoku grid
Node *findUnassignedLocation(int grid[N][N])
{
    Node *head = NULL;
    for (int row = 0; row < N; row++)
        for (int col = 0; col < N; col++)
            if (grid[row][col] == 0)
            {
                Node *newNode = (Node *)malloc(sizeof(Node));
                newNode->row = row;
                newNode->col = col;
                newNode->next = head;
                head = newNode;
            }
    return head;
}

// Function to generate a Sudoku problem
void generateSudoku(int grid[N][N], int difficulty)
{
    srand(time(NULL));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0; // Clear the grid

    solveSudoku(grid); // Generate a solved Sudoku grid

    // Adjust the number of cells to remove based on difficulty
    int remaining;
    if (difficulty == 1)
        remaining = N * N * 3 / 5; // Easy: 60% of cells removed
    else if (difficulty == 2)
        remaining = N * N * 4 / 5; // Medium: 80% of cells removed
    else
        remaining = N * N * 9 / 10; // Hard: 90% of cells removed

    // Randomly remove numbers to create the puzzle
    while (remaining > 0)
    {
        int row = rand() % N;
        int col = rand() % N;
        if (grid[row][col] != 0)
        {
            grid[row][col] = 0;
            remaining--;
        }
    }
}

// Function to save user details to a file
void saveUser(const char *userId, const char *password, const char *firstName, const char *lastName)
{
    FILE *file = fopen("users.txt", "a");
    if (file != NULL)
    {
        fprintf(file, "%s %s %s %s\n", userId, password, firstName, lastName);
        fclose(file);
    }
}

// Function to authenticate user credentials
int authenticateUser(const char *userId, const char *password)
{
    FILE *file = fopen("users.txt", "r");
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char storedUserId[50], storedPassword[50];
        sscanf(buffer, "%s %s", storedUserId, storedPassword);
        if (strcmp(storedUserId, userId) == 0 && strcmp(storedPassword, password) == 0)
        {
            fclose(file);
            return 1; // Authentication successful
        }
    }
    fclose(file);
    return 0; // Authentication failed
}

// Function to print Sudoku game information from a file
void printSudokuGameInfo()
{
    FILE *file = fopen("input.txt", "r");
    if (file != NULL)
    {
        char buffer[1000];
        while (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            printf("%s", buffer);
        }
        fclose(file);
    }
    else
    {
        printf("Unable to open input.txt.\n");
    }
}

void selectRandomWords(char words[MAX_WORDS][MAX_WORD_LENGTH], int *wordCount, char selectedWords[5][MAX_WORD_LENGTH]) {
    // Open the words file
    FILE *file = fopen("words2.txt", "r");
    if (file == NULL) {
        printf("Error opening words file.\n");
        exit(1);
    }

    // Read words from the file
    *wordCount = 0;
    while ((*wordCount < MAX_WORDS) && (fscanf(file, "%s", words[*wordCount]) == 1)) {
        (*wordCount)++;
    }

    // Select five unique random words
    srand(time(NULL));
    int selectedCount = 0;
    int usedIndexes[MAX_WORDS] = {0}; // Array to track used indexes

    while (selectedCount < 5) {
        int randomIndex = rand() % (*wordCount);
        if (!usedIndexes[randomIndex]) {
            strcpy(selectedWords[selectedCount], words[randomIndex]);
            usedIndexes[randomIndex] = 1;
            selectedCount++;
        }
    }

    fclose(file);
}

void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '-';
        }
    }
}

void fillWord(char grid[GRID_SIZE][GRID_SIZE], const char *word, int startRow, int startCol, int vertical) {
    if (vertical) {
        for (int i = 0; i < strlen(word); i++) {
            grid[startRow + i][startCol] = word[i];
        }
    } else {
        for (int i = 0; i < strlen(word); i++) {
            grid[startRow][startCol + i] = word[i];
        }
    }
}

void fillRandomLetters(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == '-') {
                grid[i][j] = 'a' + rand() % 26; // Fill with random letters from 'a' to 'z'
            }
        }
    }
}

void showGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    printf("+---------------------+\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("| ");
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("|\n");
    }
    printf("+---------------------+\n");
}

void playWordSearch() {
    char selectedWords[5][MAX_WORD_LENGTH];
    char usedWords[MAX_GUESSES][MAX_WORD_LENGTH];
    char words[MAX_WORDS][MAX_WORD_LENGTH];
    int wordCount = 0;

    // Select five random unique words
    selectRandomWords(words, &wordCount, selectedWords);

    // Initialize grid
    char grid[GRID_SIZE][GRID_SIZE];
    initializeGrid(grid);

    // Fill grid with words
    fillWord(grid, selectedWords[0], 1, 1, 0); // First word horizontally in 1st row
    fillWord(grid, selectedWords[1], 3, 2, 0); // Second word horizontally in 3rd row
    fillWord(grid, selectedWords[2], 5, 2, 0); // Third word horizontally in 5th row
    fillWord(grid, selectedWords[3], 7, 1, 0); // Fourth word horizontally in 7th row
    fillWord(grid, selectedWords[4], 9, 2, 0); // Fifth word horizontally in 9th row

    // Fill remaining spaces with random letters
    fillRandomLetters(grid);

    // Print grid
    printf("\nWord Search Grid:\n");
    showGrid(grid);

    // Game logic
    int guesses = 0;
    int correctGuesses = 0;
    char guess[MAX_WORD_LENGTH];
    printf("\nThere are total five words in the Grid.\n Guess them as quick as possible and check your verbal abilities......\n\n");
    while (correctGuesses < 5 && guesses < MAX_GUESSES) {
        printf("\nEnter your guess: ");
        scanf("%s", guess);

        int alreadyGuessed = 0;
        for (int i = 0; i < correctGuesses; i++) {
            if (strcmp(guess, usedWords[i]) == 0) {
                alreadyGuessed = 1;
                break;
            }
        }

        if (alreadyGuessed) {
            printf("The word '%s' is already guessed.\n", guess);
        } else {
            int found = 0;
            for (int i = 0; i < 5; i++) {
                if (strcmp(guess, selectedWords[i]) == 0) {
                    printf("OOH....! You guessed the word '%s'.\n", guess);
                    strcpy(usedWords[correctGuesses], guess);
                    correctGuesses++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Sorry, '%s' is not the word.\n", guess);
                guesses++;
            }
        }

        printf("Total Guesses: %d/%d\n", correctGuesses, MAX_GUESSES);
    }

    if (correctGuesses == 5) {
        printf("\nCongratulations! You have won the game!\n");
    } else {
        printf("\nSorry! You have reached the maximum number of guesses. Game over!\n");
    }
}

void printWordsearchInfo()
{
    FILE *file = fopen("wordsearch.txt", "r");
    if (file == NULL)
    {
        printf("Error opening wordsearch info file.\n");
        exit(1);
    }

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("\t%s", line);
    }
    printf("\n\n\n");
    fclose(file);
}


// Submenu function
void submenu(const char *firstName, const char *lastName)
{
    int choice;
    while (1)
    {
        printf("\nMain Menu\n");
        printf("1. Sudoku\n");
        printf("2. Hangman\n");
        printf("3. WordSeach\n");
        printf("4. Logout\n");
        printf("5. Exit\n\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
        {
            // Sudoku submenu
            int sudokuChoice;
            while (1)
            {
                printf("\nSudoku\n");
                printf("1. Solve Sudoku\n");
                printf("2. Generate Sudoku Problem\n");
                printf("3. Sudoku Game Information\n");
                printf("4. Logout\n");
                printf("5. Exit\n");
                printf("6. Return to Main Menu\n\n");
                printf("Enter your choice: ");
                scanf("%d", &sudokuChoice);
                switch (sudokuChoice)
                {
                case 1:
                {

                    // Solve Sudoku
                    printf("\n\n\n\tPlease enter the Sudoku puzzle row-wise (use '0' for empty cells):\n");
                    int userGrid[N][N];
                    for (int i = 0; i < N; i++)
                    {
                        printf("\tRow %d: ", i + 1);
                        for (int j = 0; j < N; j++)
                        {
                            scanf("%d", &userGrid[i][j]);
                        }
                        if (!isRowValid(userGrid, i))
                        {
                            printf("\n\tInvalid Sudoku! Values in row %d are not unique.\n", i + 1);
                            break;
                        }
                    }
                    for (int j = 0; j < N; j++)
                    {
                        if (!isColumnValid(userGrid, j))
                        {
                            printf("\n\tInvalid Sudoku! Values in column %d are not unique.\n", j + 1);
                            break;
                        }
                    }
                    printf("\n\n\tUser Input Sudoku Problem:\n\n");
                    printGrid(userGrid);
                    printf("\n\tSolution:\n");
                    if (solveSudoku(userGrid) == 1)
                        printGrid(userGrid);
                    else
                        printf("\n\n\tNo solution exists\n\n");
                    break;
                }
                case 2:
                {
                    // Generate Sudoku problem
                    int difficulty;
                    printf("\n\tChoose Difficulty Level:\n");
                    printf("\t1. Easy\n");
                    printf("\t2. Medium\n");
                    printf("\t3. Hard\n");
                    printf("\tEnter your choice: ");
                    scanf("%d", &difficulty);
                    printf("\n\tGenerating Sudoku problem...\n\n");
                    int grid[N][N];
                    generateSudoku(grid, difficulty);
                    printf("\n\n\tGenerated Sudoku Problem:\n");
                    printGrid(grid);
                    printf("\n\tSolution:\n");
                    if (solveSudoku(grid) == 1)
                        printGrid(grid);
                    else
                        printf("\n\tNo solution exists\n");
                    break;
                }
                case 3:
                { // Print Sudoku game information
                    printf("\n\n");
                    printf("********************************************************** Sudoku Game Information **********************************************************\n");
                    printSudokuGameInfo();
                    break;
                }
                case 4:
                    // Logout
                    printf("\nLogging out...\n\n\n");
                    main();
                case 5:
                    // Exit
                    printf("\nExiting the system...\n\n\n");
                    exit(0);
                case 6:
                    // returning to main menu
                    printf("\nReturning to the main menu...\n\n\n");
                    submenu(firstName, lastName);
                    break;
                default:
                    printf("\nInvalid choice...\n");
                }
            }
            break;
        }
        case 2:
        {
            // Hangman submenu
            int hangmanChoice;
            while (1)
            {
                printf("\nHangman\n");
                printf("1. Play Hangman\n");
                printf("2. Hangman Game information\n");
                printf("3. Logout\n");
                printf("4. Exit\n");
                printf("5. Returning to main menu\n\n");
                printf("Enter your choice: ");
                scanf("%d", &hangmanChoice);
                switch (hangmanChoice)
                {
                case 1:
                {
                    // Play Hangman
                    printf("\n\n");
                    printf("********************************************************** HANGMAN GAME **********************************************************\n");
                    hangmanGame();
                    break;
                }
                case 2:
                    // Hangman Game Information
                    printf("\nHangman Game Information\n");
                    printHangmanInfo();
                    break;
                case 3:
                    // Logout
                    printf("\nLogging out...\n\n\n");
                    main();
                case 4:
                    // Exit
                    printf("\nExiting the system...\n\n\n");
                    exit(0);
                case 5:
                    // returning to main menu
                    printf("\nReturning to the main menu...\n\n\n");
                    submenu(firstName, lastName);
                    break;
                default:
                    printf("\nInvalid choice...\n");
                }
            }
            break;
        }
        case 3:
         {
            // WordSearch submenu
            int wChoice;
            while (1)
            {
                 printf("\n\n\n********************************************************** WORDSEARCH **************************************************************************\n");
                printf("1. Play  WORDSEARCH\n");
                printf("2. WORDSEARCH Game information\n");
                printf("3. Generate WORDSEARCH Problem\n");
                printf("4. Logout\n");
                printf("5. Exit\n");
                printf("6. Returning to the Main Menu\n\n");
                printf("Enter your choice: ");
                scanf("%d", &wChoice);
                switch (wChoice)
                {
                case 1:
                {
                    // Play wordsearch
                    printf("\n\n");
                    playWordSearch();
                    break;
                }
                case 2:
                {
                    // Hangman Game Information
                    printf("\nWORDSEARCH Game Information\n");
                    printWordsearchInfo();
                    break;
                }
                case 3:
                {
                    // Hangman Game Information
                    printf("\n Generating WORDSEARCH problem form inputted Keywords......\n");
                    generateWordSearch();
                    break;
                }
                case 4:
                {
                    // Logout
                    printf("\nLogging out...\n\n\n");
                    main();
                }
                case 5:
                {
                    // Exit
                    printf("\nExiting the system...\n\n\n");
                    exit(0);
                }
                case 6:
                    // returning to main menu
                    printf("\nReturning to the main menu...\n\n\n");
                    submenu(firstName, lastName);
                    break;
                default:
                    printf("\nInvalid choice...\n");
                }
            }
            break;
        }
                
        case 4:
        {
            // Logout
            printf("\nLogging out...\n\n\n");
            return; // Exit the submenu function
        }
        case 5:
        {
            // Exit
            printf("\nExiting the system...\n\n\n");
            exit(0);
        }
        default:
            printf("\nInvalid choice...\n");
        }
    }
}

// Function to generate hash value for Sudoku puzzle
int hash(int puzzle[N][N])
{
    int hashValue = 0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            hashValue += puzzle[i][j];
        }
    }
    return hashValue % HASH_SIZE;
}

// Function to store solved Sudoku puzzle in the hash table
void storeSudokuSolution(int puzzle[N][N], int solution[N][N])
{
    int index = hash(puzzle);
    memcpy(hashTable[index].puzzle, puzzle, sizeof(int) * N * N);
    memcpy(hashTable[index].solution, solution, sizeof(int) * N * N);
}

// Function to retrieve solution of a Sudoku puzzle from the hash table
int retrieveSudokuSolution(int puzzle[N][N], int solution[N][N])
{
    int index = hash(puzzle);
    if (memcmp(hashTable[index].puzzle, puzzle, sizeof(int) * N * N) == 0)
    {
        memcpy(solution, hashTable[index].solution, sizeof(int) * N * N);
        return 1; // Solution found in hash table
    }
    return 0; // Solution not found in hash table
}

// Function to enqueue a Sudoku puzzle with its priority
void enqueuePuzzle(int puzzle[N][N], int priority)
{
    if (queueSize < MAX_PUZZLES)
    {
        memcpy(priorityQueue[queueSize].puzzle, puzzle, sizeof(int) * N * N);
        priorityQueue[queueSize].priority = priority;
        queueSize++;
        // Heapify up to maintain priority queue property
        int current = queueSize - 1;
        while (current > 0 && priorityQueue[current].priority > priorityQueue[(current - 1) / 2].priority)
        {
            SudokuPuzzle temp = priorityQueue[current];
            priorityQueue[current] = priorityQueue[(current - 1) / 2];
            priorityQueue[(current - 1) / 2] = temp;
            current = (current - 1) / 2;
        }
    }
    else
    {
        printf("Priority queue is full.\n");
    }
}

// Function to dequeue the Sudoku puzzle with the highest priority
void dequeuePuzzle(int puzzle[N][N])
{
    if (queueSize > 0)
    {
        memcpy(puzzle, priorityQueue[0].puzzle, sizeof(int) * N * N);
        // Replace the root with the last element and heapify down
        priorityQueue[0] = priorityQueue[queueSize - 1];
        queueSize--;
        int current = 0;
        while (1)
        {
            int leftChild = 2 * current + 1;
            int rightChild = 2 * current + 2;
            int maxChild = current;
            if (leftChild < queueSize && priorityQueue[leftChild].priority > priorityQueue[maxChild].priority)
            {
                maxChild = leftChild;
            }
            if (rightChild < queueSize && priorityQueue[rightChild].priority > priorityQueue[maxChild].priority)
            {
                maxChild = rightChild;
            }
            if (maxChild != current)
            {
                SudokuPuzzle temp = priorityQueue[current];
                priorityQueue[current] = priorityQueue[maxChild];
                priorityQueue[maxChild] = temp;
                current = maxChild;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        printf("Priority queue is empty.\n");
    }
}

// Function to check if a row in the Sudoku grid is valid
int isRowValid(int grid[N][N], int row)
{
    int seen[N + 1] = {0}; // Initialize seen array to keep track of seen numbers
    for (int j = 0; j < N; j++)
    {
        if (grid[row][j] != 0)
        {
            if (seen[grid[row][j]] == 1) // If number is already seen, row is invalid
                return 0;
            seen[grid[row][j]] = 1; // Mark the number as seen
        }
    }
    return 1; // Row is valid
}

// Function to check if a column in the Sudoku grid is valid
int isColumnValid(int grid[N][N], int col)
{
    int seen[N + 1] = {0}; // Initialize seen array to keep track of seen numbers
    for (int i = 0; i < N; i++)
    {
        if (grid[i][col] != 0)
        {
            if (seen[grid[i][col]] == 1) // If number is already seen, column is invalid
                return 0;
            seen[grid[i][col]] = 1; // Mark the number as seen
        }
    }
    return 1; // Column is valid
}

// Function to play Hangman game
// Function to play Hangman game
void hangmanGame()
{
    int difficulty;
    printf("\n\n");
    printf("Choose the difficulty level:\n");
    printf("1. Easy\n");
    printf("2. Medium\n");
    printf("3. Hard\n");
    printf("Enter your choice: ");
    scanf("%d", &difficulty);

    char words[MAX_WORDS][MAX_WORD_LENGTH];
    char hints[MAX_WORDS][MAX_WORD_LENGTH];
    int wordCount = 0;
    readHangmanWords(words, hints, &wordCount, difficulty);

    srand(time(NULL));
    int chosenIndex = rand() % wordCount;
    char chosenWord[MAX_WORD_LENGTH];
    strcpy(chosenWord, words[chosenIndex]);
    char hint[MAX_WORD_LENGTH];
    strcpy(hint, hints[chosenIndex]);

    bool guessedLetters[MAX_WORD_LENGTH] = {false}; // To track guessed letters
    int incorrectGuesses = 0;

    printf("\n%s\n", hint); // Display the hint
    printf("Guess the word by entering one letter at a time.\n");
    printf("You have %d attempts to guess the word.\n", MAX_TRIES);

    while (incorrectGuesses < MAX_TRIES)
    {
        printf("\n");
        displayWord(chosenWord, guessedLetters);
        printf("\n\nEnter your guess: ");
        char guess;
        scanf(" %c", &guess);
        guess = tolower(guess);

        // Clear input buffer
        while (getchar() != '\n');

        bool found = false;
        for (int i = 0; chosenWord[i] != '\0'; i++)
        {
            if (chosenWord[i] == guess)
            {
                guessedLetters[i] = true;
                found = true;
            }
        }

        if (!found)
        {
            incorrectGuesses++;
            printf("\nIncorrect guess! Attempts left: %d\n", MAX_TRIES - incorrectGuesses);
            drawHangman(incorrectGuesses);
        }

        bool allGuessed = true;
        for (int i = 0; chosenWord[i] != '\0'; i++)
        {
            if (!guessedLetters[i])
            {
                allGuessed = false;
                break;
            }
        }
        if (allGuessed)
        {
            printf("\nCongratulations! You guessed the word: %s\n", chosenWord);
            return;
        }
    }
    printf("\n\n OOPS!... you have lost the Game.\n The Hangman is Hanged\n\n");
    printf("\nSorry, you've run out of attempts!\n\n The word was: %s\n", chosenWord);
}

// Function to read Hangman words and hints from a file based on difficulty level
void readHangmanWords(char words[MAX_WORDS][MAX_WORD_LENGTH], char hints[MAX_WORDS][MAX_WORD_LENGTH], int *wordCount, int difficulty)
{
    FILE *file = fopen("words.txt", "r");
    if (file == NULL)
    {
        printf("Error opening words file.\n");
        exit(1);
    }

    int minWordLength, maxWordLength;
    if (difficulty == 1)
    {
        minWordLength = 1;
        maxWordLength = 5;
    }
    else if (difficulty == 2)
    {
        minWordLength = 6;
        maxWordLength = 8;
    }
    else if (difficulty == 3)
    {
        minWordLength = 9;
        maxWordLength = MAX_WORD_LENGTH;
    }
    else
    {
        printf("Invalid difficulty level.\n");
        return;
    }

    *wordCount = 0;
    while ((*wordCount < MAX_WORDS) && (fscanf(file, "%s - %[^\n]", words[*wordCount], hints[*wordCount]) == 2))
    {
        if (strlen(words[*wordCount]) >= minWordLength && strlen(words[*wordCount]) <= maxWordLength)
        {
            (*wordCount)++;
        }
    }

    fclose(file);
}


// Function to print Hangman game information
void printHangmanInfo()
{
    FILE *file = fopen("Hangman.txt", "r");
    if (file == NULL)
    {
        printf("Error opening Hangman info file.\n");
        exit(1);
    }

    printf("\n\n\tHangman Game Information:\n");
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("\t%s", line);
    }
    printf("\n\n\n");
    fclose(file);
}

// Function to draw Hangman based on number of incorrect guesses
void drawHangman(int attempts)
{
    printf("\n\n");
    switch (attempts)
    {
    case 1:
        printf("\t  +---+\n");
        printf("\t  |   |\n");
        printf("\t      |\n");
        printf("\t      |\n");
        printf("\t      |\n");
        printf("\t      |\n");
        printf("\t=========\n");
        break;
    case 2:
        printf("\t  +---+\n");
        printf("\t  |   |\n");
        printf("\t  O   |\n");
        printf("\t      |\n");
        printf("\t      |\n");
        printf("\t      |\n");
        printf("\t=========\n");
        break;
    case 3:
        printf("\t  +---+\n");
        printf("\t  |   |\n");
        printf("\t  O   |\n");
        printf("\t  |   |\n");
        printf("\t      |\n");
        printf("\t      |\n");
        printf("\t=========\n");
        break;
    case 4:
        printf("\t  +---+\n");
        printf("\t  |   |\n");
        printf("\t  O   |\n");
        printf("\t /|   |\n");
        printf("\t      |\n");
        printf("\t      |\n");
        printf("\t=========\n");
        break;
    case 5:
        printf("\t  +---+\n");
        printf("\t  |   |\n");
        printf("\t  O   |\n");
        printf("\t /|\\  |\n");
        printf("\t      |\n");
        printf("\t      |\n");
        printf("\t=========\n");
        break;
    case 6:
        printf("\t  +---+\n");
        printf("\t  |   |\n");
        printf("\t  O   |\n");
        printf("\t /|\\  |\n");
        printf("\t /    |\n");
        printf("\t      |\n");
        printf("\t=========\n");
        break;
    case 7:
        printf("\t  +---+\n");
        printf("\t  |   |\n");
        printf("\t  O   |\n");
        printf("\t /|\\  |\n");
        printf("\t / \\  |\n");
        printf("\t      |\n");
        printf("\t=========\n");
        break;
    default:
        break;
    }
}

// Function to display the word with guessed letters
void displayWord(char *word, bool *guessedLetters)
{
    printf("\tWord: ");
    for (int i = 0; word[i] != '\0'; i++)
    {
        if (guessedLetters[i])
        {
            printf("%c ", word[i]);
        }
        else
        {
            printf("_ ");
        }
    }
}



