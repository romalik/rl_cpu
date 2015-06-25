/*
 * bsearch.c
 */

#include <stdlib.h>

int *bsearch(const int *key, const int *base, size_t nmemb,
	      size_t size, int (*cmp) (const int *, const int *))
{
	while (nmemb) {
		size_t mididx = nmemb / 2;
		const int *midobj = base + mididx * size;
		int diff = cmp(key, midobj);

		if (diff == 0)
			return (int *)midobj;

		if (diff > 0) {
			base = midobj + size;
			nmemb -= mididx + 1;
		} else
			nmemb = mididx;
	}

	return NULL;
}
