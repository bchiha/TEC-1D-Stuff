// TEXT to SP0256a-AL2 Allophone converter by Brian Chiha
//-------------------------------------------------------
//
// Designed to be used for the Talking Electroinc Computer Speech module add on
//
// Compile and run, then type some text.  Type PP to insert a 200ms pause and EOF 
// to insert 0xFF (End of file for TEC running program).  Control-D to exit.
// Note: Puctuation must be seperated with a space or don't use it at all.
//
// Output is given as Allophones and the SP0256a-AL2 hex equivilant.  If a word
// can't be found '??' is outputed and a '.' represents a word gap.  A pause of 100ms is
// automatically placed between words.
//
// To be used with the TEC Speech Module attached to Port 7.
//
// Options
// -b 		Output Binary File which includes main program code
// -w       Don't include main program in bianary file (used with -b)
//
// For binary output to directly load into the TEC. use -b as a command line option.
// Binary files are created with the code to activate the Speech Module and the speech 
// data.  If you just want the allophones to be outputted use -w as well.  A new file is created
// after each Carriage Return.  Binary file are to be loaded at address 0x900 on the TEC
//
// Files that are requried are:
// text2allophone.c   - This Program
// cmudict-0.7b       - CMU dictionary file that associates words to the ARPAbet phoneme set
//                    - see http://www.speech.cs.cmu.edu/cgi-bin/cmudict
// cmu2sp0.symbols    - Convert CMU to SPO256a Allophones

/* sample output from the program
> Welcome to Talking Electronics PP I hope you have a great day ! EOF
TXT> WW EH LL KK2 AX MM . TT2 UW2 . TT2 AO KK2 IH NG . IH LL EH KK2 TT2 RR1 AA
     NN1 IH KK2 SS . AY . HH2 OW PP . YY2 UW2 . HH2 AE VV . AX . GG3 RR1 EY TT2 .
     DD2 EY . EH KK2 SS KK2 LL AX MM EY SH AX NN1 PP OY NN1 TT2 .

000> 2E 07 2D 29 0F 10 03 0D
008> 1F 03 0D 17 29 0C 2C 03
010> 0C 2D 07 29 0D 0E 18 0B
018> 0C 29 37 03 04 06 03 39
020> 35 09 03 19 1F 03 39 1A
028> 23 03 0F 03 22 0E 14 0D
030> 03 21 14 03 07 29 37 29
038> 2D 0F 10 14 25 0F 0B 09
040> 05 0B 0D 03 FF
*/

