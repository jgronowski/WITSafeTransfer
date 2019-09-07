#TCP OPENSSL connection
Server and client secure tcp file transfer using openssl libraries

NOTE: Before you run the code generate an openssl certificate by executing this command:
```
cd ./server/certificate && openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem
```

#### run & compile server
```
gcc -o server ./server/main.c -lcrypto -lssl
cd server && ./server <port>
```
if server throws error `Private key does not match certificate` try to write full path to `mycert.pem` in server/tcp_openssl/tcp_server.c 

#### run & compile client
```
gcc -o client ./client/main.c -lcrypto -lssl
cd client && ./client <port>
```
