// server.c
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
 

#define PORT     8080
#define CERTFILE "server.crt"
#define KEYFILE  "server.key"
 

int main() {
   int                 sockfd, clientfd;
   struct sockaddr_in  server_addr, client_addr;
   socklen_t           client_len;
   SSL_CTX            *ctx;
   SSL                *ssl;
 

   // Initialize OpenSSL
   SSL_library_init();
   OpenSSL_add_all_algorithms();
   SSL_load_error_strings();
 

   // Create SSL context
   ctx = SSL_CTX_new(TLS_server_method());
   if (!ctx) {
      perror("SSL_CTX_new");
      exit(1);
   }
 

   // Load certificate and private key
   if (SSL_CTX_use_certificate_file(ctx, CERTFILE, SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(1);
   }
   if (SSL_CTX_use_PrivateKey_file(ctx, KEYFILE, SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(1);
   }
 

   // Create socket
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
      perror("socket");
      exit(1);
   }
 

   // Bind socket
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family      = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;
   server_addr.sin_port        = htons(PORT);
   if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
      perror("bind");
      exit(1);
   }
 

   // Listen for connections
   if (listen(sockfd, 5) < 0) {
      perror("listen");
      exit(1);
   }
 

   printf("Server listening on port %d...\n", PORT);
 

   // Accept connection
   client_len = sizeof(client_addr);
   clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
   if (clientfd < 0) {
      perror("accept");
      exit(1);
   }
 

   // Create SSL connection
   ssl = SSL_new(ctx);
   SSL_set_fd(ssl, clientfd);
 

   // Perform SSL handshake
   if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(1);
   }
 

   // Receive and send data
   char buffer[1024];
   int bytes_read = SSL_read(ssl, buffer, sizeof(buffer) - 1);
   if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      printf("Received: %s\n", buffer);
      SSL_write(ssl, "Hello from server!", strlen("Hello from server!"));
   } else {
      ERR_print_errors_fp(stderr);
   }
 

   // Close connection
   SSL_free(ssl);
   close(clientfd);
   close(sockfd);
   SSL_CTX_free(ctx);
 

   return 0;
}
