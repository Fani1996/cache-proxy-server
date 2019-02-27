FROM ubuntu:16.04

WORKDIR /code

EXPOSE 12345

ENTRYPOINT [ "./main" ]
