

# can run multiple times
# can run any dir
# copy addons and export class
# class **Q_DECL_EXPORT** Dummy : public Qskxxx

shdir=$(dirname $(realpath $0))

files="iotdashboard/Skin iotdashboard/TopBar"

for f in $files ; do
    hf="$shdir/../examples/$f.h"
    cf="$shdir/../examples/$f.cpp"

    hf2="$shdir/$(basename $hf)"
    cf2="$shdir/$(basename $cf)"
    echo "Process $f.h ..."
    sed -i -E 's/class ([^:]+) : public Qsk/class Q_DECL_EXPORT \1 : public Qsk/g' "$hf" #
    # > "$hf2"
    # cp $cf $cf2
done