/* For reference, here is the Test Program that is used to run the Speech Module taking in data
   for the SP0256a-AL2 chip.  It is included in the binary output by default

0900	21 10 09	LD HL,0910		;Location of allophone data
0903	7E			LD A,(HL)		;Load A with the next allophone
0904	FE FF 		CP 0xFF			;Compare A with 0xFF (EOF for data)
0906	28 05       JR Z,090D       ;IF EOF then jump to address 090D
0908    D3 07		OUT (07),A      ;Output A to port 7 on the TEC
090A	23			INC HL          ;Index to the next allophone
090B	18 F6       JR 0903         ;Jump back to 0903 to say the next allophone
090D    76          HALT            ;Wait for key input as EOF has reached
090E    18 F0       JR 0900         ;Jump back to start

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SYMBOL_LEN 4
#define MAX_SYMBOLS 84
#define MAX_TREE_HEIGHT 20
#define MAX_KEY_LENGTH 35
#define MAX_INPUT_SIZE 5000
#define WHITESPACE " "

#define ODD(x) ((x)/2*2 != (x))

int test_code_z80[16] = {0x21,0x10,0x09,0x7E,0xFE,0xFF,0x28,0x05,0xD3,0x07,0x23,0x18,0xF6,0x76,0x18,0xF0};

FILE *dictptr=NULL;   //file pointer to dictionary

//data structures
typedef struct symdat {
    char cmu[SYMBOL_LEN];
    char sp0[SYMBOL_LEN];
    unsigned int hex;
} Symdat;

//list structure
typedef struct list_tag {
	char symbol[SYMBOL_LEN];
	struct list_tag *next;
} List_type;

//tree structure
typedef struct node_tag {
    char key[MAX_KEY_LENGTH];
    List_type *allophones;
    struct node_tag *left;
    struct node_tag *right;
} Node_type;

void upper_string(char s[]) {
   int c = 0;
   
   while (s[c] != '\0') {
      if (s[c] >= 'a' && s[c] <= 'z') {
         s[c] = s[c] - 32;
      }
      c++;
   }
}

//Binary Tree functions for dictionary

//Find the highest power of 2 that divides count
int Power2(int count) {
    int level;
    
    for (level = 0; !ODD(count); level++)
        count /= 2;
    return level;
}

//Find the root of the tree
Node_type *FindRoot(Node_type *lastnode[]) {
    int level;
    for (level = MAX_TREE_HEIGHT-1; level > 0 && !lastnode[level]; level--)
        ;  //get highest level that isn't null
    if (level <= 0)
        return NULL;
    else
        return lastnode[level];
}

//Connect free subtrees from lastnode[]
void ConnectSubtrees(Node_type *lastnode[]) {
    Node_type *p;
    int level, templevel;
    
    for (level = MAX_TREE_HEIGHT-1; level > 2 && !lastnode[level]; level--)
        ;
    while (level > 2) {
        if (lastnode[level]->right)
            level--;
        else {
            p = lastnode[level]->left;
            templevel = level - 1;
            do {
                p = p->right;
            } while (p && p == lastnode[--templevel]);
            lastnode[level]->right = lastnode[templevel];
            level = templevel;
        }
    }
}

//Insert node p into the right most node of the tree
void InsertNode(Node_type *p, int count, Node_type *lastnode[]) {
    int level = Power2(count) + 1;
    
    p->right = NULL;
    p->left = lastnode[level - 1];
    lastnode[level] = p;
    if (lastnode[level+1] && !lastnode[level+1]->right)
        lastnode[level+1]->right = p;
}

//Get the next word from dictionary file
Node_type *GetNode(void) {
    Node_type *p=NULL;
    List_type *prev_head=NULL;

    char line[128];
    char *token;
    
    if (!feof(dictptr) && fgets(line, 128, dictptr)) {
        /* remove new line character*/
        char *newline = strchr(line, '\n');
        if (newline)
        	*newline = 0;
        /* get the word */
        token = strtok(line, WHITESPACE);
        p = malloc(sizeof(Node_type));

        strcpy(p->key,token);
        p->left = p->right = NULL;
        p->allophones = NULL;
        /* walk through the allophones and add them to the node */
      	token = strtok(NULL, WHITESPACE);  //will have atleast 1 allophone
		while( token != NULL ) {
      		//create list
      		List_type* sp = malloc(sizeof(List_type));
      		strcpy(sp->symbol,token);
      		sp->next = NULL;
      		//add list to end of symbols
      		if (p->allophones)
   				prev_head->next = sp;
      		else
      			p->allophones = sp;
			prev_head = sp;
            token = strtok(NULL, WHITESPACE);
   		}
    }
    return p;
}


//Build Tree assumes entries are already sorted when inserted
Node_type *BuildCMUTree(void) {
    Node_type *p;   //current node
    int count = 0;  //number of nodes so far
    int level;      //level for current nodes
    Node_type *lastnode[MAX_TREE_HEIGHT];  //last node of each level
    
    for (level = 0; level < MAX_TREE_HEIGHT; level++)
        lastnode[level] = NULL;

    //open cmu to sp0 file
    if ((dictptr = fopen("cmudict-0.7b","r")) == NULL) {
       printf("Error! opening cmudict-0.7b file");
       exit(1);
    }

    while ((p=GetNode()) != NULL)
        InsertNode(p, ++count, lastnode);
    p = FindRoot(lastnode);
    ConnectSubtrees(lastnode);
    return p;
}

//Tree search and return key
Node_type *TreeSearch(Node_type *p, char target[]) {
    while (p && strcmp(target, p->key) != 0)
        if (strcmp(target, p->key) < 0)
            p = p->left;
        else
            p = p->right;
    return p;
}

