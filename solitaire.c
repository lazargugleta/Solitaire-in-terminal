//-----------------------------------------------------------------------------
//
// A simplified Solitaire game. The game is controlled/played via the command
// line console, and the game is set up by calling the main function alongside
// a file which contains the order of the cards, as described on Palme.
// The game ends once all cards have been moved to deposit decks, or the user
// enters "exit\n", or EOF. The rules are the same as in regular solitaire.
// There are only 2 types of cards, reds and blacks, each has 13 instances.
// Each card is unique.
//
//-----------------------------------------------------------------------------
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


struct _Card_
{
  char color_;
  int value_;
  struct _Card_ *prev_;
  struct _Card_ *next_;
};
typedef struct _Card_ Card;

//Forward declarations
int checkCardValue(char *tok);
int checkForEmptyLine(char *line);
void setFirstPointers(Card** deck, Card* card_instance);
char checkCardColor(char* tok);
int checkForSameCard(Card *cards, int i);
int entireInputFromFile(FILE *config_file, Card *card_instance);
int checkDeckNumber(char* tok);
int checkUserInput();
int checkCardsBelow(Card card_instance);
Card* travelToTheBottom(Card* card_instance);
int checkForValidMove(Card** deck,Card *wanted_card,int current_deck,
                      int move_var);
int checkMoveForDeposit(Card** deck, Card* wanted_card,int current_deck,
                        int move_var);
int travelToTheTop(Card** deck, Card *card_instance);
Card* findCardFromMoveVar(int move_var, Card* card_instance);
int printFunctionForAbhabeStapel(Card *ptr);
int printCardFromValue(Card* ptr);
int mainPrintFunction(Card** deck);
Card* getNextCard(Card* CurrentCard);
int printLines(Card** column);
int mainGameFunction(Card** deck, Card* card_instance);








//-----------------------------------------------------------------------------
///
/// The main program.
/// Main function opens a file a reads the input. Errors are executed
/// in case of invalid file or invalid file name. Memory is allocated
/// for 26 cards. There are 7 pointers for decks and 26 pointers for
/// cards. When the board is printed we enter the while loop in order
/// to start printing our cards on the board.
///
/// @param argc used to check is program called with exactly one extra
/// argument
/// @param argv used to access a input file
///
/// @return 0 if program was ran successfully
/// @return 1 if no file name was given to the run of the program
/// @return 2 if out of memory
/// @return 3 if invalid file
//
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("[ERR] Usage: %s [file-name]\n", argv[0]);
    return 1;
  }
  FILE *config_file;
  config_file = fopen(argv[1], "r");
  Card *card_instance;
  card_instance = (Card*)malloc(26 * sizeof(Card));
  if (card_instance == NULL)
  {
	  free(card_instance);
	  printf("[ERR] Out of memory\n");
	  return 2;
  }  
  int err_var = entireInputFromFile(config_file, card_instance);
  if (err_var == 3)
  {
    printf("[ERR] Invalid file!\n");
    return 3;
  }
  else if (err_var == 2)
  {
	  free(card_instance);
	  return 2;
  }   
  Card** deck;
  deck = (Card**)malloc(7 * sizeof(Card*));
  if (deck == NULL)
  {
	  free(card_instance);
	  free(deck);
	  printf("[ERR] Out of memory\n");
	  return 2;
  }
  setFirstPointers(deck, card_instance);
  
  ///////////////////////////////////////
  err_var = mainPrintFunction(deck);
  if (err_var == 2)
  {
	  free(deck);
	  free(card_instance);
	  return 2;
  }  
  while (1)
  {
    err_var = mainGameFunction(deck, card_instance);
    if (err_var == 2)
    {
      free(card_instance);
      free(deck);
      return 2;
    }
    else if (err_var == 0)
      break;
    else
      continue;
  }
  free(deck);
  free(card_instance);
  return 0;
}










