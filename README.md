# tls-server-client-c-example
This is example uses openssl library.
All the steps are coded in the makefile. 
Please lookin the makefile for the commands to install missing packages 
such as openssl and libssl. The netstat utility is also a good networking debug tool. 

make all -n
g++ server.c -o server -lssl -lcrypto
g++ client.c -o client -lssl -lcrypto
openssl version -a
openssl genrsa -out server.key 2048 
openssl rsa -in server.key -pubout -out public.key
openssl req -new -subj "/C=US/ST=CA/L=SD/O=OpenSource/CN=localhost/emailAddress=suhaspai@yahoo.com" -key server.key -out server.csr
openssl req -text -in server.csr -noout -verify
openssl x509 -in server.csr -out server.crt -req -signkey server.key -days 365
server &
client &

The initial server/client C code was obtained by prompting an AI chat-bot. The steps for generating
public/private keys and certificate were searched online and by referring to youtube videos.

