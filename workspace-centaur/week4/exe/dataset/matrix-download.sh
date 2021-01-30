# Sample Sparse Matrix Download
echo "Samples downloading from Google Drive..."

if [ -e s32.tar.gz ]; then
	echo "s32.tar.gz exists. Skipping download."
else
	wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=1rQXmeihAXmUHADrvAYUa5lQOocUtXgTr' -O s32.tar.gz
fi
wait
if [ -e s64.tar.gz ]; then
	echo "s64.tar.gz exists. Skipping download."
else
	wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=1n4DdLSRp06ruU2CULM73GcKxbVZ9P3bZ' -O s64.tar.gz
fi
wait
if [ -e s128.tar.gz ]; then
	echo "s128.tar.gz exists. Skipping download."
else
	wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=162KCAISBPkFTZG0_nJbPMbefrAb0QH8Q' -O s128.tar.gz
fi
wait
if [ -e s256.tar.gz ]; then
	echo "s256.tar.gz exists.Skipping download."
else
	wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=18_HAlSN7UFZuJiQEywHygdAlNhCPMvZG' -O s256.tar.gz
fi
wait
if [ -e s512.tar.gz ]; then
	echo "s512.tar.gz exists.Skipping download."
else
	wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=1IC2fyFspFe7H8FAOjUrwKhkUkB1B6lq7' -O s512.tar.gz
fi
#wait
#if [ -e s1024.tar.gz ]; then
#	echo "s1024.tar.gz exists.Skipping download."
#else
#	wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=1jDnYJ-yn8JS-uOmDiw-PSlKBKxBY0xhI' -O s1024.tar.gz
#fi
wait
if [ -e s1024.tar.gz ]; then
        echo "s1024.tar.gz exists.Skipping download."
else
	wget --load-cookies ./cookies.txt "https://docs.google.com/uc?export=download&confirm=$(wget --quiet --save-cookies ./cookies.txt --keep-session-cookies --no-check-certificate 'https://docs.google.com/uc?export=download&id=1jDnYJ-yn8JS-uOmDiw-PSlKBKxBY0xhI' -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')&id=1jDnYJ-yn8JS-uOmDiw-PSlKBKxBY0xhI" -O s1024.tar.gz && rm -rf ./cookies.txt
fi
#if [-e "s2048.tar.gz"]; then
#	echo "s2048.tar.gz exists.Skipping download."
#else
#	wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=17xlhj9JNu-3Eu6EhH_ED13bo3M2GzeqU' -O s2048.tar.gz
#fi
if [ -e s2048.tar.gz ]; then
        echo "s2048.tar.gz exists.Skipping download."
else
	wget --load-cookies ./cookies.txt "https://docs.google.com/uc?export=download&confirm=$(wget --quiet --save-cookies ./cookies.txt --keep-session-cookies --no-check-certificate 'https://docs.google.com/uc?export=download&id=17xlhj9JNu-3Eu6EhH_ED13bo3M2GzeqU' -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')&id=17xlhj9JNu-3Eu6EhH_ED13bo3M2GzeqU" -O s2048.tar.gz && rm -rf ./cookies.txt
fi
wait
if [ -e s4096.tar.gz ]; then
	echo "s4096.tar.gz exists.Skipping download."
else
	wget --load-cookies ./cookies.txt "https://docs.google.com/uc?export=download&confirm=$(wget --quiet --save-cookies ./cookies.txt --keep-session-cookies --no-check-certificate 'https://docs.google.com/uc?export=download&id=1-jeeb6J1_ZSwqB4FjvoEyXX_ohI6Alhd' -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')&id=1-jeeb6J1_ZSwqB4FjvoEyXX_ohI6Alhd" -O s4096.tar.gz && rm -rf ./cookies.txt
fi
wait
if [ -e s8192.tar.gz ]; then
	echo "s8192.tar.gz exists.Skipping download."
else
	wget --load-cookies ./cookies.txt "https://docs.google.com/uc?export=download&confirm=$(wget --quiet --save-cookies ./cookies.txt --keep-session-cookies --no-check-certificate 'https://docs.google.com/uc?export=download&id=18cIOP9Bvp3NnWq0vZJwlYcmqT9VyMVdE' -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')&id=18cIOP9Bvp3NnWq0vZJwlYcmqT9VyMVdE" -O s8192.tar.gz && rm -rf ./cookies.txt
fi
wait