//-----------------------------------------------------------------------------
///
/// Check if game is over. Checks if user input is a valid command.
/// If user input is a valid command it changes necessary pointers and
/// prints a new board. 
///
/// @param deck array of pointers to the first card in every deck
/// @param card_instance array of cards in the double-linked list
///
/// @return 0 program is over
/// @return 1 if move command was successful
/// @return 2 if out of memory
//
int mainGameFunction(Card** deck, Card* card_instance)
{
  if ((deck[0] == NULL) && (deck[1] == NULL) && (deck[2] == NULL) &&
      (deck[3] == NULL) && (deck[4] == NULL))
  {
    return 0;
  }
  Card *wanted_card;
  int current_deck;
  int wanted_deck;
  int move_var = checkUserInput();
  if (move_var == 2)
  {
    return 2;
  }
  else if (move_var == -2)
  {
    printf("[INFO] Invalid command!\n");
    return 1;
  }
  else if (move_var == -1)
  {
    printf("possible command:\n");
    printf(" - move <color> <value> to <stacknumber>\n");
    printf(" - help\n");
    printf(" - exit\n");
    return 1;
  }
  else if (move_var == 0)
  {
    return  0;
  }
  else
  {
    wanted_card = findCardFromMoveVar(move_var, card_instance);
    current_deck = travelToTheTop(deck, wanted_card);
    wanted_deck = move_var / 100;
    if(wanted_deck == current_deck)
    {
      return -2;
    }
    if (((current_deck == 0) && (wanted_card->next_ != NULL)) ||
        (current_deck == 5) || (current_deck == 6))
    {
      printf("[INFO] Invalid move command!\n");
      move_var = -2;
    }
    else if (wanted_deck == 0)
    {
      printf("[INFO] Invalid move command!\n");
      move_var = -2;
    }
    else if ((wanted_deck == 5) || (wanted_deck == 6))
    {
      move_var = checkMoveForDeposit(deck, wanted_card, current_deck,
                                     wanted_deck);
    }
    else
    {
      move_var = checkForValidMove(deck, wanted_card, current_deck,
                                   wanted_deck);
    }
    if ((move_var != -2) && (move_var != -1))
    {
      return mainPrintFunction(deck);
    }
  }
  return 1;
}

//-----------------------------------------------------------------------------
///
/// Prints a table with first two rows same always, but under these two rows
/// cards are changing and printing every time a successful command was ran.
/// For loop is going through all decks and printing cards in one row.
///
/// @param deck array of pointers to the first card in every deck
///
/// @return 1 if printing was successful
/// @return 2 if out of memory
//
int mainPrintFunction(Card** deck)
{
  int i;
  int oom;
  Card** column;
  column = (Card**)malloc(7*sizeof(Card*));
  if (column == NULL)
  {
	  free(column);
	  printf("[ERR] Out of memory\n");
	  return 2;
  }  
  for (i = 0; i < 7; i++)
  {
    column[i] = deck[i];
  }
  printf("0   | 1   | 2   | 3   | 4   | DEP | DEP\n");
  printf("---------------------------------------\n");
  oom = printLines(column);
  if (oom == 2)
  {
	  free(column);
    return 2;	
  }
  free(column);
  return 1;
}


//-----------------------------------------------------------------------------
///
/// Prints the cards in one row separated by horizontal lines.
/// These are printed in a way so that there are same number of slots
/// of double-linked list from head pointers.
///
/// @param column array of pointers of cards in a same row
///
/// @return 0 if printing was successful
/// @return 2 if out of memory
//
int printLines(Card** column)
{
  int i;
  int j;
  int oom;
  for (i = 0; i < 16; i++)
  {
    oom = printFunctionForAbhabeStapel(column[0]);
    if (oom == 2)
    {
      return 2;
    }
	  for (j = 1; j < 7; j++)
	  {
	    printf(" | ");
	    oom = printCardFromValue(column[j]);
	    if (oom == 2)
      {
        return 2;
      }
	  }
	  printf("\n");
	  for (j = 0; j < 7; j++)
	  {
	    column[j] = getNextCard(column[j]);
	  }
  }
  return 0;
}







//-----------------------------------------------------------------------------
///
/// Simply gets the next card in a double-linked list
///
/// @param current_card used to point to a card
///
/// @return current_card->next_ pointer to the next card
/// @return NULL if card is at the bottom of the double-linked list
///
Card* getNextCard(Card* current_card)
{
  if (current_card == NULL)
  {
    return NULL;
  }
  else
  {
    return current_card->next_;
  }
}






