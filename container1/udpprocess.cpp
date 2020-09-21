#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <netdb.h>
#include <vector>
#include <algorithm>

using namespace std;
//int ports[20];
string container_ids[20];
char *message = "default";
int *arrSize;
std::ifstream infile("hostfile");

void *udpserver(void *vargp)
{
    int count = 0;
    int sockfd;
    vector<string> connected;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv, client;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8080);
    struct hostent *server;
    server = gethostbyname(message);
    bcopy((char *)server->h_addr,
          (char *)&serv.sin_addr.s_addr,
          server->h_length);
    bind(sockfd, (struct sockaddr *)&serv, sizeof(serv));
    char buffer[256];
    socklen_t l = sizeof(client);
    int total = *arrSize;
    int rc;
    string hostname;
    while (true)
    {
        rc = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &l);
        hostname = inet_ntoa(client.sin_addr);
        if (rc < 0)
        {
            cout << "ERROR READING FROM SOCKET";
        }
        else
        {
            if (!(std::count(connected.begin(), connected.end(), buffer)))
            {
                connected.push_back(buffer);
                cout << "\n ** Received from :: " << buffer << " with ip **" << hostname << endl;
                count++;
            }
        }

        if (count == total)
        {
            break;
        }
    }
    cout << "\n***********READY**********" << endl;
}

void *udpclient(void *vargp)
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv, client;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8080);
    char buffer[256];
    socklen_t l = sizeof(client);
    socklen_t m = sizeof(serv);
    struct hostent *server;
    int count = *arrSize;
    while (true)
    {
        for (int i = 0; i < count; i++)
        {
            server = gethostbyname(container_ids[i].c_str());
            if (server != 0)
            {
                bcopy((char *)server->h_addr,
                      (char *)&serv.sin_addr.s_addr,
                      server->h_length);
            }
            if (sendto(sockfd, message, strlen(message) + 1, 0, (struct sockaddr *)&serv, m) < 0)
            {
                perror("send failed");
                continue;
            }
        }
    }
}

int getHosts(char file_name[], string container_ids[])
{
    int size = 0;

    std::string line;
    while (std::getline(infile, line))
    {
        // std::istringstream iss(line);
        // if (!(iss >> container_ids[size] >> ports[size])) { break; } // error
        container_ids[size] = line;
        size++;
    }
    return size;
}

int main(int argc, char *argv[])
{
    char filename[] = "hostfile";
    if (argc >= 2)
    {
        message = argv[1];
    }
    arrSize = new int;
    *arrSize = getHosts(filename, container_ids);
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, udpserver, NULL);
    pthread_t client_thread;
    pthread_create(&client_thread, NULL, udpclient, NULL);
    pthread_join(server_thread, NULL);
    pthread_join(client_thread, NULL);
    exit(0);
}