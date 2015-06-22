int memcpy(unsigned int * dest, unsigned int * src, int n) {
	int k = n;
	while(k--) {
		*dest = *src;
		dest++;
		src++;
	}
	return n;
}


int memcpy_r(unsigned int * dest, unsigned int * src, int n) {
	int k = n;
	while(k--) {
		*dest = *src;
		dest++;
		src++;
	}
	return n;
}

