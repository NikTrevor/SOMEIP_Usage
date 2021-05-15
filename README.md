
Enable multicast in both hosts:  
```sudo route add -nv <multicast_address> dev <interface>  ```

Example:   
```sudo route add -nv 224.224.224.245 dev eth0```


To run:  
```
mkdir build    
cd build 
cmake ..  
make
//Run on Server side 
VSOMEIP_CONFIGURATION=../service.json ./service  
//Run on Client side
VSOMEIP_CONFIGURATION=../client.json ./client
```

Wireshark: 
![image](https://user-images.githubusercontent.com/8686228/117564844-5d3cdd00-b0ae-11eb-9907-23baafb6c349.png)
