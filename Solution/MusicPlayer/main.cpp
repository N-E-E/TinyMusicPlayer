#include "kmusicplayer.h"
//#include "kmusicmodule.h"
//#include "klyricmodule.h"
#include "khttpdownloader.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KMusicPlayer w;
    w.show();

    /*KMusicModule* musicModule = new KMusicModule(nullptr);
    musicModule->play("./music.mp3");*/

    /*KLyricDecoder* lyricDecoder = new KLyricDecoder(nullptr);
    lyricDecoder->open(u8"./local/孤勇者-陈奕迅/lyric.txt");*/

    //qDebug() << QSslSocket::sslLibraryBuildVersionString();

    // test
    /*KHttpMusicDownloader* downloader = new KHttpMusicDownloader(nullptr);
    downloader->download("https://gitee.com/MarkYangUp/music/raw/master/%E9%A3%8E%E9%9B%A8%E6%97%A0%E9%98%BB/%E5%91%A8%E5%8D%8E%E5%81%A5/music.mp3");*/
    /*KHttpDownloader downloader(nullptr);
    downloader.download("https://gitee.com/MarkYangUp/music/raw/master/风雨无阻/周华健/music.mp3", "./test1.mp3");*/
    //while (downloader.getDownloadState() == true){}
    //downloader.save("./testchinese.json");
    return a.exec();
}
