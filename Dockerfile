
FROM ubuntu:14.04

#Install dependencies
RUN apt-get update && apt-get install -y make unrar-free autoconf automake libtool \
    gcc g++ gperf flex bison texinfo gawk ncurses-dev libexpat-dev python-dev \
    python python-serial sed git unzip bash help2man wget bzip2 libtool git

#Crosstool-ng can't be run as root, so we'll be using a normal user instead
RUN adduser --disabled-password --gecos '' usr

USER usr

ADD ./test /home/usr/test

#Clone and build toolchain (build files are removed to avoid having a multi GB image)
RUN cd && git clone --recursive https://github.com/pfalcon/esp-open-sdk.git && cd esp-open-sdk \
      && make STANDALONE=y && mv xtensa-lx106-elf ~ && cd && rm -rf esp-open-sdk

WORKDIR /home/usr

#Run bash by default, with the toolchain added to PATH
CMD PATH=$PATH:/home/usr/xtensa-lx106-elf/bin; cd test; make