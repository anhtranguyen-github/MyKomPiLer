
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reader.h"

FILE *inputStream;
int lineNo, colNo;
int currentChar;

#define INITIAL_BUFFER_SIZE 1024

// Dynamic buffer to store the read characters
char *readBuffer = NULL;
size_t bufferSize = INITIAL_BUFFER_SIZE;
size_t bufferIndex = 0;

int readChar(void) {
  currentChar = getc(inputStream);
  colNo++;
  printf("%c", currentChar);
  if (currentChar == '\n') {
    lineNo++;
    colNo = 0;
  }

  // Save the current character to the buffer
  if (bufferIndex >= bufferSize - 1) {
    bufferSize *= 2; // Expand buffer if needed
    readBuffer = realloc(readBuffer, bufferSize);
    if (readBuffer == NULL) {
      fprintf(stderr, "Memory allocation failed\n");
      exit(EXIT_FAILURE);
    }
  }

  if (currentChar != EOF) {
    readBuffer[bufferIndex++] = (char)currentChar;
    readBuffer[bufferIndex] = '\0'; // Null-terminate the string
  }

  return currentChar;
}

int openInputStream(char *fileName) {
  inputStream = fopen(fileName, "rt");
  if (inputStream == NULL)
    return IO_ERROR;

  // Initialize line and column numbers
  lineNo = 1;
  colNo = 0;

  // Initialize the buffer
  readBuffer = (char *)malloc(INITIAL_BUFFER_SIZE);
  if (readBuffer == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return IO_ERROR;
  }
  readBuffer[0] = '\0'; // Start with an empty string

  readChar();
  return IO_SUCCESS;
}

void closeInputStream() {
  fclose(inputStream);

  // Free the dynamically allocated buffer
  if (readBuffer != NULL) {
    free(readBuffer);
    readBuffer = NULL;
  }
}
