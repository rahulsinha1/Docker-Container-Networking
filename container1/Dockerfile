FROM ubuntu:latest

ENV sample default_env_value
RUN apt-get update
RUN apt-get install -y g++

ADD udpprocess.cpp /app/
ADD hostfile /app/
WORKDIR /app/
RUN g++ -pthread udpprocess.cpp -o udpprocess

ENTRYPOINT /app/udpprocess sample
