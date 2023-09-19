FROM ubuntu:latest

COPY . .

RUN mv -f ./mirror-sources.list /etc/apt/sources.list 

RUN apt update

RUN apt install cmake g++ -y
RUN apt install libgrpc++-dev -y

CMD [ "/bin/sh", "run.sh" ]