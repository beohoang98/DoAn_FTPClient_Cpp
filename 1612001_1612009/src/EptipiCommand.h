#pragma once

#include "stdafx.h"
#include <string>
#include <map>

namespace FTPCommand
{
	struct cmdDescription {
		std::string title, description;
	};
	const std::map<std::string, cmdDescription> listCmd = {
		{
			"open",
			cmdDescription{
			"Ket noi server",

			"open [server name] - ket noi den dia chi [server name]"
		}
		},
		{
			"help",
			cmdDescription{
			"Hien thi thong tin cua ham",

			"help - thong tin tat ca ca ham\n"
			"help [cmd] - thong tin cho ham [cmd]"
		}
		},
		{
			"dir",
			cmdDescription{
			"liet ke chi tiet thu muc tren server",
			""
		}
		},
		{
			"ls",
			cmdDescription{
			"liet ke ten cac thu muc tren server",
			""
		}
		},
		{
			"ldir",
			cmdDescription{
			"liet ke chi tiet thu muc client",
			""
		}
		},
		{
			"lls",
			cmdDescription{
			"liet ke ten thu muc client",
			""
		}
		},
		{
			"cd",
			cmdDescription{
			"thay doi duong dan tren server",
			"\tcd [duong dan] de thay doi working dir den [duong dan]"
		}
		},
		{
			"lcd",
			cmdDescription{
			"thay doi duong dan o client",
			"\tlcd [duong dan] de thay doi working dir den [duong dan]"
		}
		},
		{
			"pwd",
			cmdDescription{
			"in ra duong dan hien tai tren server",
			""
		}
		},
		{
			"get",
			cmdDescription{
			"download file ve client",

			"\tget [ten file] - de download file [ten file] tren )\n"
			"\t\tserver ve path hien tai cua client"
		}
		},
		{
			"mget",
			cmdDescription{
			"mget [expr] - download nhieu file thoa man [expr]",

			"\t[expr] co the la:\n"
			"\t- *.txt\n"
			"\t- folder/*.*\n"
			"\t..."
		}
		},
		{
			"put",
			cmdDescription{
			"put [asd] - upload file [asd] len server",
			""
		}
		},
		{
			"mput",
			cmdDescription{
			"upload nhieu file",
			""
		}
		},
		{
			"mode",
			cmdDescription{
			"set mode to BINARY or ASCII",

			"\tA - ASCII mode\n"
			"\tI - BINARY mode"
		}
		},
		{
			"quit",
			cmdDescription{
			"thoat ftp",
			""
		}
		},
		{
			"del",
			cmdDescription{
			"Xoa file tren server",

			"\tmdel [ten-file] - xoa file [ten-file]"
		}
		},
		{
			"mdel",
			cmdDescription{
			"Xoa nhieu file tren server",

			"\tmdel [expr...] - xoa file thoa man [expr...]\n"
			"\t- mdel *.txt *.asd - xoa het file .txt va .asd\n"
			"\t- ..."
		}
		},
		{
			"rmdir",
			cmdDescription{
			"Xoa thu muc trong",

			"\trmdir [tenfolder] - Xoa thu muc [tenfolder]\n"
			"\t- Neu thu muc khong rong, khong xoa dc"
		}
		},
		{
			"mkdir",
			cmdDescription{
			"Tao thu muc moi",

			"\tmkdir [tenfolder] - Tao thu muc moi co ten [tenfolder]"
		}
		},
		{
			"bye",
			cmdDescription({
				"Logout ra khoi server",
				""
			})
		},
		{
			"disconnect",
			cmdDescription({
				"Ngat ket noi voi server",

				""
			})
		}
	};
}