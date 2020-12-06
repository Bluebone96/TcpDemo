cd build 
cmake ..
make

cd bin
// 启动顺序有依赖性
./db_server
./game_server
./gate_server
./login_server


