FROM ubuntu:16.04

RUN mkdir /code
WORKDIR /code
COPY . /code

EXPOSE 80
EXPOSE 12345

ENTRYPOINT [ "./main" ]
