### 1、mbedTLS安装使用

- **下载：**

[https://github.com/Mbed-TLS/mbedtls/tags](https://github.com/Mbed-TLS/mbedtls/tags)

- **编译：**

```bash
tar xzf mbedtls-3.0.0.tar.gz
cd mbedtls-3.0.0/
make CC=gcc CFLAGS=-fPIC -j96
make DESTDIR=$PWD/_install install
```

- **示例：**

https示例参考`mbedtls-3.0.0/programs/ssl/ssl_client1.c`，其他示例参考`mbedtls-3.0.0/programs/`子目录下的其他程序。

## 2、demo文件说明

- `include`：ubuntu16.04 x86_64环境下编译得到的mbedTLS头文件；
- `lib`：ubuntu16.04 x86_64环境下编译得到的mbedTLS静态库文件；
- `GET/ssl_client1.c`：拷贝自``mbedtls-3.0.0/programs/ssl/ssl_client1.c`;
- `GET/sample.c`：基于`GET/ssl_client1.c`的自定义修改为GET请求示例；
- `GET/sample2_download.c`：基于`GET/sample.c`的自定义修改为通过GET方式下载文件示例；
- `POST/sample.c`：基于`GET/sample.c`的自定义修改为POST请求示例；

