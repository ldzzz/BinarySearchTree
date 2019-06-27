//-----------------------------------------------------------------------------
// ass2.c
//
// Implementation of the simple textadventure game for the ESP assignement 2
// Goal is to parse multiple files recursively, and create a binary tree,
// then depending on the user input, we go down the tree to the end of the story
//
//-----------------------------------------------------------------------------
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//typedefining bool
typedef int bool;
#define TRUE 1
#define FALSE 0

#define FILE_ERROR 3
#define MEMORY_OUT 2
#define MAX_INPUT_LENGTH 256


//------------------------------------------------------------------------------
///
/// defining struct _Chapter_ 
//
// @char *title, pointer to string title
// @char *text, pointer to string text
// @ struct *answer_A_, pointer to the struct for answer A
// @ struct *answer_B_, pointer to the struct for answer B
//
//
// typedefining our struct with name Chapter
//
typedef struct _Chapter_
{
  char *title_;
  char *text_;
  struct _Chapter_ *answer_A_;
  struct _Chapter_ *answer_B_;
}Chapter;


//forward declarations
Chapter *createChapter(char *title, char *text);
void destroyStory(Chapter *chapter);
Chapter *parseString(char *file_content);
short fileExists(const char *filename);
char *readFile(const char *filename);
short playGame(Chapter *root);
short compareInput(char user_input[]);
Chapter *chooseNextChapter(char user_input[], Chapter *chapter);
short printChapter(Chapter *chapter);

///-----------------------------------------------------------------------------
///
/// Main function
//
// check if the argc == 2
// read the initial into the local variable that is a pointer called file_string 
// 
// check the return of readFile function stored in file_string
// and give out a corresponding error
//
// create Chapter *root, and initialize it with the return value of 
// parseString(file_string), this is now pointer to the root node of the 
// binary tree
//
// call playGame function and pass it  the address to which root is pointing
// at the end we free all of the memory allocated by calling destroyStory 
// function, and passing it the address of the root of the tree
// 
// @return TRUE, if the command line parameter count is invalid(that is != 2)
// @return FALSE, if everything was fine
//
int main(int argc, char const *argv[])
{
  if (argc != 2)
  {
    printf("Usage: ./ass2 [file-name]\n");
    return TRUE;
  }

  char *file_string = readFile(argv[1]);
  if(file_string == NULL)
  {
    return FILE_ERROR;
  }
  Chapter *root = parseString(file_string);

  playGame(root);
  destroyStory(root);

  return FALSE;
}


//------------------------------------------------------------------------------
///
/// function createChapter
//
// @param title, pointer to string title
// @param text, pointer to string text
//
// function that serves to allocate memory and create a new node
// here we allocate memory for our new struct
// then we copy the title and text to the created struct and set its child nodes
// to NULL
//
// @return new_chapter, the struct that we created
//
Chapter *createChapter(char *title, char *text)
{
  Chapter *new_chapter = malloc(sizeof(Chapter));
  new_chapter->title_ = title;
  new_chapter->text_ = text;
  new_chapter->answer_A_ = NULL;
  new_chapter->answer_B_ = NULL; 
  return new_chapter;
}


///-----------------------------------------------------------------------------
///
/// parseString function
// 
// @param file_content, pointer to memory address where the string is stored
// 
// @var elem, is being incremented to go through string, char by char
// @var title, is holding a corresponding pointer where the title starts
// @var first_filename, is holding a pointer where the first filename starts
// @var second_filename, is holding a pointer where the second filename starts
// @var text, is holding pointer to where the text starts
// @var counter, counting how many newlines were encountered
//
// here we will parse the given string in such manner that the first line is 
// representing title, second line is name of the first file and third line is
// name of the second file, whilst everything else after that is being text
//
// in an infinite loop, we keep looping until we reach the nul terminator,
// which marks the end of the string
// instead of first three newlines(those after title,first and second filename)
// we will put \0, so all of the string operations are valid and we know where
// our string ends for those variables
//
// after that, we create the chapter, over createChapter function 
// with corresponding text and title, then we will have an if statement
// which serves us to check if filenames are '-', meaning no more storyline
// we keep recursively parsing all of the files and initializing all of the 
// neccessary nodes for answer_A_ and answer_B_
//
// @return chapter, returning pointer to root node, where our newly created
//  binary tree starts
//
Chapter *parseString(char *file_content)
{
  int elem = 0;
  char *title = NULL;
  title = file_content;
  char *first_filename = NULL;
  char *second_filename = NULL;
  char *text = NULL;

  int counter = 0;

  while(file_content[elem] != '\0')
  {
    if(file_content[elem] == '\n')
    {
      if(counter == 0)
      {
        file_content[elem] = '\0';
        counter++;
        first_filename = file_content + elem + 1;
      }
      else if(counter == 1)
      {
        file_content[elem] = '\0';
        counter++;
        second_filename = file_content + elem + 1;
      }
      else
      {
        file_content[elem] = '\0';
        text = file_content + elem + 1;
        break;
      }
    }
    elem++;
  }
  Chapter *chapter = createChapter(title, text);
    //TODO: handle this NULL return
  if(chapter == NULL)
  {
    free(chapter);
    printf("[ERR] Out of memory.\n");
    return NULL;
  }
  //TODO: fileExists needs to return something meaningful to differ if file exists or not
  printf("FIRST %s\n", first_filename);
  printf("SECOND %s\n", second_filename);
  if((first_filename[0] != '-') && (second_filename[0] != '-'))
  {
      chapter->answer_A_ = parseString(readFile(first_filename));
      chapter->answer_B_ = parseString(readFile(second_filename));
  }
  return chapter;
}

