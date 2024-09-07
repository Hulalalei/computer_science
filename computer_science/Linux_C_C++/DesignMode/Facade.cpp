#include <iostream>
#include <algorithm>


//子系统类-音频播放器
class AudioPlayer {
public:
    void playAudio(const std::string& filePath) {
        std::cout << "播放音频：" << filePath << std::endl;
        //实际音频播放逻辑
    }
};

//子系统类-视频播放器
class VideoPlayer {
public:
    void playVideo(const std::string& filePath) {
        std::cout << "播放视频：" << filePath << std::endl;
        //实际视频播放逻辑
    }
};

//子系统类-字幕播放器
class SubtitlePlayer {
public:
    void playVideo(const std::string& filePath) {
        std::cout << "播放字幕：" << filePath << std::endl;
        //实际字幕播放逻辑
    }
};

//外观类--多媒体播放器
class MultimediaPlayer {
public:
    MultimediaPlayer() {
        //初始化子系统
    }

    //提供一个播放多媒体文件的简化接口
    void palyMedia(const std::string& mediaFilePath) {
        audioPlayer.playAudio(mediaFilePath);
        videoPlayer.playVideo(mediaFilePath);
        subtitlePlayer.playVideo(mediaFilePath);
    }
private:
    AudioPlayer audioPlayer;
    VideoPlayer videoPlayer;
    SubtitlePlayer subtitlePlayer;
};

int main (void) {
    MultimediaPlayer multi;
    multi.palyMedia("小鲤鱼历险记");
    
    
    return 0;
}