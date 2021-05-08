
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
#Run on Server side 
VSOMEIP_CONFIGURATION=../service.json ./service_UB  
#Run on Client side
VSOMEIP_CONFIGURATION=../client.json ./client_UB
```
