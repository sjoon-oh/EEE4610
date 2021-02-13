echo "iree cloning"
cd ~
git clone https://github.com/google/iree
wait

apt-get update
wait

apt-get upgrade
wait

apt-get install -y cmake clang ninja-build
wait

git submodule update --init
