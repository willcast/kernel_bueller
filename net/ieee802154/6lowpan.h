/*
 * Copyright 2011, Siemens AG
 * written by Alexander Smirnov <alex.bluesman.smirnov@gmail.com>
 */

/*
 * Based on patches from Jon Smirl <jonsmirl@gmail.com>
 * Copyright (c) 2011 Jon Smirl <jonsmirl@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/* Jon's code is based on 6lowpan implementation for Contiki which is:
 * Copyright (c) 2008, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __6LOWPAN_H__
#define __6LOWPAN_H__

#define UIP_802154_SHORTADDR_LEN	2  /* compressed ipv6 address length */
#define UIP_IPH_LEN			40 /* ipv6 fixed header size */
#define UIP_PROTO_UDP			17 /* ipv6 next header value for UDP */
#define UIP_FRAGH_LEN			8  /* ipv6 fragment header size */

/*
 * ipv6 address based on mac
 * second bit-flip (Universe/Local) is done according RFC2464
 */
#define is_addr_mac_addr_based(a, m) \
	((((a)->s6_addr[8])  == (((m)[0]) ^ 0x02)) &&	\
	 (((a)->s6_addr[9])  == (m)[1]) &&		\
	 (((a)->s6_addr[10]) == (m)[2]) &&		\
	 (((a)->s6_addr[11]) == (m)[3]) &&		\
	 (((a)->s6_addr[12]) == (m)[4]) &&		\
	 (((a)->s6_addr[13]) == (m)[5]) &&		\
	 (((a)->s6_addr[14]) == (m)[6]) &&		\
	 (((a)->s6_addr[15]) == (m)[7]))

/* ipv6 address is unspecified */
#define is_addr_unspecified(a)		\
	((((a)->s6_addr32[0]) == 0) &&	\
	 (((a)->s6_addr32[1]) == 0) &&	\
	 (((a)->s6_addr32[2]) == 0) &&	\
	 (((a)->s6_addr32[3]) == 0))

/* compare ipv6 addresses prefixes */
#define ipaddr_prefixcmp(addr1, addr2, length) \
	(memcmp(addr1, addr2, length >> 3) == 0)

/* local link, i.e. FE80::/10 */
#define is_addr_link_local(a) (((a)->s6_addr16[0]) == htons(0xFE80))

/*
 * check whether we can compress the IID to 16 bits,
 * it's possible for unicast adresses with first 49 bits are zero only.
 */
