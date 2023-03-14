echo Installing dependencies...

lib_dir="./lib"

if [ ! -d "./lib" ]; then
    mkdir -p "lib"
fi

cd "./lib"

if [ ! -d "./alef" ]; then
    git clone https://github.com/wingmann/alef.git
else
    echo alef was already installed
fi

if [ ! -d "./concepts" ]; then
    git clone https://github.com/wingmann/concepts.git
else
    echo concepts was already installed
fi


if [ ! -d "./cast" ]; then
    git clone https://github.com/wingmann/cast.git
else
    echo cast was already installed
fi

if [ ! -d "./biginteger" ]; then
    git clone https://github.com/wingmann/biginteger.git
else
    echo biginteger was already installed
fi