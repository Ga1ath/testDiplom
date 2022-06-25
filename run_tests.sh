test_amount=29
median=15
low_quartile=8
high_quartile=22

sum=0
data=()

for i in {1..29} ; do
    output="$(/home/me/VIII/Diplom/cmake-build-debug/Diplom)"
    data+=("$output")
    sum=$(echo "$sum" + "$output" | bc)
#    echo "$sum"
done

average=$(echo "$sum" / "$test_amount" | bc -l)
echo "$average"

# shellcheck disable=SC2207
sorted=( $( printf "%s\n" "${data[@]}" | sort -n ) )
#printf "[%s]\n" "${sorted[@]}"

echo "${sorted["$low_quartile"]}"
echo "${sorted["$median"]}"
echo "${sorted["$high_quartile"]}"
