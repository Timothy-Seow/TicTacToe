#include <stdio.h>
#include <string.h>

int main(){
    FILE *file_ptr;
    file_ptr=fopen("tic-tac-toe.data","r");

    if (file_ptr == NULL){
        printf("error reading file\n");
    }

    // Read file character by character and store it in a 3D array
    int ch = 0, move_count = 0, item_count = 0, char_count = 0, letter = 0;
    char buffer[10][10];
    char legal_moves[958][10][10];
    char temp[10];
    
    /*
    while((ch=fgetc(file_ptr))!=EOF){
        //printf("%d", char_count);
        printf("%c", (char)ch);
        if (ch == 44)
        {
            char_count++;
            continue;
        }
        else if (char_count >= 18)
        {  
            temp[letter++] = (char)ch;
            if (letter == 8)
            {
                temp[letter] = '\0';
                //printf("%s", temp);
                strcpy(buffer[item_count], temp);
                memcpy(legal_moves[move_count++], buffer, sizeof(buffer));
                item_count = 0;
                char_count = 0;
                letter = 0;
            }
        }
        else
        {
            temp[0] = (char)ch;
            temp[1] = '\0';
            strcpy(buffer[item_count++], temp);
        }
        char_count++;
    }

    */

    while (fscanf(file_ptr, "%c,%c,%c,%c,%c,%c,"))

    fclose(file_ptr);

   
}