
if [[ ! ${1+x} ]]; then
    echo "No ROM file specified"
    echo "Usage: ./build.sh <path/to/rom/file>"
    exit 0
fi

source /usr/local/emsdk/emsdk_env.sh --build=RELEASE
make ROM=$1
npm run build
