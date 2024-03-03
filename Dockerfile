FROM alpine
WORKDIR /app
COPY . .

RUN apk update && apk upgrade
RUN apk add gcc make libc-dev git
