/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
  {
    readChar();
  }
}

void skipComment() {
  while (1)
  {
    readChar();
    if (currentChar == EOF)
    {
      error(ERR_ENDOFCOMMENT, lineNo, colNo);
    }
    else if (charCodes[currentChar] == CHAR_TIMES)
    {
      readChar();
      if (currentChar == EOF)
      {
        error(ERR_ENDOFCOMMENT, lineNo, colNo);
      }
      else if (charCodes[currentChar] == CHAR_RPAR)
      {
        readChar();
        return;
      }
    }
  }
}

Token* readIdentKeyword(void) {
  // TODO
  int TKLen = 0;
  Token *token = makeToken(TK_IDENT, lineNo, colNo);

  while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)
  {
    token->string[TKLen] = currentChar;
    TKLen++;
    readChar();
  }

  token->string[TKLen] = '\0';

  if (TKLen > MAX_IDENT_LEN)
  {
    error(ERR_IDENTTOOLONG, lineNo, colNo - TKLen);
  }
  else
  {
    TokenType type = checkKeyword(token->string);

    if (type != TK_NONE)
    {
      token->tokenType = type;
    }
  }
  return token;
}

Token* readNumber(void) {
  readChar();
  int TKLen = 0;
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);

  while (charCodes[currentChar] == CHAR_DIGIT)
  {
    token->string[TKLen] = currentChar;
    TKLen++;
    readChar();
  }

  token->string[TKLen] = '\0';

  token->value = atoi(token->string);
  return token;
}

Token* readConstChar(void) {
  // TODO
  Token *token = makeToken(TK_CHAR, lineNo, colNo);

  readChar();

  if (currentChar == EOF)
  {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
  }
  else
  {
    switch (charCodes[currentChar])
    {
    case CHAR_SINGLEQUOTE:
      readChar();

      if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
      {
        token->string[0] = currentChar;

        readChar();
        if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
        {
          token->string[1] = '\0';
          return token;
        }
        else
        {
          error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
        }
      }
      else
      {
        error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
      }
      break;
    default:

      token->string[0] = currentChar;

      readChar();
      if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
      {
        token->string[1] = '\0';
        readChar();
        return token;
      }
      else
      {
        error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
        break;
      }
      break;
    }
  }

  return token;
}

Token* getToken(void) {
  Token *token;
  //int ln, cn;

  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;


  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;

  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;

  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;

  case CHAR_LT:
    token = makeToken(SB_LT, lineNo, colNo);
    readChar();

    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_LE;
      readChar();
      return token;
    }
    return token;

  case CHAR_GT:
    token = makeToken(SB_GT, lineNo, colNo);

    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_GE;
      readChar();
      return token;
    }
    return token;

  case CHAR_EXCLAIMATION:
    token = makeToken(TK_NONE, lineNo, colNo);
    readChar();

    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_NEQ;
      readChar();
      return token;
    }
    else
    {
      error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
      readChar();
      return token;
    }

  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;

  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;

  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD, lineNo, colNo);
    readChar();

    if (charCodes[currentChar] == CHAR_RPAR)
    {
      token->tokenType = SB_RSEL;
      readChar();
    }
    return token;

  case CHAR_COLON:
    token = makeToken(SB_COLON, lineNo, colNo);
    readChar();

    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_ASSIGN;
      readChar();
    }
    return token;

  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;

  case CHAR_SINGLEQUOTE:
    return readConstChar();

  case CHAR_LPAR:
    token = makeToken(SB_LPAR, lineNo, colNo);
    readChar();

    if (charCodes[currentChar] == CHAR_PERIOD)
    {
      token->tokenType = SB_LSEL;
      readChar();
      return token;
    }
    else if (charCodes[currentChar] == CHAR_TIMES)
    {
      free(token);
      skipComment();
      return getToken();
    }
    else
    {
      return token;
    }

  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;

  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/


