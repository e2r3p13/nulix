#include <stddef.h>

static int isof(int c, const char *charset) {
	while (*charset) {
		if (c == *charset)
			return 1;
		charset++;
	}
	return 0;
}

size_t	strlen(const char *s) {
	size_t i = 0;

	while (*s++) {
		i++;
	}
	return i;
}

char *strchr(const char *s, int c) {
	while (*s && *s != c)
		s++;
	return *s == c ? (char *)s : NULL;	
}

char *strrchr(const char *s, int c) {
	size_t len = strlen(s);

	s += len;
	while (len-- && *s != c)
		s--;
	return *s == c ? (char *)s : NULL;
}

int strcmp(const char *s1, const char *s2) {
	while (*s1 && *s2 && *s1 == *s2) {
		s1++, s2++;
	}
	return s1 - s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n-- && *s1 && *s2 && *s1 == *s2) {
		s1++, s2++;
	}
	return n == 0 ? 0 : *s1 - *s2;
}

char *strcpy(char *dst, const char *src) {
	char *ptr = dst;

	while (*src) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return ptr;
}

char *strncpy(char *dst, const char *src, size_t n) {
	char *ptr = dst;

	while (n--) {
		*dst++ = *src++;
	}
	return ptr;
}

size_t strspn(const char *s, const char *accept) {
	size_t count = 0;

	while (*s && isof(*s, accept)) {
		s++, count++;
	}
	return count;
}

size_t strcspn(const char *s, const char *reject) {
	size_t count = 0;

	while (*s && !isof(*s, reject)) {
		s++, count++;
	}
	return count;
}

char *strpbrk(const char *s, const char *accept) {
	while (*s && !isof(*s, accept)) {
		s++;
	}
	return *s ? (char *)s : NULL;
}

char *strstr(const char *haystack, const char *needle) {
	const char *tmph, *tmpn;

	while (*haystack) {
		tmph = haystack;
		tmpn = needle;
		while (*tmph  && *tmpn && *tmph == *tmpn) {
			tmph++, tmpn++;
		}
		if (*tmpn == '\0')
			break;
		haystack++;
	}
	return (char *)haystack;
}

char *strtok(char *str, const char *delim) {
	static char *ptr, *tmp;

	if (str)
		ptr = str;

	while (*ptr && isof(*ptr, delim))
		ptr++;
	tmp = ptr;
	while (*tmp && !isof(*tmp, delim))
		tmp++;
	*tmp = '\0';
	return ptr;
}

char *strcat(char *dst, const char *src) {
	char *ptr = dst;

	while (*dst++);
	strcpy(dst, src);
	return ptr;
}

char *strncat(char *dst, const char * src, size_t n) {
	char *ptr = dst;

	while (*dst++);
	strncpy(dst, src, n);
	*(dst + n) = '\0';
	return ptr;
}
