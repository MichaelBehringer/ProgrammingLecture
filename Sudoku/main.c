/* 
 * File:   main.c
 * Author: Michael Behringer
 *         Baden-Wuerttemberg Cooperative State University
 *
 * Created on 5. Januar 2022, 10:40
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stack.h"

int main(int argc, char** argv) {
    char name[256];
    char column;
    int line;
    int valueInput;
    int isRuleCheckActive = 1;
    int counter = 10;
    int fieldValues[9][9] = {0};
    int savedFieldValues[9][9] = {0};
    int colorArray[9][9] = {0};
    size_t i_elementSize=sizeof(position);
    size_t i_maxAmount=81;
    myStack_t* pMyStack = StackNew(i_elementSize, i_maxAmount);
    myStack_t* pMyInputStack = StackNew(i_elementSize, i_maxAmount);
    srand(time(NULL));
    printf("Please enter your name: ");
    scanf("%s", name);
    getchar();
    
    generateFieldValues(fieldValues, pMyStack);
    
    for(int i = 0; i<9; i++) {
        for(int j = 0; j<9; j++) {
            savedFieldValues[i][j] = fieldValues[i][j];
        }
    }
    
    int numbersToDelete = 0;
    printf("Wie viele Zahlen sollen sichtbar sein?\n");
    printf("Mindestens 17 und max, 36 Zahlen sollten sichtbar sein: ");
    scanf("%d", &numbersToDelete);
    getchar();
    numbersToDelete = 81 - numbersToDelete;
    
    for(int i = 0; i<numbersToDelete; i++){
        position top;
        Pop (pMyStack, &top);
        fieldValues[top.line][top.row] = 0;
        colorArray[top.line][top.row] = 1;
    }
    
    printGameBoard(fieldValues, name, colorArray);
    
    while(1) {
        printf("menue: q=quit p=printGameBoard r=toggleRules u=undo \n");
        printf("Please enter Column Line Value (e 2 9): ");
        scanf("%c %d %d", &column, &line, &valueInput);
        getchar();
        
        if(column == 'q') {
            break;
        }
        
        if(column == 'p') {
            printGameBoard(savedFieldValues, name, colorArray);
            continue;
        }
        
        if(column == 'r') {
            isRuleCheckActive = !isRuleCheckActive;
            printf("%s", isRuleCheckActive == 1 ? "Regeln Aktiviert\n" : "Regeln Deaktiviert\n");
            continue;
        }
        
        if(column == 'u') {
            if(IsStackEmpty(pMyInputStack)) {
                printf("\033[1;31m");
                printf("Rueckgaenging nicht moeglich!\n");
                printf("\033[0m");
            } else {
                position top;
                Pop (pMyInputStack, &top);
                fieldValues[top.line][top.row] = 0;
                printGameBoard(fieldValues, name, colorArray);
            }
            continue;
        }
        
        if(colorArray[line-1][column-'a'] == 0) {
            printf("\033[1;31m");
            printf("Feld darf nicht veraendert werden!\n");
            printf("\033[0m");
            continue;
        }
        
        int checkResult = checkFields(fieldValues, line-1, column-'a', valueInput);
        if(checkResult==1 || isRuleCheckActive==0){
            fieldValues[line-1][column-'a'] = valueInput;
                position record; 
            record.line=line-1; 
            record.row=column-'a';
            Push(pMyInputStack,&record);
        } else {
            printf("\033[1;31m");
            printf("Zahl passt nicht!\n");
            printf("\033[0m");
            continue;
        }
        
        
        
        printGameBoard(fieldValues, name, colorArray);
    }
    
    
    
    
    return (EXIT_SUCCESS);
}

void generateFieldValues(int fieldValues[9][9], myStack_t* pMyStack){
    int realCounter = 0;
    int failCounter = 1;
    while(1) {
        int column =  rand()%9;
        int line = rand()%9;
        
        if(fieldValues[line][column]!=0){
            continue;
        }
        
        int sometingFound = 0;
        for(int value2 = 1; value2 <=9; value2++){
            int checkResult = checkFields(fieldValues, line, column, value2);
            if(checkResult == 1) {
                fieldValues[line][column] = value2;
                position record; 
                record.line=line; 
                record.row=column;
                Push(pMyStack,&record);
                realCounter++;
                sometingFound = 1;
                failCounter = 1;
                break;
            }
        }
        
        if(realCounter == 81) {
            break;
        }
        
        if(sometingFound == 0){
            failCounter*=3;
            for(int i = 0; i<failCounter; i++){
                position top;
                Pop (pMyStack, &top);
                fieldValues[top.line][top.row] = 0;
                realCounter--;
            }
        }
    }
}

void printHyphenLine(){
    printf("  --- --- ---\n");
}

void printLine(int fieldValues[9][9], int line, int colorArray[9][9]){
    printf("%d|", line);
    for(int i = 0; i < 9 ; i++) {
        printf(colorArray[line-1][i] == 0 ? "\033[0m" : "\033[0;34m");
        printf("%c", fieldValues[line-1][i] == 0 ? ' ' : fieldValues[line-1][i] + '0');
        if((i+1)%3==0) {
            printf("\033[0m");
            printf("|");
        }
    }
    printf("\n");
}

void printGameBoard(int fieldValues[9][9], char name[], int colorArray[9][9]) {
    printf("\n\n");
    
    printf("Player: %s\n", name);
    printf("  abc def ghi \n");
    printHyphenLine();
    for(int i = 0; i<3; i++) {
        for(int j = 1; j<=3; j++) {
            printLine(fieldValues, j + 3*i, colorArray);
        }
        printHyphenLine();
    }
}

int checkFields(int fieldValues[9][9], int line, int column, int value) {
    if(fieldValues[line][column]!=0){
        return 0;
    }
    
    for(int i = 0; i < 9; i++) {
        if(fieldValues[line][i] == value) {
            return 0;
        }
        if(fieldValues[i][column] == value) {
            return 0;
        }
    }
    
    int searchColumn = column / 3;
    int searchLine = line / 3;
    searchColumn *=3;
    searchLine *=3;
    
    for(int fcolumn = searchColumn; fcolumn < searchColumn +3; fcolumn++) {
        for(int fline = searchLine; fline < searchLine +3; fline++) {
            if(fieldValues[fline][fcolumn] == value) {
                return 0;
            }
        }
    }
    
    return 1;
}