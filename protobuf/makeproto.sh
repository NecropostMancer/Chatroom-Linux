SRC_DIR="./"
DST_DIR="./out"
rm -rf $DST_DIR
mkdir $DST_DIR
protoc -I=$SRC_DIR --cpp_out=$DST_DIR ./*.proto
