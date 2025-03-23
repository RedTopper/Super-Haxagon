// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "MemoryFS.hpp"

// This section SHOULD be code generated
extern unsigned char romfs_levels_haxagon[];
extern unsigned int romfs_levels_haxagon_len;

extern unsigned char romfs_bgm_callMeKatla_txt[];
extern unsigned int romfs_bgm_callMeKatla_txt_len;

extern unsigned char romfs_bgm_captainCool_txt[];
extern unsigned int romfs_bgm_captainCool_txt_len;

extern unsigned char romfs_bgm_commandoSteve_txt[];
extern unsigned int romfs_bgm_commandoSteve_txt_len;

extern unsigned char romfs_bgm_drFinkelfracken_txt[];
extern unsigned int romfs_bgm_drFinkelfracken_txt_len;

extern unsigned char romfs_bgm_esiannoyamFoEzam_txt[];
extern unsigned int romfs_bgm_esiannoyamFoEzam_txt_len;

extern unsigned char romfs_bgm_jackRussel_txt[];
extern unsigned int romfs_bgm_jackRussel_txt_len;

extern unsigned char romfs_bgm_mazeOfMayonnaise_txt[];
extern unsigned int romfs_bgm_mazeOfMayonnaise_txt_len;

extern unsigned char romfs_bgm_screenSaver_txt[];
extern unsigned int romfs_bgm_screenSaver_txt_len;
// End section

namespace SuperHaxagon {
	struct MemoryBuffer : std::streambuf {
		MemoryBuffer(unsigned char* base, const size_t size) {
			auto* const p(reinterpret_cast<char*>(base));
			this->setg(p, p, p + size);
		}
	};

	struct MemoryStream final : virtual MemoryBuffer, std::istream {
		MemoryStream(unsigned char* base, const size_t size) :
			MemoryBuffer(base, size),
			std::istream(static_cast<std::streambuf*>(this)) {
		}
	};
	
	std::unique_ptr<std::istream> MemoryFS::openFile(const std::string& partial) {
		// This section SHOULD be code generated
		if (partial == "/levels.haxagon") {
			return std::make_unique<MemoryStream>(&romfs_levels_haxagon[0], romfs_levels_haxagon_len);
		}

		if (partial == "/bgm/callMeKatla.txt") {
			return std::make_unique<MemoryStream>(&romfs_bgm_callMeKatla_txt[0], romfs_bgm_callMeKatla_txt_len);
		}

		if (partial == "/bgm/captainCool.txt") {
			return std::make_unique<MemoryStream>(&romfs_bgm_captainCool_txt[0], romfs_bgm_captainCool_txt_len);
		}

		if (partial == "/bgm/commandoSteve.txt") {
			return std::make_unique<MemoryStream>(&romfs_bgm_commandoSteve_txt[0], romfs_bgm_commandoSteve_txt_len);
		}

		if (partial == "/bgm/drFinkelfracken.txt") {
			return std::make_unique<MemoryStream>(&romfs_bgm_drFinkelfracken_txt[0], romfs_bgm_drFinkelfracken_txt_len);
		}

		if (partial == "/bgm/esiannoyamFoEzam.txt") {
			return std::make_unique<MemoryStream>(&romfs_bgm_esiannoyamFoEzam_txt[0], romfs_bgm_esiannoyamFoEzam_txt_len);
		}

		if (partial == "/bgm/jackRussel.txt") {
			return std::make_unique<MemoryStream>(&romfs_bgm_jackRussel_txt[0], romfs_bgm_jackRussel_txt_len);
		}

		if (partial == "/bgm/mazeOfMayonnaise.txt") {
			return std::make_unique<MemoryStream>(&romfs_bgm_mazeOfMayonnaise_txt[0], romfs_bgm_mazeOfMayonnaise_txt_len);
		}

		if (partial == "/bgm/screenSaver.txt") {
			return std::make_unique<MemoryStream>(&romfs_bgm_screenSaver_txt[0], romfs_bgm_screenSaver_txt_len);
		}
		// End section

		return nullptr;
	}
}
