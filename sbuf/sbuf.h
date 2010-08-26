#ifndef LIBS_SBUF
#define LIBS_SBUF

#include <stdarg.h>

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
    #define GNUC_PRINTF_CHECK(fmt_idx, arg_idx) __attribute__((format (printf, fmt_idx, arg_idx)))
#else
    #define GNUC_PRINTF_CHECK(fmt_idx, arg_idx)
#endif

#define sbuf_raw(SB) ((SB)->NUL ? (SB)->buf : NULL)
#define sbuf_len(SB) ((SB)->NUL)

/* shorthand to get a char * from an struct sbuf */
#define _S(SB) sbuf_raw((SB))

void sbuf_init(struct sbuf *sb, size_t reserve);
void sbuf_cleanup(struct sbuf *sb);

void sbuf_clear(struct sbuf *sb);
char *sbuf_detach(struct sbuf *sb);
void sbuf_move(struct sbuf *src, struct sbuf *dest);
void sbuf_dup(struct sbuf *src, struct sbuf *dest);

void sbuf_putc(struct sbuf *sb, const char c);

void sbuf_cat(struct sbuf *sb, const char *str);
void sbuf_ncat(struct sbuf *sb, const char *str, size_t len);
void sbuf_bcat(struct sbuf *sb, const struct sbuf *buf);
void sbuf_scatf(struct sbuf *sb, const char *fmt, ...) GNUC_PRINTF_CHECK(2,3);

int sbuf_cmp(struct sbuf *sb, const char *str);
int sbuf_ncmp(struct sbuf *sb, const char *str, size_t len);
int sbuf_bcmp(struct sbuf *sb, const struct sbuf *buf);

#endif
