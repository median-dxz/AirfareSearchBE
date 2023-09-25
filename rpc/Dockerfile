FROM ubuntu:23.10

COPY ./build/main /tmp

EXPOSE 3368

RUN chmod +x /tmp/main

ENTRYPOINT [ "/tmp/main" ]