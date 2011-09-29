/* This uses Levenshtein distance as Edit distance and uses BK Trees for optimizing in searching the closest word to the query word */
/* The algorithm and explanation about BK Trees are explained in these links */
/*   http://blog.notdot.net/2007/4/Damn-Cool-Algorithms-Part-1-BK-Trees    */
/*   http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.160.2510&rep=rep1&type=pdf   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BK_Node {
        char str[25];
        struct BK_Node *children[26];
};

struct BK_Node *root = NULL;
int Prev_row[26], This_row[26];
int min_distance = -1;
int total_edit_distance = 0;
char good_word[26];

int minimum(int a, int b, int c) {
	if( a<=b && a<=c )
		return a;
	if( b<=a && b<=c )
		return b;
	return c;
}

int LevenshteinDistance(char *s1, char *s2) {

	int s1_length = strlen(s1);
	int s2_length = strlen(s2);
	int i = 0, j = 0;
	int *prev_row, *this_row, *temp;

	prev_row = Prev_row;
	this_row = This_row;

	for(j = 0; j <= s2_length; j++)	
   		prev_row[j] = j;

	for(i = 1; i <= s1_length; i++) {
   		j = 0;
   		this_row[j] = i;
   		for(j = 1; j <= s2_length; j++) {
     			if(s1[i-1] == s2[j-1])
       				this_row[j] = prev_row[j-1];
     			else 
       				this_row[j] = minimum(prev_row[j]+1, this_row[j-1]+1, prev_row[j-1]+1); 
		}
  		temp = prev_row;
 		prev_row = this_row;
  		this_row = temp;
   
	}

	return prev_row[s2_length];
}

void initialise_BK_Node(struct BK_Node **node, char *str) {
	int i = 0;
	strcpy((*node)->str, str);
	for(i = 0; i <= 25; i++) {
  		(*node)->children[i] = NULL;
	}
}


//inserting str into BK tree
void insert(char *str, struct BK_Node **node) {
	if(*node == NULL) {
  		*node = (struct BK_Node *)malloc(sizeof(struct BK_Node));
  		initialise_BK_Node(node, str);
  		return;
	}
	int ld = LevenshteinDistance((*node)->str, str);
	insert(str, &((*node)->children[ld]));
}

void Build_BK_Tree(char *filename) {
	FILE * fp;
	fp = fopen(filename, "r");
	char word[25];
	int i = 0, count = 0;
	while(fscanf(fp,"%s",word) != EOF) {
  		i = 0;
  		count++;
  		while(word[i] != '\0') {
     			if(word[i] >= 'A' && word[i] <= 'Z')
        			word[i] = word[i] + 32;
  				i++;
  		}
		insert(word, &root);

	}
	fclose(fp);
}

// This uses the triangle inequality explained in the links in the top of this file. The triangle inequality helps to prune the search
void search_closest_word(struct BK_Node *node, char *str) {
	if(node == NULL)
		 return;
	int j = LevenshteinDistance(node->str, str);
	if(j < min_distance || min_distance == -1) {
    		min_distance = j;
     		strcpy(good_word,node->str);
  	}
  	int k = j;
  	while((k-j) < min_distance && k<=25 ) {
   		search_closest_word(node->children[k], str);
   		k++;
  	}
	k = j-1;
	while((j-k) < min_distance && k >= 0) {
		search_closest_word(node->children[k], str);
		k--;
	}
}


int main(int argc, char *argv[]) {
	Build_BK_Tree("tw106.txt");
	FILE * fp;
	fp = fopen(argv[1], "r");
	char word[25];
	while(fscanf(fp,"%s",word) != EOF) {
		min_distance = -1;
		search_closest_word(root, word);
        	printf("%s ",good_word);  // I also print a closest matching word. The code can be modified to print all closest matching words, if needed.
		total_edit_distance = total_edit_distance + min_distance;
	}
	printf("\n%d\n",total_edit_distance);
	fclose(fp);
	return 1;
}





