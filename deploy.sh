#!/bin/bash

# Pull the repository

sudo -u user git pull

# stop / rm the old running docker
echo -e $1 | sudo -S docker stop solver_ctn
echo -e $1 | sudo -S docker rm solver_ctn

# Generate the new docker image
echo -e $1 | sudo -S docker build --network=host . -t solver_img

# Run the docker
echo -e $1 | sudo -S docker run -dp 50003:50003 --name solver_ctn solver_img
