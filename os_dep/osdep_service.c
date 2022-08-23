/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _OSDEP_SERVICE_C_

#include <drv_types.h>

#ifdef CONFIG_HWSIM
#include "rtw_hwsim_intf.h"
inline int _rtw_netif_rx(_nic_hdl ndev, struct sk_buff *skb)
{
	skb->dev = ndev;
	rtw_hwsim_medium_pre_netif_rx(skb);
	return netif_rx(skb);
}
#endif /* CONFIG_HWSIM */

u32 rtw_atoi(u8 *s)
{

	int num = 0, flag = 0;
	int i;
	for (i = 0; i <= strlen(s); i++) {
		if (s[i] >= '0' && s[i] <= '9')
			num = num * 10 + s[i] - '0';
		else if (s[0] == '-' && i == 0)
			flag = 1;
		else
			break;
	}

	if (flag == 1)
		num = num * -1;

	return num;

}

void *rtw_malloc2d(int h, int w, size_t size)
{
	int j;

	void **a = (void **) rtw_zmalloc(h * sizeof(void *) + h * w * size);
	if (a == NULL) {
		RTW_INFO("%s: alloc memory fail!\n", __FUNCTION__);
		return NULL;
	}

	for (j = 0; j < h; j++)
		a[j] = ((char *)(a + h)) + j * w * size;

	return a;
}

void rtw_mfree2d(void *pbuf, int h, int w, int size)
{
	rtw_mfree((u8 *)pbuf, h * sizeof(void *) + w * h * size);
}

int _rtw_memcmp2(const void *dst, const void *src, u32 sz)
{
	const unsigned char *p1 = dst, *p2 = src;

	if (sz == 0)
		return 0;

	while (*p1 == *p2) {
		p1++;
		p2++;
		sz--;
		if (sz == 0)
			return 0;
	}

	return *p1 - *p2;
}

void _rtw_init_queue(_queue *pqueue)
{
	_rtw_init_listhead(&(pqueue->queue));
	_rtw_spinlock_init(&(pqueue->lock));
}

void _rtw_deinit_queue(_queue *pqueue)
{
	_rtw_spinlock_free(&(pqueue->lock));
}

u32 _rtw_queue_empty(_queue	*pqueue)
{
	return rtw_is_list_empty(&(pqueue->queue));
}


u32 rtw_end_of_queue_search(_list *head, _list *plist)
{
	if (head == plist)
		return _TRUE;
	else
		return _FALSE;
}

/* the input parameter start use the same unit as returned by rtw_get_current_time */
inline s32 _rtw_get_passing_time_ms(systime start)
{
	return _rtw_systime_to_ms(_rtw_get_current_time() - start);
}

inline s32 _rtw_get_remaining_time_ms(systime end)
{
	return _rtw_systime_to_ms(end - _rtw_get_current_time());
}

inline s32 _rtw_get_time_interval_ms(systime start, systime end)
{
	return _rtw_systime_to_ms(end - start);
}

bool rtw_macaddr_is_larger(const u8 *a, const u8 *b)
{
	u32 va, vb;

	va = be32_to_cpu(*((u32 *)a));
	vb = be32_to_cpu(*((u32 *)b));
	if (va > vb)
		return 1;
	else if (va < vb)
		return 0;

	return be16_to_cpu(*((u16 *)(a + 4))) > be16_to_cpu(*((u16 *)(b + 4)));
}


/*
* Test if the specifi @param path is a readable file with valid size.
* If readable, @param sz is got
* @param path the path of the file to test
* @return _TRUE or _FALSE
*/
int rtw_readable_file_sz_chk(const char *path, u32 sz)
{
	u32 fsz;

	if (rtw_is_file_readable_with_size(path, &fsz) == _FALSE)
		return _FALSE;

	if (fsz > sz)
		return _FALSE;
	
	return _TRUE;
}

void rtw_buf_free(u8 **buf, u32 *buf_len)
{
	u32 ori_len;

	if (!buf || !buf_len)
		return;

	ori_len = *buf_len;

	if (*buf) {
		u32 tmp_buf_len = *buf_len;
		*buf_len = 0;
		rtw_mfree(*buf, tmp_buf_len);
		*buf = NULL;
	}
}

