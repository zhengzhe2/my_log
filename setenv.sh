CURRENT_DIR=$(cd $(dirname $0); pwd)

sudo cp ${CURRENT_DIR}/build/Proxy/libmy_log_proxy.so /usr/lib/
sudo cp ${CURRENT_DIR}/build/ProxyWrapper/libmy_log_proxy_wrapper.so /usr/lib/

cd ${CURRENT_DIR}/build/testApp/
./my_log_test_app
