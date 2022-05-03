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