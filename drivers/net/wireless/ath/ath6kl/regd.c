
/*
 * Copyright (c) 2012 Qualcomm Atheros, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/moduleparam.h>
#include "regd.h"
#if 1 /* 20130402 country code */
#include "debug.h"
#include <linux/vmalloc.h>
#endif /* 20130402 */

static unsigned short reg_domain = 0xffff;
module_param(reg_domain, ushort, 0644);

/*
 * Number of bytes in board data that we are interested
 * in while setting regulatory domain from host
 */
#define REG_DMN_BOARD_DATA_LEN	16

/* Modifies regulatory domain in board data in target RAM */
int ath6kl_set_reg_dmn(struct ath6kl *ar)
{
	u8 buf[REG_DMN_BOARD_DATA_LEN];
	__le16 old_sum, old_ver, old_rd, old_rd_next;
	__le32 brd_dat_addr = 0, new_sum, new_rd;
	int ret;

	if (reg_domain == 0xffff)
		return 0;

	ret = ath6kl_bmi_read(ar, AR6003_BOARD_DATA_ADDR,
			      (u8 *)&brd_dat_addr, 4);
	if (ret)
		return ret;

	memset(buf, 0, sizeof(buf));
	ret = ath6kl_bmi_read(ar, brd_dat_addr, buf, sizeof(buf));
	if (ret)
		return ret;

	memcpy((u8 *)&old_sum, buf + AR6003_BOARD_DATA_OFFSET, 2);
	memcpy((u8 *)&old_ver, buf + AR6003_BOARD_DATA_OFFSET + 2, 2);
	memcpy((u8 *)&old_rd, buf + AR6003_RD_OFFSET, 2);
	memcpy((u8 *)&old_rd_next, buf + AR6003_RD_OFFSET + 2, 2);

	/*
	 * Overwrite the new regulatory domain and preserve the
	 * MAC addr which is in the same word.
	 */
	new_rd = cpu_to_le32((le32_to_cpu(old_rd_next) << 16) + reg_domain);
	ret = ath6kl_bmi_write(ar,
		cpu_to_le32(le32_to_cpu(brd_dat_addr) + AR6003_RD_OFFSET),
		(u8 *)&new_rd, 4);
	if (ret)
		return ret;

	/*
	 * Recompute the board data checksum with the new regulatory
	 * domain, preserve the version information which is in the
	 * same word.
	 */
	new_sum = cpu_to_le32((le32_to_cpu(old_ver) << 16) +
			      (le32_to_cpu(old_sum) ^ le32_to_cpu(old_rd) ^
			       reg_domain));
	ret = ath6kl_bmi_write(ar,
		cpu_to_le32(le32_to_cpu(brd_dat_addr) +
		AR6003_BOARD_DATA_OFFSET),
		(u8 *)&new_sum, 4);

	return ret;
}
#if 1 /* 20130402 country code */
#define COUNTRY_MAX 134
struct channels
{
	char country[30];
	char ccode[6];
	unsigned short reg_dmn_code;
} pArray[COUNTRY_MAX] = {
	{"Afghanistan",		"AF",	0x406a},	//WWR
	{"Albania",			"AL",	0x8008},
	{"Argentina",		"AR",	0x8020},
	{"Armenia",			"AM",	0x8033},
	{"Aruba",			"AW",	0x8215},
	{"Algeria",			"DZ",	0x800c},
	{"Angola",			"AD",	0x406a},	//WWR
	{"Austria",			"AT",	0x8028},
	{"Australia",		"AU",	0x8024},
	{"Azerbaijan",		"AZ",	0x801f},
	{"Baharain",		"BH",	0x8030},
	{"Bangladesh",		"BD",	0x8032},
	{"Barbados",		"BB",	0x8034},
	{"Belarus",			"BY",	0x8070},
	{"Belgium",			"BE",	0x8038},
	{"Belize",			"BZ",	0x8054},
	{"Bolvia",			"BO",	0x8044},
	{"Brazil",			"BR",	0x804c},
	{"Bosnia",			"BA",	0x8046},
	{"Brunel",			"BN",	0x8060},
	{"Bulgaria",		"BG",	0x8064},
	{"Cambodia",		"KH",	0x8074},
	{"Canada",			"CA",	0x807c},
	{"Chile",			"CL",	0x8098},
	{"China",			"CN",	0x809c},
	{"Colombia",		"CO",	0x80aa},
	{"Costa Rica",		"CR",	0x80bc},
	{"Croatia",			"HR",	0x80bf},
	{"Cyprus",			"CY",	0x80cb},
	{"Greece",			"GR",	0x812c},
	{"Czech Republic",	"CZ",	0x80cb},
	{"Denmark",			"DK",	0x80d0},
	{"Dominican",		"DO",	0x80d6},
	{"Ecuador",			"EC",	0x80da},
	{"Egypt",			"EG",	0x8332},
	{"El Salvador",		"SV",	0x80de},
	{"Estonia",			"EE",	0x80e9},
	{"Nordic",			"FI",	0x80f6},
	{"Finland",			"FI",	0x80f6},
	{"France",			"FR",	0x80FA},
	{"Gabon",			"GA",	0x406a},	//WWR
	{"Georgla",			"GE",	0x810c},
	{"Germany",			"DE",	0x8114},
	{"Ghana",			"GH",	0x406a},	//WWR
	{"Greece",			"GR",	0x812c},
	{"Greenland",		"GL",	0x8130},
	{"Grenada",			"GD",	0x8134},
	{"Guam",			"GU",	0x813c},
	{"Guatemala",		"GT",	0x8140},
	{"Haiti",			"HT",	0x814c},
	{"Honduras",		"HN",	0x8154},
	{"Hong kong",		"HK",	0x8158},
	{"Hungary",			"HU",	0x815c},
	{"Iceland",			"IS",	0x8160},
	{"India",			"IN",	0x8164},
	{"Indonesia",		"ID",	0x8168},
	{"Iraq",			"IQ",	0x406a},	//WWR
	{"Iran",			"IR",	0x816c},
	{"IE",				"IE",	0x8174},
	{"Ireland",			"IE",	0x8174},
	{"Israel",			"IL",	0x8178},
	{"Italy",			"IT",	0x817c},
	{"Jamaica",			"JM",	0x8184},
	{"Jordan",			"JO",	0x8190},
	{"Central Asia",	"KZ",	0x818e},
	{"Kazakhstan",		"KZ",	0x818e},
	{"Kenya",			"KE",	0x8198},
	{"North korea",		"KP",	0x8198},
	{"Kuwait",			"KW",	0x819e},
	{"BALTIC",			"LV",	0x81ac},
	{"Libya",			"LY",	0x406a},	//WWR
	{"Lebanon",			"LB",	0x81a6},
	{"Liechtenstein",	"LI",	0x81b6},
	{"Lithuania",		"LT",	0x81b8},
	{"Luxemburg",		"LU",	0x81ba},
	{"Macau sar",		"Mo",	0x81be},
	{"Macedonia",		"MK",	0x8327},
	{"Malaysia",		"MY",	0x81ca},
	{"Mongolia",		"MN",	0x406a},	//WWR
	{"Malta",			"MT",	0x81d6},
	{"Mexico",			"MX",	0x81e4},
	{"Monaco",			"MC",	0x81ec},
	{"Netherlands",		"AN",	0x8212},
	{"Montenegro",		"ME",	0x837b},
	{"Morocco",			"MA",	0x81f8},
	{"Nepal",			"NP",	0x820c},
	{"Netherlands",		"NL",	0x8210},
	{"The Netherlands",	"NL",	0x8210},
	{"New Zealand",		"NZ",	0x822a},
	{"Nigeria",			"NG",	0x406a},	//WWR
	{"Norway",			"NO",	0x8242},
	{"Oman",			"OM",	0x8200},
	{"Pakistan",		"PK",	0x824a},
	{"Panama",			"PA",	0x824f},
	{"Peru",			"PE",	0x825c},
	{"Philippines",		"PH",	0x8260},
	{"Poland",			"PL",	0x8268},
	{"Portugal",		"PT",	0x826c},
	{"Puerto rico",		"PR",	0x8276},
	{"Qatar",			"QA",	0x827a},
	{"Romania",			"RO",	0x8282},
	{"Russia",			"RU",	0x8283},
	{"Rwanda",			"RW",	0x8286},
	{"KSA",				"SA",	0x82aa},
	{"Senegal",			"SN",	0x406a},	//WWR
	{"Serbia",			"RS",	0x8114},
	{"Montenegro",		"ME",	0x81f3},
	{"Singapore",		"SG",	0x82be},
	{"Slovakia",		"SK",	0x82bf},
	{"Slovenia",		"SI",	0x82c1},
	{"South Africa",	"ZA",	0x82c6},
	{"Spain",			"ES",	0x82D4},
	{"Sri Lanka",		"LK",	0x8090},
	{"Sweden",			"SE",	0x82f0},
	{"Switzerland",		"CH",	0x82f4},
	{"Syrla",			"SY",	0x82f8},
	{"Taiwan",			"TW",	0x809e},
	{"Thailand",		"TH",	0x82fc},
	{"Trinidad",		"TT",	0x830c},
	{"Tunisia",			"TN",	0x8314},
	{"Turkey",			"TR",	0x8318},
	{"UAE",				"AE",	0x8310},
	{"Ukraine",			"UA",	0x8324},
	{"UK",				"GB",	0x833A},
	{"UK &IRE",			"GB",	0x833A},
	{"United Kingdom",	"GB",	0x833A},
	{"US",				"US",	0x8348},
	{"Uruguay",			"UY",	0x835a},
	{"Uzbekistan",		"UZ",	0x835c},
	{"Venezuela",		"VE",	0x835e},
	{"Vietnam",			"VN",	0x82c0},
	{"Yemen",			"TE",	0x8377},
	{"Zimbabwe",		"ZW",	0x82cc},
	{"Japan",			"JP",	0x0088}
};

