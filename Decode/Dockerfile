FROM kienkauko/arm32v7ubuntu:1.0
COPY qemu-arm-static /usr/bin
COPY . /home
WORKDIR /home
RUN make
#CMD ["./run.sh"]
EXPOSE 9000 9001

