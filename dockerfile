FROM ubuntu
EXPOSE 8086
WORKDIR /usr/app
COPY ./bin/Debug .
COPY ./docker-script .
ENV LD_LIBRARY_PATH=".:$LD_LIBRARY_PATH"
RUN apt-get update && \
    apt-get -y install openssl libssl-dev iproute2
ENTRYPOINT ["./chatting-room"]

