# FileTransfer
version2.0 tcp control

version3.0 tcp multithread transfer

文件传输不完整，file-transfer.c中
int size = (1073741824 / 1024 + 1) / THREAD_NUM + 1; 有点问题
//file.c 1g文件生成
//fileclient.c 客户端
//fileserver.c 服务端
//file-stream.c 传输前建立连接函数
//file-transfer.c 传输文件函数
