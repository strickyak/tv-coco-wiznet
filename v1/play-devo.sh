
make undevo
./undevo < ~/Videos/lemma/Devo---Whip-It--Official-Music-Video----Warner-Vault-j-QLzthSkfM.webm.lemma

(
	cat ,tv-cmd
	cat ,audio-00000

	while true
	do
		i=1
		while [ $i -lt 834 ]
		do
			cat $(printf ,audio-%05d $i)
			cat $(printf ,video-%05d $i)
			i=$(expr 1 + $i)
		done
	done
) | netcat -l 2319
