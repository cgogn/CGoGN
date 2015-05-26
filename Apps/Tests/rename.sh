name=`basename $1 .h`
dir=`dirname $1`
mv $dir/$name.h $dir/$name.cpp