//-----------------------------------------------------------------------------
///
/// Prints "X  " if card is not at the bottom of DLL
/// Prints card if it is at the bottom
/// Prints "   " if there is a slot under the bottom card of DLL
///
/// @param ptr used to point at a card
///
/// @return 0 if printing was successful
/// @return 2 if out of memory
//
int printFunctionForAbhabeStapel(Card *ptr)
{
  int oom;
  char* output;
  output = (char*)malloc(4*sizeof(char));
  if (output == NULL)
  {
  	free(output);
	  printf("[ERR] Out of memory\n");
	  return 2;
  }  
  if (ptr == NULL)
  {
    output[0] = ' ';
    output[1] = ' ';
    output[2] = ' ';
    output[3] = '\0';
    printf("%s", output);
  }
  else if (ptr->next_ != NULL)
  {
    output[0] = 'X';
    output[1] = ' ';
    output[2] = ' ';
    output[3] = '\0';
    printf("%s", output);
  }
  else
  {
    oom = printCardFromValue(ptr);
  }
  if (oom == 2)
	  return 2;
  free(output);
  return 0;
}






//-----------------------------------------------------------------------------
///
/// Prints card if slot is populated by a card. Prints "   " if slot is
/// empty.
///
/// @param ptr used to point at a card
///
/// @return 0 if printing was successful
/// @return 2 if out of memory
//
int printCardFromValue(Card* ptr)
{
  char* output;
  output = (char*)malloc(4*sizeof(char));
  if (output == NULL)
  {
	  free(output);
	  printf("[ERR] Out of memory\n");
	  return 2;
  }  
  if (ptr == NULL)
  {
    output[0] = ' ';
    output[1] = ' ';
    output[2] = ' ';
    output[3] = '\0';
  }
  else
  {
    output[0] = ptr->color_;
    output[2] = ' ';
    switch (ptr->value_)
    {
      case 1:
        output[1] = 'A';
        break;
      case 2:
        output[1] = '2';
        break;
      case 3:
        output[1] = '3';
        break;
      case 4:
        output[1] = '4';
        break;
      case 5:
        output[1] = '5';
        break;
      case 6:
        output[1] = '6';
        break;
      case 7:
        output[1] = '7';
        break;
      case 8:
        output[1] = '8';
        break;
      case 9:
        output[1] = '9';
        break;
      case 10:
        output[1] = '1';
        output[2] = '0';
        break;
      case 11:
        output[1] = 'J';
        break;
      case 12:
        output[1] = 'Q';
        break;
      case 13:
        output[1] = 'K';
        break;
    }
    output[3] = '\0';
  }
  printf("%s", output);
  free(output);
  return 0;
}









