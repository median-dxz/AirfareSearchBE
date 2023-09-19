# AirfareSearchBE
AirfareSearch Backend Components

# 模块说明

- rpc: rpc服务器模块，对外提供服务的模块，入口点
- search-serivce: 主服务模块，提供搜索逻辑，多线程竞争rpc传入的请求
- database: 数据库相关接口代码和业务代码，作为中间层运行，为主服务提供数据库无关抽象
- data-updater: 数据更新服务，数据更新相关逻辑
- data-generator: 模拟数据生成

模块之间的依赖关系如下：

- 在`rpc`中起一个服务器，监听客户端请求和数据更新请求，本身作为一个`Controller`。
  - 如果请求来自客户端: 将请求转发给`search-service`，竞争线程处理请求，将结果返回给客户端。
  - 如果请求来自数据更新服务: 将请求转发给`data-updater`，将结果返回给数据更新服务。

- `search-service` 和 `data-updater` 都会调用 `database` 进行数据库内容的持久化，同时`data-updator`通过`data-generator`生成模拟数据。