
#Enable multicast in both hosts:
sudo route add -nv <multicast_address> dev <interface>

example: 
sudo route add -nv 224.224.224.245 dev eth0


#To run:
mkdir build
cd build 
cmake ..
make 
VSOMEIP_CONFIGURATION=../service.json ./service_UB
#or 
VSOMEIP_CONFIGURATION=../client.json ./client_UB