void rtw_buf_update(u8 **buf, u32 *buf_len, const u8 *src, u32 src_len)
{
	u32 ori_len = 0, dup_len = 0;
	u8 *ori = NULL;
	u8 *dup = NULL;

	if (!buf || !buf_len)
		return;

	if (!src || !src_len)
		goto keep_ori;

	/* duplicate src */
	dup = rtw_malloc(src_len);
	if (dup) {
		dup_len = src_len;
		_rtw_memcpy(dup, src, dup_len);
	}

keep_ori:
	ori = *buf;
	ori_len = *buf_len;

	/* replace buf with dup */
	*buf_len = 0;
	*buf = dup;
	*buf_len = dup_len;

	/* free ori */
	if (ori && ori_len > 0)
		rtw_mfree(ori, ori_len);
}


/**
 * rtw_cbuf_full - test if cbuf is full
 * @cbuf: pointer of struct rtw_cbuf
 *
 * Returns: _TRUE if cbuf is full
 */
inline bool rtw_cbuf_full(struct rtw_cbuf *cbuf)
{
	return (cbuf->write == cbuf->read - 1) ? _TRUE : _FALSE;
}

/**
 * rtw_cbuf_empty - test if cbuf is empty
 * @cbuf: pointer of struct rtw_cbuf
 *
 * Returns: _TRUE if cbuf is empty
 */
inline bool rtw_cbuf_empty(struct rtw_cbuf *cbuf)
{
	return (cbuf->write == cbuf->read) ? _TRUE : _FALSE;
}

/**
 * rtw_cbuf_push - push a pointer into cbuf
 * @cbuf: pointer of struct rtw_cbuf
 * @buf: pointer to push in
 *
 * Lock free operation, be careful of the use scheme
 * Returns: _TRUE push success
 */
bool rtw_cbuf_push(struct rtw_cbuf *cbuf, void *buf)
{
	if (rtw_cbuf_full(cbuf))
		return _FAIL;

	if (0)
		RTW_INFO("%s on %u\n", __func__, cbuf->write);
	cbuf->bufs[cbuf->write] = buf;
	cbuf->write = (cbuf->write + 1) % cbuf->size;

	return _SUCCESS;
}

/**
 * rtw_cbuf_pop - pop a pointer from cbuf
 * @cbuf: pointer of struct rtw_cbuf
 *
 * Lock free operation, be careful of the use scheme
 * Returns: pointer popped out
 */
void *rtw_cbuf_pop(struct rtw_cbuf *cbuf)
{
	void *buf;
	if (rtw_cbuf_empty(cbuf))
		return NULL;

	if (0)
		RTW_INFO("%s on %u\n", __func__, cbuf->read);
	buf = cbuf->bufs[cbuf->read];
	cbuf->read = (cbuf->read + 1) % cbuf->size;

	return buf;
}

/**
 * rtw_cbuf_alloc - allocte a rtw_cbuf with given size and do initialization
 * @size: size of pointer
 *
 * Returns: pointer of srtuct rtw_cbuf, NULL for allocation failure
 */
struct rtw_cbuf *rtw_cbuf_alloc(u32 size)
{
	struct rtw_cbuf *cbuf;

	cbuf = (struct rtw_cbuf *)rtw_malloc(sizeof(*cbuf) + sizeof(void *) * size);

	if (cbuf) {
		cbuf->write = cbuf->read = 0;
		cbuf->size = size;
	}

	return cbuf;
}

/**
 * rtw_cbuf_free - free the given rtw_cbuf
 * @cbuf: pointer of struct rtw_cbuf to free
 */
void rtw_cbuf_free(struct rtw_cbuf *cbuf)
{
	rtw_mfree((u8 *)cbuf, sizeof(*cbuf) + sizeof(void *) * cbuf->size);
}

/**
 * map_readN - read a range of map data
 * @map: map to read
 * @offset: start address to read
 * @len: length to read
 * @buf: pointer of buffer to store data read
 *
 * Returns: _SUCCESS or _FAIL
 */
int map_readN(const struct map_t *map, u16 offset, u16 len, u8 *buf)
{
	const struct map_seg_t *seg;
	int ret = _FAIL;
	int i;

	if (len == 0) {
		rtw_warn_on(1);
		goto exit;
	}

	if (offset + len > map->len) {
		rtw_warn_on(1);
		goto exit;
	}

	_rtw_memset(buf, map->init_value, len);

	for (i = 0; i < map->seg_num; i++) {
		u8 *c_dst, *c_src;
		u16 c_len;

		seg = map->segs + i;
		if (seg->sa + seg->len <= offset || seg->sa >= offset + len)
			continue;

		if (seg->sa >= offset) {
			c_dst = buf + (seg->sa - offset);
			c_src = seg->c;
			if (seg->sa + seg->len <= offset + len)
				c_len = seg->len;
			else
				c_len = offset + len - seg->sa;
		} else {
			c_dst = buf;
			c_src = seg->c + (offset - seg->sa);
			if (seg->sa + seg->len >= offset + len)
				c_len = len;
			else
				c_len = seg->sa + seg->len - offset;
		}
			
		_rtw_memcpy(c_dst, c_src, c_len);
	}

exit:
	return ret;
}

