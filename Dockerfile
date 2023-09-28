# 生成随机数据库数据
FROM node:20.7.0-alpine3.18 AS data-gen

RUN mkdir -p /usr/local/airfare-search/build

RUN corepack enable
RUN corepack prepare pnpm@latest --activate

WORKDIR /usr/local/airfare-search/data-generator

COPY data-generator .
RUN pnpm install

RUN pnpm start

# 运行时
FROM ubuntu:latest

WORKDIR /usr/local/airfare-search

COPY --from=data-gen \
    /usr/local/airfare-search/build/ \
    ./build/

RUN apt-get update \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends sqlite3

COPY ./build/rpc .
COPY ./scripts ./scripts

RUN sh ./scripts/create_table.sh

RUN groupadd shs && useradd -m -g shs shs
RUN chown --recursive shs:shs .
RUN chown --recursive shs:shs /var/as_data
USER shs

ENTRYPOINT [ "/usr/local/airfare-search/main" ]