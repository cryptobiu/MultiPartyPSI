sudo service docker stop
sudo pkill -f docker
sudo service docker start

sudo docker rmi -f scapicryptobiu/multipartypsi
sudo docker build --no-cache -t scapicryptobiu/multipartypsi:latest .