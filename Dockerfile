
FROM ubuntu:20.04

EXPOSE 50003

RUN apt-get update

COPY . /app
WORKDIR /app


#######################################################
# Pistache installation for C++ HTTP request handling #
#######################################################

# need software-properties-common for add-apt-repository
RUN apt update
RUN apt-get upgrade -y
RUN apt-get install -y software-properties-common --fix-missing
RUN add-apt-repository ppa:pistache+team/unstable
RUN apt update
RUN apt install -y libpistache-dev


############################
# Build of the application #
############################

RUN apt install -y g++
RUN g++ -std=c++17 service.cpp Island.cpp HashiGrid.cpp HashiState.cpp -lpistache -lstdc++fs -o solver

############################
# Run application on start #
############################

CMD "./solver"