///-----------------------------------------------------------------------------
///
/// readFile function
//
// @param filename, pointer to the filename
// 
// here we check if file that was given is valid by utilizing fileExists func
// if everything is fine with the file, we will open it and get the num of chars
// in it, then proceed to allocate memory needed for the whole file string
// if there is no memory available, then we check and return correspondingly
// the whole file will be saved into the local variable called file_content
// which is pointer to the string
// we close the file and return
//
//
// @return NULL, if file doesnt exist or out of memory
// @return file_content, returning the address of string in memory
//
char *readFile(const char *filename)
{
   if(!fileExists(filename))
   {
     printf("[ERR] Could not read file %s.\n", filename);
     return NULL;
   }

  FILE *file = fopen(filename, "r"); 
  fseek(file, 0, SEEK_END);
  int file_length = ftell(file);
  rewind(file);
  char *file_content = (char*)calloc(file_length + 2, sizeof(char));
  //TODO: handle different NULL returns
  if(file_content == NULL)
  {
    free(file_content);
    printf("[ERR] Out of memory.\n");
    return NULL;
  }
  fread(file_content, sizeof(char), file_length, file);
  file_content[file_length + 1] = '\0';
  fclose(file);
  return file_content;
}

///-----------------------------------------------------------------------------
///
/// fileExists function
//
// @param filename, pointer to the filename
// 
// we will open our file and check if it is empty/corrupted/doesnt exist or not
//
// @return TRUE, if file is readable/exists
// @return FALSE, file is not readable/doesn't exist
//
short fileExists(const char *filename) 
{
  FILE *file_stream;
  if((file_stream = fopen(filename, "r")) != NULL) 
  {
      fclose(file_stream);
      return TRUE;
  }
  return FALSE;
}

///-----------------------------------------------------------------------------
///
/// playGame function
//
// @param root, pointer to the memory where our struct resides 
// 
// @var user_choice[MAX_INPU_LENGTH], variable where we store users input
// in an infinite loop, until the root is not NULL
// first we check return value of printChapter(), if it's 1 we break out of loop
// meaning, we reached the end of the storyline
// 
// then in a do..while loop we read the users input, and keep doing it until
// the user inputs valid string(A or B), as well we check for the EOF
// @return FALSE, if EOF reached
//
// after all we set our root to be the next corresponding chapter
// by giving it the return value of chooseNextChapter
//
// @return FALSE, if everything was ok
//
short playGame(Chapter *root)
{
  char user_choice[MAX_INPUT_LENGTH];
  while(root != NULL)
  {
    if(printChapter(root))
    {
      break;
    }

    do
    {
      if(scanf("%s",user_choice) == EOF)
      {
        return FALSE;
      }
    }
    while(!compareInput(user_choice) && printf("[ERR] Please enter A or B.\n"));

    root = chooseNextChapter(user_choice, root);
  }
  return FALSE;
}

///-----------------------------------------------------------------------------
///
/// compareInput function
//
// @param, user_input[], array of chars(string)
// 
// @return 1 or 0, depending of the logic behind
//  if user had correct input A or B, we return 1, else we return 0
//
short compareInput(char user_input[])
{
  return ((user_input[0] == 'A' || user_input[0] == 'B') && 
    (user_input[1] == '\0'));
}

///-----------------------------------------------------------------------------
///
/// chooseNextChapter function
// 
// @param user_input [], array of chars(string)
// @param chapter, pointer to the memory where current chapter resides
// 
// in this function we check which storyline did user choose, A or B
//
// @return chapter->answer_A_, if user has answered 'A'
// @return chapter->answer_B_, if user has answered 'B'
//
Chapter *chooseNextChapter(char user_input[], Chapter *chapter)
{
  if(user_input[0] == 'A')
  {
    return chapter->answer_A_;
  }
  return chapter->answer_B_;
}


///-----------------------------------------------------------------------------
///
/// printChapter function
//
// @ param chapter, pointer to memory where the current chapter is,to print 
// 
// if the nodes of given chapter are NULL, meaning no more chapters
// then we print that chapters text and title with the "ENDE" at the end to mark
// the end of the storyline
//  
// if nodes are not NULL, then we print title and text and Corresponding
// "Deine Wahl (A/B)?" at the end
//
// @return FALSE, if story continues
// @return TRUE, if story ends
//
short printChapter(Chapter *chapter)
{
  if(chapter->answer_A_ == NULL && chapter->answer_B_ == NULL)
  {
    printf("------------------------------\n[%s]\n\n[%s]\n\nENDE\n",
    chapter->title_, chapter->text_);

    return TRUE;
  }
  printf("------------------------------\n[%s]\n\n[%s]\n\nDeine Wahl (A/B)?",
    chapter->title_, chapter->text_);

  return FALSE;
}

//------------------------------------------------------------------------------
///
/// function destroyStory
//
// @param chapter, pointer to struct Chapter in memory
// 
// function to free the memory occupied by our structs(binary tree)
// we check if our node(chapter) is NULL, if not, then we free the title
//
// we free the title as it holds the pointer to the memory that we initilized 
// when we read in the whole file into a string
//
// we proceed to free its left and right child nodes(answer_A_ and answer_B_)
// at the end we free the node itself(chapter)
//
// @return none, type void
//
void destroyStory(Chapter *chapter)
{
  if (chapter != NULL)
  {
    free(chapter->title_);
    destroyStory(chapter->answer_A_);
    destroyStory(chapter->answer_B_);
    free(chapter);
  }
} 
