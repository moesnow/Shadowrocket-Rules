Shadowrocket-Rules : build.c
	gcc build.c -o build -lcurl

clean :
	rm build rule.conf
