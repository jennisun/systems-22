#include "pipe_networking.h"

int server_setup() {
  int val = mkfifo(WKP, 0777);
  if (val < 0) {
    remove(WKP);
    mkfifo(WKP, 0777);
  }
  int from_client = open(WKP, O_RDONLY, 0777);
  remove(WKP);
  return from_client;
}

int server_connect(int from_client) {
  char buffer[HANDSHAKE_BUFFER_SIZE];

  int val = read(from_client, buffer, HANDSHAKE_BUFFER_SIZE);
  if (val < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }
  int to_client = open(buffer, O_WRONLY, 0777);
  write(to_client, ACK, HANDSHAKE_BUFFER_SIZE);

  val = read(from_client, buffer, HANDSHAKE_BUFFER_SIZE);
  if (val < 0) {
    printf("here Error: %s\n", strerror(errno));
    return 0;
  }
  if (strcmp(buffer, ACK) != 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }
  printf("Server handshake successful\n");
  return to_client;
}

/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  printf("Server creates WKP..\n");
  int val = mkfifo(WKP, 0644);
  if (val < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }

  printf("Server opening and reading secret pipe..\n");
  int server = open(WKP, O_RDONLY);
  if (server < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }
  char client[HANDSHAKE_BUFFER_SIZE];
  val = read(server, client, HANDSHAKE_BUFFER_SIZE);
  if (val < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }

  printf("Server removing WKP..\n");
  remove(WKP);

  printf("Server sending message to client..\n");
  *to_client = open(client, O_WRONLY);
  if (*to_client < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }
  val = write(*to_client, ACK, sizeof(ACK));
  if (val < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }

  printf("Server receives verification message from client..\n");
  val = read(server, client, HANDSHAKE_BUFFER_SIZE);
  if (val < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }
  if (strcmp(client, ACK) != 0) {
    printf("Error occured\n");
    return 0;
  }
  else {
    printf("Server success\n");
    return server;
  }
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  printf("Client creating secret pipe..\n");
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%d", getpid() );
  int val = mkfifo(buffer, 0644);
  if (val < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }

  printf("Client sending message to server..\n");
  *to_server = open(WKP, O_WRONLY);
  if (*to_server < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }
  val = write(*to_server, buffer, HANDSHAKE_BUFFER_SIZE);
  if (val < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }

  printf("Client receives response, removes secret pipe..\n");
  int server = open(buffer, O_RDONLY);
  if (server < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }
  val = read(server, buffer, HANDSHAKE_BUFFER_SIZE);
  if (val < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }
  if (strcmp(buffer, ACK) != 0) {
    printf("Error occured\n");
    return 0;
  }

  printf("Client sending final verification message..\n");

  val = write(*to_server, ACK, HANDSHAKE_BUFFER_SIZE);
  if (val < 0) {
    printf("Error: %s\n", strerror(errno));
    return 0;
  }

  printf("Client success\n");
  return server;
}