Symdat **LoadSymbolTable(char filename[]) {
    FILE *fp=NULL;
    Symdat **sd;
    char tmp_cmu[SYMBOL_LEN], tmp_sp0[SYMBOL_LEN];
    unsigned int tmp_hex;
    int sd_index=0;
    sd = malloc(sizeof(Symdat)*MAX_SYMBOLS);
    
    //open cmu to sp0 file
    if ((fp = fopen(filename,"r")) == NULL) {
       printf("Error! opening cmu2sp0.symbols file");
       exit(1);
    }
    //read file and store in sd array
    while(fscanf(fp,"%3s %3s %x",tmp_cmu, tmp_sp0, &tmp_hex)==3) {
        Symdat * newSymdat;
        newSymdat = malloc(sizeof(Symdat));
        for (int i = 0; i < 4; i++) {
            newSymdat->cmu[i] = tmp_cmu[i];
            newSymdat->sp0[i] = tmp_sp0[i];
        }
        newSymdat->hex = tmp_hex;
        sd[sd_index] = newSymdat;
        sd_index++;
    }
    fclose(fp);
    return sd;
}

// Main entry point.  Takes in options args -b and -w
int main(int argc, char *argv[]) {
    FILE *fp=NULL,*fout=NULL;
    Symdat **sd;
    char *token;
    Node_type *dictionary, *target;
    List_type *curr_symbol;
    char input_text[MAX_INPUT_SIZE];
    char bin_file_name[14];
    unsigned int input_hex[MAX_INPUT_SIZE];
    int sd_index,hex_index=0;
    int opt;
    int f_binary_file=0;
    int f_without_header=0;
    int file_count=0;

    //parse options if any
	while((opt = getopt(argc, argv, ":bw")) != -1) 
	{ 
		switch(opt) 
		{ 
			case 'b':
				f_binary_file = 1;
				break;
			case 'w':
				f_without_header = 1; 
				break;
		} 
	} 
	
    //load data files
    sd = LoadSymbolTable("cmu2sp0.symbols");
    dictionary = BuildCMUTree();
    
    //get input text
    printf("Text to SPO256a converter by Brian Chiha\n");
    printf("----------------------------------------\n");
    printf("Type in a sentence to convert...(EOF for FF, PP for 200ms Pause, C-d to exit)\n\n> ");
    
    while(fgets(input_text,MAX_INPUT_SIZE,stdin)) {
    	//open file if binary output selected
    	if (f_binary_file) {
    		sprintf(bin_file_name,"speech%03d.bin",file_count++);
    		fout = fopen(bin_file_name,"wb");
    		if (!f_without_header)
    			for (int address = 0; address < 16; address++)
				    fprintf(fout,"%c",test_code_z80[address]);
    	} 

	    /* remove new line character*/
	    char *newline = strchr(input_text, '\n');
	    *newline = 0;

	    // parse words keyed in and convert to allophones
		printf("TXT> ");
		for (token = strtok(input_text, WHITESPACE); token != NULL; token = strtok(NULL, WHITESPACE))
		{
			upper_string(token);
			target = TreeSearch(dictionary,token);
			if (target) {
				curr_symbol = target->allophones;
	        	while (curr_symbol) {
	        		for (sd_index=0; strcmp(sd[sd_index]->cmu,curr_symbol->symbol) != 0; sd_index++)
	        			;
	        		printf("%s ",sd[sd_index]->sp0);
	        		input_hex[hex_index++] = sd[sd_index]->hex; 
	        		curr_symbol = curr_symbol->next;
	        	}
		        printf(". ");  //end of word
		        input_hex[hex_index++] = 0x03; //word pause gap
			}
			else 
			    if (strcmp(token,"EOF") == 0)
			    	input_hex[hex_index++] = 0xFF;
			    else if (strcmp(token,"PP") == 0)
			    	input_hex[hex_index++] = 0x04;
			    else
			     printf("?? . "); //no word found
		}
		printf("\n");
		// display hex data stored in input_hex
		for (int i = 0; i < hex_index ; i++) {
			if ((i % 8) == 0)
				printf("\n%03X> ",i);
			printf("%02X ",input_hex[i]);
			if (f_binary_file)
				fprintf(fout,"%c",input_hex[i]);
		}
		//reset indexes and close file
		hex_index=0;
		if (fout)
			fclose(fout);
		printf("\n\n> ");
	}
    return 0;
}

//file writing
//int main()
//{
//    loadCmuRefFile();

//  FILE *fptr;
//   fptr = fopen("blank.bin","wb");
//   for (int address = 0; address <= 2047; address += 1) {
//      fprintf(fptr,"%c",0x00); 
//   }
//fclose(fptr);
//return 0;
//}