//-----------------------------------------------------------------------------
///
/// Returns the number of the deck that the wanted card is in.
///
/// @param deck array of pointers to the first card in every deck
/// @param wanted_card pointer to the wanted card
///
/// @return number of the deck the card is in
//
int travelToTheTop(Card** deck, Card *wanted_card)
{
  if (wanted_card->prev_ == NULL)
  {
    if (deck[0] == wanted_card)
    {
      return 0;
    }
    else if (deck[6] == wanted_card)
    {
      return 6;
    }
    else if (deck[5] == wanted_card)
    {
      return 5;
    }
    else if (deck[4] == wanted_card)
    {
      return 4;
    }
    else if (deck[3] == wanted_card)
    {
      return 3;
    }
    else if (deck[2] == wanted_card)
    {
      return 2;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return travelToTheTop(deck, wanted_card->prev_);
  }
}






//-----------------------------------------------------------------------------
///
/// Checks if inputted command is valid for the current game state.
///
/// @param deck array of pointers to the first card in every deck
/// @param wanted_card pointer to the wanted card
/// @param current_deck the deck that the card is in
/// @param desired_deck the deck that we want to move the card to
///
/// @return 0 if move is permitted
/// @return -2 if the move is invalid
//
int checkForValidMove(Card** deck,Card* wanted_card,
                        int current_deck, int desired_deck)
{
  Card *ptr_to_btm;
  ptr_to_btm = travelToTheBottom(deck[desired_deck]);
  if (checkCardsBelow(*wanted_card) == -1)
  {
    printf("[INFO] Invalid move command!\n");
    return -2;
  }
  if (ptr_to_btm == NULL)
  {
    if (wanted_card->value_ == 13)
    {
      deck[desired_deck] = wanted_card;
      if (wanted_card->prev_ == NULL)
      {
        deck[current_deck] = NULL;
      }
      else
      {
        wanted_card->prev_->next_ = NULL;
      }
      wanted_card->prev_ = NULL;
    }
    else
    {
      printf("[INFO] Invalid move command!\n");
      return -2;
    }
  }
  else if (ptr_to_btm->color_ != wanted_card->color_)
  {
    if (ptr_to_btm->value_ == wanted_card->value_ + 1)
    {
      if (wanted_card->prev_ == NULL)
      {
        deck[current_deck] = NULL;
      }
      else
      {
        wanted_card->prev_->next_ = NULL;
      }
      wanted_card->prev_ = ptr_to_btm;
      ptr_to_btm->next_ = wanted_card;
    }
    else
    {
      printf("[INFO] Invalid move command!\n");
      return -2;
    }
  }
  else
  {
    printf("[INFO] Invalid move command!\n");
    return -2;
  }
  return 0;
}




//-----------------------------------------------------------------------------
///
/// Checks move command for deposit decks because of the special rules
/// applied to these two decks.
///
/// @param deck array of pointers to the first card in every deck
/// @param wanted_card pointer to the wanted card
/// @param current_deck the deck that the card is in
/// @param desired_deck the deck that we want to move the card to
///
/// @return 0 if move is permitted
/// @return -2 if the move is invalid
//
int checkMoveForDeposit(Card** deck, Card* wanted_card, int current_deck,
                         int desired_deck)
{
  Card* ptr_to_btm;
  ptr_to_btm = travelToTheBottom(deck[desired_deck]);
  if (wanted_card->next_ != NULL)
  {
    printf("[INFO] Invalid move command!\n");
    return -2;
  }
  else if (ptr_to_btm == NULL)
  {
    if (wanted_card->value_ == 1)
    {
      if (wanted_card->prev_ == NULL)
      {
        deck[current_deck] = NULL;
      }
      else
      {
        wanted_card->prev_->next_ = NULL;
      }
      wanted_card->prev_ = ptr_to_btm;
      deck[desired_deck] = wanted_card;
    }
    else
    {
      printf("[INFO] Invalid move command!\n");
      return -2;
    }
  }
  else
  {
    if (wanted_card->color_ == ptr_to_btm->color_)
    {
      if (wanted_card->value_ == ptr_to_btm->value_ + 1)
      {
        ptr_to_btm->next_ = wanted_card;
        if (wanted_card->prev_ == NULL)
        {
          deck[current_deck] = NULL;
        }
        else
        {
          wanted_card->prev_->next_ = NULL;
        }
        wanted_card->prev_ = ptr_to_btm;
      }
      else
      {
        printf("[INFO] Invalid move command!\n");
        return -2;
      }
    }
    else
    {
      printf("[INFO] Invalid move command!\n");
      return -2;
    }
  }
  return 0;
}









//-----------------------------------------------------------------------------
///
/// Return the address of the card described by the user input. Three digit
/// passed to the function, the last two digits are the description of the
/// card, if bigger than 13, then card is black and value is for 13 less than
/// the last two digits, otherwise two digits are the value and the color is
/// red.
///
/// @param move_var the number containing description of the wanted card
/// @param card_instance array of cards
///
/// @return &card_instance[i]
//
Card* findCardFromMoveVar(int move_var, Card* card_instance)
{
  int card_value;
  char card_color;
  card_value = move_var % 100;
  if (card_value > 13)
  {
    card_color = 'B';
    card_value -= 13;
  }
  else
  {
    card_color = 'R';
  }
  int i;
  for (i = 0; i < 26; i++)
  {
    if (card_instance[i].value_ == card_value)
    {
      if (card_instance[i].color_ == card_color)
      {
        break;
      }
    }
  }
  return &card_instance[i];
}




//-----------------------------------------------------------------------------
///
/// Function that goes to the bottom of the linked list and than it returns
/// address of the card at the bottom.
///
/// @param card_instance pointer to card for which deck we want to check for
///
/// @return card_instance if it is at the bottom
/// @return NULL if deck is completely empty
//
Card* travelToTheBottom(Card* card_instance)
{
  if (card_instance == NULL)
  {
    return NULL;
  }
  else if ((*card_instance).next_ != NULL)
  {
    return travelToTheBottom((*card_instance).next_);
  }
  else
  {
    return card_instance;
  }
}




//-----------------------------------------------------------------------------
///
/// Checks if the card we want to move can be moved with all the other cards
/// below it.
///
/// @param card_instance pointer to card for which deck we want to check for
///
/// @return 0 if moving all cards below is permitted
/// @return -1 if moving all cards below is not permitted
//
int checkCardsBelow(Card card_instance)
{
  if (card_instance.next_ != NULL)
  {
    if ((*(card_instance.next_)).color_ != card_instance.color_)
    {
      if ((*(card_instance.next_)).value_ == card_instance.value_ - 1)
      {
        return checkCardsBelow(*(card_instance.next_));
      }
      else
      {
        return -1;
      }
    }
    else
    {
      return -1;
    }
  }
  else
  {
    return 0;
  }
}




//-----------------------------------------------------------------------------
///
/// At the beginning of every command prints "esp>" and awaits for the
/// following command to be executed by the user input.
///
///
/// @return move_var the number containing description of the wanted card
/// @return 2 out of memory
/// @return 0 if command is exit or EOF
/// @return -1 if command is help
/// @return -2 if command is move
//
int checkUserInput()
{
  printf("esp> ");
  char *read_line;
  read_line = (char *) malloc(100 * sizeof(char));
  if (read_line == NULL)
  {
    free(read_line);
    printf("[ERR] Out of memory\n");
    return 2;
  }
  int i;
  int color_var;
  int value;
  int move_var;
  int length_counter = 100;
  fgets(read_line, 100, stdin);
  while(1)
  {
    if (feof(stdin) != 0)
      return 0;
    if (read_line[strlen(read_line) - 1] != '\n')
    {
      length_counter *= 2;
      read_line = (char *) realloc(read_line, length_counter * sizeof(char));
      if (read_line == NULL)
      {
        printf("[ERR] Out of memory\n");
        free(read_line);
        return 2;
      }
      fgets(&(read_line[strlen(read_line)]), 100, stdin);
    }
    else
    {
      break;
    }
  }
  read_line[strlen(read_line) - 1] = ' ';
  for (i = 0; i < strlen(read_line); i++)
  {
    read_line[i] = toupper(read_line[i]);
  }
  char** tokens;
  tokens = (char**)malloc(7*sizeof(char*));
  if (tokens == NULL)
  {
	  free(tokens);
	  free(read_line);
	  printf("[ERR] Out of memory\n");
	  return 2;
  }  
  for (i =0; i < 7; i++)
  {
    tokens[i] = (char*)malloc(20*sizeof(char));
    if (tokens[i] == NULL)
    {
	    free(tokens);
	    free(read_line);
	    printf("[ERR] Out of memory\n");
	    return 2;
    }  	
  }
  tokens[0] = strtok(read_line, " ");
  for (i = 1; i < 5; i++)
  {
    tokens[i] = strtok(NULL, " ");
    if (tokens[i] == NULL)
    {
      break;
    }
  }
  if ((strcmp(tokens[0], "HELP") == 0) && (tokens[1] == NULL))
  {
    return -1;
  }
  else if ((strcmp(tokens[0], "EXIT") == 0) && (tokens[1] == NULL))
  {
    return 0;
  }
  if (strcmp(tokens[0], "MOVE") != 0)
  {
    return -2;
  }
  if ((strcmp(tokens[1], "BLACK") != 0) && (strcmp(tokens[1], "RED") != 0))
  {
    return -2;
  }
  if (strcmp(tokens[1], "BLACK") == 0)
  {
    color_var = 13;
  }
  else
  {
    color_var = 0;
  }
  value = checkCardValue(tokens[2]);
  if (value == -1)
  {
    return -2;
  }
  value += color_var;
  if (strcmp(tokens[3], "TO") != 0)
  {
    return -2;
  }
  move_var = checkDeckNumber(tokens[4]);
  if (move_var == -1)
  {
    return -2;
  }
  else
  {
    move_var = (move_var * 100) + value;
  }
  free(tokens);
  free(read_line);
  return move_var;
}




//-----------------------------------------------------------------------------
///
/// Function that checks what deck number the user wants a card moved to, then
/// returns the number of that deck
///
/// @param tok token containing the number we want, represented as a string
///
/// @return -1 if token is invalid
/// @return number of deck that we want to move to
//
int checkDeckNumber(char* tok)
{
  if (tok[0] == '0')
  {
    if ((tok[1] == 10) || (tok[1] == 0) || (tok[1] == 13))
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '1')
  {
    if ((tok[1] == 10) || (tok[1] == 0) || (tok[1] == 13))
    {
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '2')
  {
    if ((tok[1] == 10) || (tok[1] == 0) || (tok[1] == 13))
    {
      return 2;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '3')
  {
    if ((tok[1] == 10) || (tok[1] == 0) || (tok[1] == 13))
    {
      return 3;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '4')
  {
    if ((tok[1] == 10) || (tok[1] == 0) || (tok[1] == 13))
    {
      return 4;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '5')
  {
    if ((tok[1] == 10) || (tok[1] == 0) || (tok[1] == 13))
    {
      return 5;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '6')
  {
    if ((tok[1] == 10) || (tok[1] == 0) || (tok[1] == 13))
    {
      return 6;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    return -1;
  }
}



//-----------------------------------------------------------------------------
///
/// Setting up the decks.
/// This function is for pointing all the cards to the decks.
/// The cards are set up as described in Palme.
///
/// @param deck array of pointers to the first card in every deck
/// @param card_instance pointer to card for which deck we want to check for
///
//
void setFirstPointers(Card** deck, Card* card_instance)
{
  deck[0] = &card_instance[0];
  
  deck[1] = &card_instance[25];
  card_instance[25].prev_ = NULL;
  card_instance[25].next_ = NULL;
  
  deck[2] = &card_instance[24];
  card_instance[24].prev_ = NULL;
  card_instance[24].next_ = &card_instance[21];
  card_instance[21].prev_ = &card_instance[24];
  card_instance[21].next_ = NULL;
  
  deck[3] = &card_instance[23];
  card_instance[23].prev_ = NULL;
  card_instance[23].next_ = &card_instance[20];
  card_instance[20].prev_ = &card_instance[23];
  card_instance[20].next_ = &card_instance[18];
  card_instance[18].prev_ = &card_instance[20];
  card_instance[18].next_ = NULL;
  
  deck[4] = &card_instance[22];
  card_instance[22].prev_ = NULL;
  card_instance[22].next_ = &card_instance[19];
  card_instance[19].prev_ = &card_instance[22];
  card_instance[19].next_ = &card_instance[17];
  card_instance[17].prev_ = &card_instance[19];
  card_instance[17].next_ = &card_instance[16];
  card_instance[16].prev_ = &card_instance[17];
  card_instance[16].next_ = NULL;
  
  deck[5] = NULL;
  deck[6] = NULL;
  
  int i;

  for (i = 0; i < 16; i++)
  {
    card_instance[i].prev_ = &card_instance[i - 1];
    card_instance[i].next_ = &card_instance[i + 1];
  }
  card_instance[15].next_ = NULL;
  card_instance[0].prev_ = NULL;
}



//-----------------------------------------------------------------------------
///
/// Function is used for checking the file validity and lines validity
///
/// @param config_file is a pointer to the file used with the program
/// @param card_instance pointer to card for which deck we want to check for
///
/// @return 0 if file is valid;
/// @return 2 if out of memory;
/// @return 3 if file invalid or not found
//
int entireInputFromFile(FILE *config_file, Card *card_instance)
{
  if (config_file == NULL)
  {
    return 3;
  }
  int i;
  int length_counter = 100;
  char **line;
  line = (char**)malloc(26 * sizeof(char*));
  if (line == NULL)
  {
	  free(line);
	  printf("[ERR] Out of memory\n");
	  return 2;
  }
  for (i = 0; i < 26; i++)
  {
    line[i] = (char*)malloc(length_counter * sizeof(char));
	  if (line == NULL)
    {
	    free(line);
	    printf("[ERR] Out of memory\n");
	    return 2;
    }
  }
  for (i = 0; i < 26; i++)
  {
    fgets(line[i], 100, config_file);
    while (1)
    {
      if (feof(config_file) != 0)
        break;
      if (line[i][strlen(line[i]) - 1] != '\n')
      {
        length_counter *= 2;
        line[i] = (char*)realloc(line[i], length_counter * sizeof(char));
        if (line[i] == NULL)
        {
          printf("[ERR] Out of memory\n");
          free(line);
          return 2;
        }
        fgets(&(line[i][strlen(line[i])]), 100, config_file);
      }
      else
      {
        break;
      }
    }
    if (checkForEmptyLine(line[i]) == 0)
    {
      i -= 1;
    }
  }
  char *token;
  char *tok_2;
  char *tok_3;
  tok_3 = NULL;
  int *value;
  char *color;
  value = (int*)malloc(sizeof(int));
  if (value == NULL)
  {
	  free(value);
	  free(line);
	  printf("[ERR] Out of memory\n");
	  return 2;
  }
  color = (char*)malloc(sizeof(char));
  if (value == NULL)
  {
	  free(color);
	  free(value);
	  free(line);
	  printf("[ERR] Out of memory\n");
	  return 2;
  }  
  for (i = 0; i < 26; i++)
  {
    token = strtok(line[i], " ");
    tok_2 = strtok(NULL, " ");
    tok_3 = strtok(NULL, " ");
    if (tok_3 != NULL)
    {
      if (checkForEmptyLine(tok_3) == 1)
      {
        return 3;
      }
    }
    *color = checkCardColor(token);
    if (*color == 'E')
    {
      return 3;
    }
    card_instance[i].color_ = *color;
    *value = checkCardValue(tok_2);
    if (*value == -1)
    {
      return 3;
    }
    card_instance[i].value_ = *value;
    if (checkForSameCard(card_instance, i) == -1)
    {
      return 3;
    }
  }
  free(line);
  free(color);
  free(value);
  return 0;
}




//-----------------------------------------------------------------------------
///
/// Depending on the user input, this function checks for the card color
///
///
/// @param tok used as a token (string part) in string to check for deck number
///
/// @return 'R' for red
/// @return 'B' for black
/// @return 'E' for non existing color of the card
//
char checkCardColor(char* tok)
{
  if (strcmp(tok, "RED") == 0)
  {
    return 'R';
  }
  else if (strcmp(tok, "BLACK") == 0)
  {
    return 'B';
  }
  else
  {
    return 'E';
  }
}


//-----------------------------------------------------------------------------
///
/// This function checks for the same card when loaded from the file.
///
/// @param cards all pointers from struct Card
/// @param i is a counter
///
/// @return 0
/// @return -1
//
int checkForSameCard(Card *cards, int i)
{
  int current;
  current = i;
  for (; i > 0;)
  {
    i--;
    if (cards[current].value_ == cards[i].value_)
    {
      if (cards[current].color_ == cards[i].color_)
      {
        return -1;
      }
    }
  }
  return 0;
}



//-----------------------------------------------------------------------------
///
/// Skips all the empty lines in the file.
///
/// @param line pointer to the line in file input
///
/// @return 1 successful
/// @return 0 not successful
//
int checkForEmptyLine(char *line)
{
  int i;
  for (i = 0; i < strlen(line); i += 1)
  {
    if ((line[i] != ' ') && (line[i] != '\n') && (line[i] != '\0') &&
        (line[i] != '\t') && (line[i] != '\r') && (line[i] != 13))
    {
      return 1;
    }
  }
  return 0;
}



//-----------------------------------------------------------------------------
///
/// Checks for values of the cards in the deck and returns their values.
///
///
/// @param tok used as a token (string part) in string to check for deck number
///
/// @return -1 if wrong
/// @return required number as the value of the card
//
int checkCardValue(char *tok)
{
  if (tok == NULL)
  {
    return -1;
  }
  if (tok[0] == '1')// this checks for 10
  {
    if (tok[1] == '0')
    {
      if ((tok[2] == 10) || (tok[2] == 0) ||  (tok[2] == 13) ||
            (tok[2] == EOF))
      {
        return 10;
      }
      else
      {
        return -1;
      }
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '2')// this checks for 2
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 2;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '3')// this checks for 3
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 3;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '4')// this checks for 4
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 4;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '5')// this checks for 5
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 5;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '6')// this checks for 6
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 6;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '7')// this checks for 7
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 7;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '8')// this checks for 8
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 8;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == '9')// this checks for 9
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 9;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == 'A')// this checks for Ace
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == 'J')// this checks for Jack
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 11;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == 'Q')// this checks for Queen
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 12;
    }
    else
    {
      return -1;
    }
  }
  else if (tok[0] == 'K')// this checks for King
  {
    if ((tok[1] == 10) || (tok[1] == 0) ||  (tok[1] == 13) || (tok[1] == EOF))
    {
      return 13;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    return -1;
  }
}