static unsigned short ath6kl_get_reg_dmn_code(u8 *ccode)
{
	int i = 0;

	for (i=0; i < COUNTRY_MAX; i++) {
		if (strncmp(ccode, pArray[i].ccode, 2) == 0) {
			ath6kl_dbg(ATH6KL_DBG_BOOT, "%s: %s %s 0x%x",
				__func__, pArray[i].country, pArray[i].ccode, pArray[i].reg_dmn_code);
			return pArray[i].reg_dmn_code;
		}
	}
	ath6kl_err("%s() Failed to find reg_domain code for %s\n", __func__, ccode);
	/*
	* if ccode is not included in country code,
	* the default country code (GB) is returned.
	*/
	return 0x833a;
}

void ath6kl_update_ccodeinfo(struct ath6kl *ar)
{
	char ccode_filename[32];

	do {
		int ret = 0;
		size_t length;
		u8 *pdata = NULL;
		snprintf(ccode_filename, sizeof(ccode_filename), "/data/.ccode.info");
		ret = android_readwrite_file(ccode_filename, NULL, NULL, 0);
		if (ret < 0)
			break;
		else
			length = ret;

		pdata = vmalloc(length + 1);

		if (!pdata) {
			ath6kl_dbg(ATH6KL_DBG_BOOT,
				"%s: Cannot allocate buffer for ccode_info (%d)\n",
				__func__, length);
			break;
		}

		if (android_readwrite_file(ccode_filename,
				(char *)pdata, NULL, length) != length) {
			ath6kl_dbg(ATH6KL_DBG_BOOT,
				"%s: file read error, length %d\n",
				__func__, length);
			vfree(pdata);
			break;
		}
		pdata[length] = '\0';

		if (reg_domain == 0xffff) {
			reg_domain = ath6kl_get_reg_dmn_code(pdata);
		}

		vfree(pdata);
	} while (0);
}
#endif /* 20130402 */
