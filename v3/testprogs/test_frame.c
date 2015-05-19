void fnVoid(int a) {
	a = 1;
}

int fnInt(int a, int b) {
	a = 2;
}

int main() {
	
	fnVoid(3);
	fnInt(4,5);
	fnVoid(6);

	return 0;
}