/**
 * map_read8 - read 1 byte of map data
 * @map: map to read
 * @offset: address to read
 *
 * Returns: value of data of specified offset. map.init_value if offset is out of range
 */
u8 map_read8(const struct map_t *map, u16 offset)
{
	const struct map_seg_t *seg;
	u8 val = map->init_value;
	int i;

	if (offset + 1 > map->len) {
		rtw_warn_on(1);
		goto exit;
	}

	for (i = 0; i < map->seg_num; i++) {
		seg = map->segs + i;
		if (seg->sa + seg->len <= offset || seg->sa >= offset + 1)
			continue;

		val = *(seg->c + offset - seg->sa);
		break;
	}

exit:
	return val;
}

int rtw_blacklist_add(_queue *blist, const u8 *addr, u32 timeout_ms)
{
	struct blacklist_ent *ent;
	_list *list, *head;
	u8 exist = _FALSE, timeout = _FALSE;

	_rtw_spinlock_bh(&blist->lock);

	head = &blist->queue;
	list = get_next(head);
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		ent = LIST_CONTAINOR(list, struct blacklist_ent, list);
		list = get_next(list);

		if (_rtw_memcmp(ent->addr, addr, ETH_ALEN) == _TRUE) {
			exist = _TRUE;
			if (rtw_time_after(rtw_get_current_time(), ent->exp_time))
				timeout = _TRUE;
			ent->exp_time = rtw_get_current_time()
				+ rtw_ms_to_systime(timeout_ms);
			break;
		}

		if (rtw_time_after(rtw_get_current_time(), ent->exp_time)) {
			rtw_list_delete(&ent->list);
			rtw_mfree(ent, sizeof(struct blacklist_ent));
		}
	}

	if (exist == _FALSE) {
		ent = rtw_malloc(sizeof(struct blacklist_ent));
		if (ent) {
			_rtw_memcpy(ent->addr, addr, ETH_ALEN);
			ent->exp_time = rtw_get_current_time()
				+ rtw_ms_to_systime(timeout_ms);
			rtw_list_insert_tail(&ent->list, head);
		}
	}

	_rtw_spinunlock_bh(&blist->lock);

	return (exist == _TRUE && timeout == _FALSE) ? RTW_ALREADY : (ent ? _SUCCESS : _FAIL);
}

int rtw_blacklist_del(_queue *blist, const u8 *addr)
{
	struct blacklist_ent *ent = NULL;
	_list *list, *head;
	u8 exist = _FALSE;

	_rtw_spinlock_bh(&blist->lock);
	head = &blist->queue;
	list = get_next(head);
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		ent = LIST_CONTAINOR(list, struct blacklist_ent, list);
		list = get_next(list);

		if (_rtw_memcmp(ent->addr, addr, ETH_ALEN) == _TRUE) {
			rtw_list_delete(&ent->list);
			rtw_mfree(ent, sizeof(struct blacklist_ent));
			exist = _TRUE;
			break;
		}

		if (rtw_time_after(rtw_get_current_time(), ent->exp_time)) {
			rtw_list_delete(&ent->list);
			rtw_mfree(ent, sizeof(struct blacklist_ent));
		}
	}

	_rtw_spinunlock_bh(&blist->lock);

	return exist == _TRUE ? _SUCCESS : RTW_ALREADY;
}

int rtw_blacklist_search(_queue *blist, const u8 *addr)
{
	struct blacklist_ent *ent = NULL;
	_list *list, *head;
	u8 exist = _FALSE;

	_rtw_spinlock_bh(&blist->lock);
	head = &blist->queue;
	list = get_next(head);
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		ent = LIST_CONTAINOR(list, struct blacklist_ent, list);
		list = get_next(list);

		if (_rtw_memcmp(ent->addr, addr, ETH_ALEN) == _TRUE) {
			if (rtw_time_after(rtw_get_current_time(), ent->exp_time)) {
				rtw_list_delete(&ent->list);
				rtw_mfree(ent, sizeof(struct blacklist_ent));
			} else
				exist = _TRUE;
			break;
		}

		if (rtw_time_after(rtw_get_current_time(), ent->exp_time)) {
			rtw_list_delete(&ent->list);
			rtw_mfree(ent, sizeof(struct blacklist_ent));
		}
	}

	_rtw_spinunlock_bh(&blist->lock);

	return exist;
}

