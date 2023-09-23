FROM ubuntu:23.10

COPY ./build/rpc/main /tmp

EXPOSE 3368

RUN chmod +x /tmp/main

ENTRYPOINT [ "/tmp/main" ]