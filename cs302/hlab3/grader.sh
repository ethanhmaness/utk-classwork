n=`wc randinput.txt | awk '{ print $1 }'`
i=1
while [ $i -le $n ]; do
  a=`sed -n "$i"p randinput.txt | awk '{ print $1 }'`
  p=`sed -n "$i"p randinput.txt | awk '{ print $2 }'`
  ok=`sed -n "$i"p randinput.txt | awk '{ print $3 }'`
  rv=`echo $a $p | ./a.out -`
  if [ "A$rv" = A ]; then
    if [ $ok = no ]; then
      echo $i ok
    else
      echo $i -- you gave no answer, but there is one
    fi
  else
    j=`echo $a $p $rv | ./confirm`
    if [ "A$j" = AGood ]; then
      echo $i ok
    else
      echo $i -- $j
    fi
  fi
  i=$(($i+1))
done
