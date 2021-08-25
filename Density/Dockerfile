FROM kienkauko/arm32v7ubuntu:gnu
COPY qemu-arm-static /usr/bin
COPY . /home
WORKDIR /home
RUN make
EXPOSE 9002 9001