void printToken(Token *token, FILE *outputFile) {
  fprintf(outputFile, "%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: fprintf(outputFile, "TK_NONE\n"); break;
  case TK_IDENT: fprintf(outputFile, "TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: fprintf(outputFile, "TK_NUMBER(%d)\n", token->value); break;
  case TK_CHAR: fprintf(outputFile, "TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: fprintf(outputFile, "TK_EOF\n"); break;

  case KW_PROGRAM: fprintf(outputFile, "KW_PROGRAM\n"); break;
  case KW_CONST: fprintf(outputFile, "KW_CONST\n"); break;
  case KW_TYPE: fprintf(outputFile, "KW_TYPE\n"); break;
  case KW_VAR: fprintf(outputFile, "KW_VAR\n"); break;
  case KW_INTEGER: fprintf(outputFile, "KW_INTEGER\n"); break;
  case KW_CHAR: fprintf(outputFile, "KW_CHAR\n"); break;
  case KW_ARRAY: fprintf(outputFile, "KW_ARRAY\n"); break;
  case KW_OF: fprintf(outputFile, "KW_OF\n"); break;
  case KW_FUNCTION: fprintf(outputFile, "KW_FUNCTION\n"); break;
  case KW_PROCEDURE: fprintf(outputFile, "KW_PROCEDURE\n"); break;
  case KW_BEGIN: fprintf(outputFile, "KW_BEGIN\n"); break;
  case KW_END: fprintf(outputFile, "KW_END\n"); break;
  case KW_CALL: fprintf(outputFile, "KW_CALL\n"); break;
  case KW_IF: fprintf(outputFile, "KW_IF\n"); break;
  case KW_THEN: fprintf(outputFile, "KW_THEN\n"); break;
  case KW_ELSE: fprintf(outputFile, "KW_ELSE\n"); break;
  case KW_WHILE: fprintf(outputFile, "KW_WHILE\n"); break;
  case KW_DO: fprintf(outputFile, "KW_DO\n"); break;
  case KW_FOR: fprintf(outputFile, "KW_FOR\n"); break;
  case KW_TO: fprintf(outputFile, "KW_TO\n"); break;

  case SB_SEMICOLON: fprintf(outputFile, "SB_SEMICOLON\n"); break;
  case SB_COLON: fprintf(outputFile, "SB_COLON\n"); break;
  case SB_PERIOD: fprintf(outputFile, "SB_PERIOD\n"); break;
  case SB_COMMA: fprintf(outputFile, "SB_COMMA\n"); break;
  case SB_ASSIGN: fprintf(outputFile, "SB_ASSIGN\n"); break;
  case SB_EQ: fprintf(outputFile, "SB_EQ\n"); break;
  case SB_NEQ: fprintf(outputFile, "SB_NEQ\n"); break;
  case SB_LT: fprintf(outputFile, "SB_LT\n"); break;
  case SB_LE: fprintf(outputFile, "SB_LE\n"); break;
  case SB_GT: fprintf(outputFile, "SB_GT\n"); break;
  case SB_GE: fprintf(outputFile, "SB_GE\n"); break;
  case SB_PLUS: fprintf(outputFile, "SB_PLUS\n"); break;
  case SB_MINUS: fprintf(outputFile, "SB_MINUS\n"); break;
  case SB_TIMES: fprintf(outputFile, "SB_TIMES\n"); break;
  case SB_SLASH: fprintf(outputFile, "SB_SLASH\n"); break;
  case SB_LPAR: fprintf(outputFile, "SB_LPAR\n"); break;
  case SB_RPAR: fprintf(outputFile, "SB_RPAR\n"); break;
  case SB_LSEL: fprintf(outputFile, "SB_LSEL\n"); break;
  case SB_RSEL: fprintf(outputFile, "SB_RSEL\n"); break;
  }
}

int scan(char *fileName, FILE *outputFile) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token, outputFile);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

int compareFiles(FILE *outputFile, FILE *expectedFile) {
  char outputLine[256], expectedLine[256];
  int line = 1;

  rewind(outputFile);
  rewind(expectedFile);

  while (fgets(outputLine, sizeof(outputLine), outputFile) &&
         fgets(expectedLine, sizeof(expectedLine), expectedFile)) {
    if (strcmp(outputLine, expectedLine) != 0) {
      printf("Difference at line %d:\n", line);
      printf("Output   : %s", outputLine);
      printf("Expected : %s", expectedLine);
      return -1;
    }
    line++;
  }

  if (!feof(outputFile) || !feof(expectedFile)) {
    printf("File sizes do not match.\n");
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("scanner: insufficient arguments. Provide input file and comparison file.\n");
    return -1;
  }

  FILE *tempFile = tmpfile();
  if (!tempFile) {
    printf("Failed to create temporary file.\n");
    return -1;
  }

  if (scan(argv[1], tempFile) == IO_ERROR) {
    printf("Can't read input file!\n");
    fclose(tempFile);
    return -1;
  }

  FILE *expectedFile = fopen(argv[2], "r");
  if (!expectedFile) {
    printf("Can't open expected output file!\n");
    fclose(tempFile);
    return -1;
  }

  int comparisonResult = compareFiles(tempFile, expectedFile);

  fclose(expectedFile);
  fclose(tempFile);

  if (comparisonResult == 0) {
    printf("Output matches expected output.\n");
  } else {
    printf("Output does not match expected output.\n");
  }

  return comparisonResult;
}