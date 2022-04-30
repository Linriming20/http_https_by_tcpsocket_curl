### 1、下载

[https://curl.se/download.html](https://curl.se/download.html)



### 2、编译

```bash
tar xjf curl-7.82.0.tar.bz2
cd curl-7.82.0/
./configure --prefix=$PWD/_install --without-ssl # --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc
make -j96
make install
```