void rtw_blacklist_flush(_queue *blist)
{
	struct blacklist_ent *ent;
	_list *list, *head;
	_list tmp;

	_rtw_init_listhead(&tmp);

	_rtw_spinlock_bh(&blist->lock);
	rtw_list_splice_init(&blist->queue, &tmp);
	_rtw_spinunlock_bh(&blist->lock);

	head = &tmp;
	list = get_next(head);
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		ent = LIST_CONTAINOR(list, struct blacklist_ent, list);
		list = get_next(list);
		rtw_list_delete(&ent->list);
		rtw_mfree(ent, sizeof(struct blacklist_ent));
	}
}

void dump_blacklist(void *sel, _queue *blist, const char *title)
{
	struct blacklist_ent *ent = NULL;
	_list *list, *head;

	_rtw_spinlock_bh(&blist->lock);
	head = &blist->queue;
	list = get_next(head);

	if (rtw_end_of_queue_search(head, list) == _FALSE) {
		if (title)
			RTW_PRINT_SEL(sel, "%s:\n", title);
	
		while (rtw_end_of_queue_search(head, list) == _FALSE) {
			ent = LIST_CONTAINOR(list, struct blacklist_ent, list);
			list = get_next(list);

			if (rtw_time_after(rtw_get_current_time(), ent->exp_time))
				RTW_PRINT_SEL(sel, MAC_FMT" expired\n", MAC_ARG(ent->addr));
			else
				RTW_PRINT_SEL(sel, MAC_FMT" %u\n", MAC_ARG(ent->addr)
					, rtw_get_remaining_time_ms(ent->exp_time));
		}

	}
	_rtw_spinunlock_bh(&blist->lock);
}

/**
* is_null -
*
* Return	TRUE if c is null character
*		FALSE otherwise.
*/
inline BOOLEAN is_null(char c)
{
	if (c == '\0')
		return _TRUE;
	else
		return _FALSE;
}

inline BOOLEAN is_all_null(char *c, int len)
{
	for (; len > 0; len--)
		if (c[len - 1] != '\0')
			return _FALSE;

	return _TRUE;
}

/**
* is_eol -
*
* Return	TRUE if c is represent for EOL (end of line)
*		FALSE otherwise.
*/
inline BOOLEAN is_eol(char c)
{
	if (c == '\r' || c == '\n')
		return _TRUE;
	else
		return _FALSE;
}

/**
* is_space -
*
* Return	TRUE if c is represent for space
*		FALSE otherwise.
*/
inline BOOLEAN is_space(char c)
{
	if (c == ' ' || c == '\t')
		return _TRUE;
	else
		return _FALSE;
}

/**
* is_decimal -
*
* Return	TRUE if chTmp is represent for decimal digit
*		FALSE otherwise.
*/
inline BOOLEAN is_decimal(char chTmp)
{
	if ((chTmp >= '0' && chTmp <= '9'))
		return _TRUE;
	else
		return _FALSE;
}

/**
* IsHexDigit -
*
* Return	TRUE if chTmp is represent for hex digit
*		FALSE otherwise.
*/
inline BOOLEAN IsHexDigit(char chTmp)
{
	if ((chTmp >= '0' && chTmp <= '9') ||
		(chTmp >= 'a' && chTmp <= 'f') ||
		(chTmp >= 'A' && chTmp <= 'F'))
		return _TRUE;
	else
		return _FALSE;
}

/**
* is_alpha -
*
* Return	TRUE if chTmp is represent for alphabet
*		FALSE otherwise.
*/
inline BOOLEAN is_alpha(char chTmp)
{
	if ((chTmp >= 'a' && chTmp <= 'z') ||
		(chTmp >= 'A' && chTmp <= 'Z'))
		return _TRUE;
	else
		return _FALSE;
}

inline char alpha_to_upper(char c)
{
	if ((c >= 'a' && c <= 'z'))
		c = 'A' + (c - 'a');
	return c;
}

int hex2num_i(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

int hex2byte_i(const char *hex)
{
	int a, b;
	a = hex2num_i(*hex++);
	if (a < 0)
		return -1;
	b = hex2num_i(*hex++);
	if (b < 0)
		return -1;
	return (a << 4) | b;
}

int hexstr2bin(const char *hex, u8 *buf, size_t len)
{
	size_t i;
	int a;
	const char *ipos = hex;
	u8 *opos = buf;

	for (i = 0; i < len; i++) {
		a = hex2byte_i(ipos);
		if (a < 0)
			return -1;
		*opos++ = a;
		ipos += 2;
	}
	return 0;
}