#define lowpan_is_iid_16_bit_compressable(a)	\
	((((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr[10]) == 0) &&		\
	 (((a)->s6_addr[11]) == 0xff) &&	\
	 (((a)->s6_addr[12]) == 0xfe) &&	\
	 (((a)->s6_addr[13]) == 0))

/* multicast address */
#define is_addr_mcast(a) (((a)->s6_addr[0]) == 0xFF)

/* check whether the 112-bit gid of the multicast address is mappable to: */

/* 9 bits, for FF02::1 (all nodes) and FF02::2 (all routers) addresses only. */
#define lowpan_is_mcast_addr_compressable(a)	\
	((((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr16[5]) == 0) &&		\
	 (((a)->s6_addr16[6]) == 0) &&		\
	 (((a)->s6_addr[14])  == 0) &&		\
	 ((((a)->s6_addr[15]) == 1) || (((a)->s6_addr[15]) == 2)))

/* 48 bits, FFXX::00XX:XXXX:XXXX */
#define lowpan_is_mcast_addr_compressable48(a)	\
	((((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr[10]) == 0))

/* 32 bits, FFXX::00XX:XXXX */
#define lowpan_is_mcast_addr_compressable32(a)	\
	((((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr16[5]) == 0) &&		\
	 (((a)->s6_addr[12]) == 0))

/* 8 bits, FF02::00XX */
#define lowpan_is_mcast_addr_compressable8(a)	\
	((((a)->s6_addr[1])  == 2) &&		\
	 (((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr16[5]) == 0) &&		\
	 (((a)->s6_addr16[6]) == 0) &&		\
	 (((a)->s6_addr[14]) == 0))

#define lowpan_is_addr_broadcast(a)	\
	((((a)[0]) == 0xFF) &&	\
	 (((a)[1]) == 0xFF) &&	\
	 (((a)[2]) == 0xFF) &&	\
	 (((a)[3]) == 0xFF) &&	\
	 (((a)[4]) == 0xFF) &&	\
	 (((a)[5]) == 0xFF) &&	\
	 (((a)[6]) == 0xFF) &&	\
	 (((a)[7]) == 0xFF))

#define LOWPAN_DISPATCH_IPV6	0x41 /* 01000001 = 65 */
#define LOWPAN_DISPATCH_HC1	0x42 /* 01000010 = 66 */
#define LOWPAN_DISPATCH_IPHC	0x60 /* 011xxxxx = ... */
#define LOWPAN_DISPATCH_FRAG1	0xc0 /* 11000xxx */
#define LOWPAN_DISPATCH_FRAGN	0xe0 /* 11100xxx */

#define LOWPAN_DISPATCH_MASK	0xf8 /* 11111000 */

#define LOWPAN_FRAG_TIMEOUT	(HZ * 60)	/* time-out 60 sec */

#define LOWPAN_FRAG1_HEAD_SIZE	0x4
#define LOWPAN_FRAGN_HEAD_SIZE	0x5

/*
 * According IEEE802.15.4 standard:
 *   - MTU is 127 octets
 *   - maximum MHR size is 37 octets
 *   - MFR size is 2 octets
 *
 * so minimal payload size that we may guarantee is:
 *   MTU - MHR - MFR = 88 octets
 */
#define LOWPAN_FRAG_SIZE	88

/*
 * Values of fields within the IPHC encoding first byte
 * (C stands for compressed and I for inline)
 */
#define LOWPAN_IPHC_TF		0x18

#define LOWPAN_IPHC_FL_C	0x10
#define LOWPAN_IPHC_TC_C	0x08
#define LOWPAN_IPHC_NH_C	0x04
#define LOWPAN_IPHC_TTL_1	0x01
#define LOWPAN_IPHC_TTL_64	0x02
#define LOWPAN_IPHC_TTL_255	0x03
#define LOWPAN_IPHC_TTL_I	0x00


/* Values of fields within the IPHC encoding second byte */
#define LOWPAN_IPHC_CID		0x80

#define LOWPAN_IPHC_ADDR_00	0x00
#define LOWPAN_IPHC_ADDR_01	0x01
#define LOWPAN_IPHC_ADDR_02	0x02
#define LOWPAN_IPHC_ADDR_03	0x03

#define LOWPAN_IPHC_SAC		0x40
#define LOWPAN_IPHC_SAM		0x30

#define LOWPAN_IPHC_SAM_BIT	4

#define LOWPAN_IPHC_M		0x08
#define LOWPAN_IPHC_DAC		0x04
#define LOWPAN_IPHC_DAM_00	0x00
#define LOWPAN_IPHC_DAM_01	0x01
#define LOWPAN_IPHC_DAM_10	0x02
#define LOWPAN_IPHC_DAM_11	0x03

#define LOWPAN_IPHC_DAM_BIT	0
/*
 * LOWPAN_UDP encoding (works together with IPHC)
 */
#define LOWPAN_NHC_UDP_MASK		0xF8
#define LOWPAN_NHC_UDP_ID		0xF0
#define LOWPAN_NHC_UDP_CHECKSUMC	0x04
#define LOWPAN_NHC_UDP_CHECKSUMI	0x00

#define LOWPAN_NHC_UDP_4BIT_PORT	0xF0B0
#define LOWPAN_NHC_UDP_4BIT_MASK	0xFFF0
#define LOWPAN_NHC_UDP_8BIT_PORT	0xF000
#define LOWPAN_NHC_UDP_8BIT_MASK	0xFF00

/* values for port compression, _with checksum_ ie bit 5 set to 0 */
#define LOWPAN_NHC_UDP_CS_P_00	0xF0 /* all inline */
#define LOWPAN_NHC_UDP_CS_P_01	0xF1 /* source 16bit inline,
					dest = 0xF0 + 8 bit inline */
#define LOWPAN_NHC_UDP_CS_P_10	0xF2 /* source = 0xF0 + 8bit inline,
					dest = 16 bit inline */
#define LOWPAN_NHC_UDP_CS_P_11	0xF3 /* source & dest = 0xF0B + 4bit inline */
#define LOWPAN_NHC_UDP_CS_C	0x04 /* checksum elided */

#ifdef DEBUG
/* print data in line */
static inline void raw_dump_inline(const char *caller, char *msg,
				   unsigned char *buf, int len)
{
	if (msg)
		pr_debug("%s():%s: ", caller, msg);

	print_hex_dump(KERN_DEBUG, "", DUMP_PREFIX_NONE, 16, 1, buf, len, false);
}

/* print data in a table format:
 *
 * addr: xx xx xx xx xx xx
 * addr: xx xx xx xx xx xx
 * ...
 */
static inline void raw_dump_table(const char *caller, char *msg,
				  unsigned char *buf, int len)
{
	if (msg)
		pr_debug("%s():%s:\n", caller, msg);

	print_hex_dump(KERN_DEBUG, "\t", DUMP_PREFIX_OFFSET, 16, 1, buf, len, false);
}
#else
static inline void raw_dump_table(const char *caller, char *msg,
				  unsigned char *buf, int len) { }
static inline void raw_dump_inline(const char *caller, char *msg,
				   unsigned char *buf, int len) { }
#endif

static inline int lowpan_fetch_skb_u8(struct sk_buff *skb, u8 *val)
{
	if (unlikely(!pskb_may_pull(skb, 1)))
		return -EINVAL;

	*val = skb->data[0];
	skb_pull(skb, 1);

	return 0;
}

static inline int lowpan_fetch_skb_u16(struct sk_buff *skb, u16 *val)
{
	if (unlikely(!pskb_may_pull(skb, 2)))
		return -EINVAL;

	*val = (skb->data[0] << 8) | skb->data[1];
	skb_pull(skb, 2);

	return 0;
}

static inline bool lowpan_fetch_skb(struct sk_buff *skb,
		void *data, const unsigned int len)
{
	if (unlikely(!pskb_may_pull(skb, len)))
		return true;

	skb_copy_from_linear_data(skb, data, len);
	skb_pull(skb, len);

	return false;
}

static inline void lowpan_push_hc_data(u8 **hc_ptr, const void *data,
				       const size_t len)
{
	memcpy(*hc_ptr, data, len);
	*hc_ptr += len;
}

typedef int (*skb_delivery_cb)(struct sk_buff *skb, struct net_device *dev);

int lowpan_process_data(struct sk_buff *skb, struct net_device *dev,
		const u8 *saddr, const u8 saddr_type, const u8 saddr_len,
		const u8 *daddr, const u8 daddr_type, const u8 daddr_len,
		u8 iphc0, u8 iphc1, skb_delivery_cb skb_deliver);
int lowpan_header_compress(struct sk_buff *skb, struct net_device *dev,
			unsigned short type, const void *_daddr,
			const void *_saddr, unsigned int len);

#endif /* __6LOWPAN_H__ */
