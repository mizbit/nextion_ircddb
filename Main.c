/*
 *  Copyright (C) 2018-2020 by Yosh Todo JE3HCZ
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  本プログラムはD-STAR Network の一部の機能を実現するための物で、
 *  アマチュア無線の技術習得とその本来の用途以外では使用しないでください。
 *
 */


#include	"Nextion.h"
#define		WAITTIME 500000	// microsec

int main(int argc, char *argv[])
{
	char	*ret;
	int	fd;
	int	i;
	int	flag;
	char	command[64]	= {'\0'};
	char	usercmd[32]	= {'\0'};
	char	tmpstr[32]	= {'\0'};

	/* GPIO シリアルポートのオープン*/
	fd = openport(SERIALPORT, BAUDRATE);

	/* 環境設定ファイルの読み取り */
	sendcmd("dim=dims");
	getconfig();

	/* 送・受信ループ */
	while (1)
	{

		/*
		 * 受信処理
		 */

		/* タッチパネルのデータを読み込む */
		recvdata(usercmd);

		/* コマンドをスイッチに振り分ける */
		if (strncmp(usercmd, "restart",  7) == 0) flag = 1;
		if (strncmp(usercmd, "reboot",   6) == 0) flag = 2;
		if (strncmp(usercmd, "shutdown", 8) == 0) flag = 3;

		switch (flag)
		{
			case 1:
				sendcmd("dim=10");
				system("sudo systemctl restart ircddbgateway.service");
				system("sudo systemctl restart nextion.service");
				break;

			case 2:
				sendcmd("dim=10");
				system("sudo shutdown -r now");
				break;

			case 3:
				sendcmd("dim=10");
				system("sudo shutdown -h now");
				break;

			default:
				break;
		}

		/*
		 * 送信処理
		 */

		/* 日付･時刻表示 */
		jstimer = time(NULL);
		jstimeptr = localtime(&jstimer);
		strftime(tmpstr, sizeof(tmpstr), "%Y.%m.%d %H:%M:%S ", jstimeptr);
		sprintf(command, "MAIN.t11.txt=\"%s\"", tmpstr);
		sendcmd(command);

		/* CPU 温度の表示 */
		dispcmdinfo();

		/* ログステータスの読み取り */
		if (strlen(chkversion) == 0) disploginfo();

		/* ストリームよりDStarRepeater のバージョンを取得 */
//		dispstreaminfo();

		usleep(WAITTIME);
	}

	/* GPIO シリアルポートのクローズ*/
	close(fd);
	return (EXIT_SUCCESS);
}
