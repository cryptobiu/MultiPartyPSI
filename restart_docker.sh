sudo service docker stop
sudo pkill -f docker
sudo service docker start

docker rmi -f scapicryptobiu/multipartypsi
docker build --no-cache -t scapicryptobiu/multipartypsi:latest .