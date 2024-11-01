docker run --rm --privileged -e TZ=Asia/Tokyo -v $PWD:/project -w /project -u $UID -e HOME=/tmp -it --name esp32_container espressif/idf
