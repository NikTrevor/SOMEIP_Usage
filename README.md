
Some example usages of SOME/IP communication using VSOMEIP.

This could be run on the same host or between 2 different hosts.

**To run on the same host:**
```
mkdir build    
cd build 
cmake ..  
make 
./service  
./client
```

**To run on 2 different hosts:**

Enable multicast in both hosts:  
```sudo route add -nv <multicast_address> dev <interface>  ```

Example:   
```sudo route add -nv 224.224.224.245 dev eth0```

Build and run:  
```
mkdir build    
cd build 
cmake ..  
make
//Run on Server side (may need sudo)
VSOMEIP_CONFIGURATION=../service.json ./service  
//Run on Client side (may need sudo)
VSOMEIP_CONFIGURATION=../client.json ./client
```

Wireshark: 
![image](https://user-images.githubusercontent.com/8686228/117564844-5d3cdd00-b0ae-11eb-9907-23baafb6c349.png)
