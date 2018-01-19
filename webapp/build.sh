
if [[ ! ${1+x} ]]; then
    echo "No ROM file specified"
    echo "Usage: ./build.sh <path/to/rom/file>"
    exit 0
fi

cp $1 assets/ROM
source /usr/local/emsdk/emsdk_env.sh --build=RELEASE
make clean
make
npm run build
rm assets/ROM
