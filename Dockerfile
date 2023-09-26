# 生成随机数据库数据
FROM node:20.7.0-alpine3.18 AS data-gen

WORKDIR /usr/local/airfare-search

RUN mkdir -p /usr/local/airfare-search/build

COPY data-generator ./data-generator

WORKDIR /usr/local/airfare-search/data-generator

RUN corepack enable
RUN corepack prepare pnpm@latest --activate
RUN pnpm install

CMD pnpm start

# 运行时
FROM alpine:latest

WORKDIR /usr/local/airfare-search

COPY ./build/rpc .

COPY --from=data-gen \
    /usr/local/airfare-search/build/ \
    ./build/import_data.sql

RUN apk update && \
    apk add --no-cache \
    libstdc++ sqlite3

RUN sh ./scripts/init_database.sh

RUN addgroup -S shs && adduser -S shs -G shs
USER shs

RUN chown --recursive shs:shs .

ENTRYPOINT [ "/usr/local/airfare-search/main" ]