#!/bin/bash

url='https://gitee.com/LightSwitch/data_mining_project/raw/master/%E6%A0%B7%E4%BE%8B%E6%95%B0%E6%8D%AE%E9%9B%86/%E6%B8%B8%E6%88%8F%E7%B1%BB/pokemon/pokemon_alopez247.csv'

filename=`basename $url`

sha256sum -c "data/$filename.sig"
if [ $? -ne 0 ]; then
	wget $url -O data/$filename
fi
