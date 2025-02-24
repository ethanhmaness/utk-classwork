hp=1.0
max_hp=0
if [ $(echo "$hp > $max_hp" | bc) == 1 ]; then
    echo "success"
fi