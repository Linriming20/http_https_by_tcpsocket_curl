### 1、下载

**mbedTLS**：[https://github.com/Mbed-TLS/mbedtls/tags](https://github.com/Mbed-TLS/mbedtls/tags)

**cURL**：[https://curl.se/download.html](https://curl.se/download.html)



### 2、编译

**mbedTLS**：

```bash
tar xzf mbedtls-3.0.0.tar.gz
cd mbedtls-3.0.0/
make CC=gcc CFLAGS=-fPIC -j96
make DESTDIR=$PWD/_install install
```

**cURL**：

```bash
tar xjf curl-7.82.0.tar.bz2
cd curl-7.82.0/
#./configure --prefix=$PWD/_install --without-ssl # --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc
./configure --prefix=$PWD/_install --with-mbedtls=$PWD/../mbedtls-3.0.0/_install --enable-optimize --disable-debug --disable-curldebug --disable-symbol-hiding --disable-dict --disable-gopher --disable-imap --disable-pop3 --disable-rtsp --disable-smtp --disable-telnet --disable-sspi --disable-smb --disable-ntlm-wb --disable-tls-srp --disable-soname-bump --disable-manual --disable-file --disable-ldap --disable-tftp --enable-http --disable-ftp --disable-mqtt --disable-ipv6 # --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc
make -j96
make install
```

