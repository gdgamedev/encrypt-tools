cmd_Release/obj.target/encrypt.node := g++ -shared -pthread -rdynamic -m64  -Wl,-soname=encrypt.node -o Release/obj.target/encrypt.node -Wl,--start-group Release/obj.target/encrypt/code.o -Wl,--end-group 
