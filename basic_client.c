#include "pipe_networking.h"


int main() {

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  char input[BUFFER_SIZE];
  while(1) {
    printf("Enter input: ");
    fgets(input, BUFFER_SIZE, stdin);
    input[strlen(input) - 1] = '\0';
    int val = write(to_server, input, sizeof(input));
    if (val < 0) {
      printf("Error: %s\n", strerror(errno));
      return 0;
    }
    val = read(from_server, input, sizeof(input));
    printf("Output: %s\n", input);
  }
}
