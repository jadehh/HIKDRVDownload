#include <iostream>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#else __linux__
#include <unistd.h>
#endif
#include "HCNetSDK.h"

int main() {
#ifdef _WIN32
    // 设置控制台输出编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif
    // 初始化SDK
    if (!NET_DVR_Init()) {
        std::cerr << "SDK初始化失败!" << std::endl;
        return -1;
    }

    // 设备登录信息
    NET_DVR_DEVICEINFO_V30 deviceInfo = {0};

    // 登录设备
    LONG lUserID = NET_DVR_Login_V30(const_cast<char *>("192.168.29.110"), 8000, const_cast<char *>("admin"),
                                     const_cast<char *>("samples456"), &deviceInfo);
    if (lUserID < 0) {
        std::cerr << "登录失败，错误码: " << NET_DVR_GetLastError() << std::endl;
        NET_DVR_Cleanup();
        return -1;
    }

    std::cout << "登录成功!" << std::endl;

    // 输出设备详细信息
    std::cout << std::left << std::setw(20) << "设备序列号:" << deviceInfo.sSerialNumber << std::endl;
    std::cout << std::left << std::setw(20) << "报警输入数量:" << static_cast<int>(deviceInfo.byAlarmInPortNum) << std::endl;
    std::cout << std::left << std::setw(20) << "报警输出数量:" << static_cast<int>(deviceInfo.byAlarmOutPortNum) << std::endl;
    std::cout << std::left << std::setw(20) << "硬盘数量:" << static_cast<int>(deviceInfo.byDiskNum) << std::endl;
    std::cout << std::left << std::setw(20) << "设备类型:" << static_cast<int>(deviceInfo.byDVRType) << std::endl;
    std::cout << std::left << std::setw(20) << "模拟通道数量:" << static_cast<int>(deviceInfo.byChanNum) << std::endl;
    std::cout << std::left << std::setw(20) << "IP通道数量:" << static_cast<int>(deviceInfo.byIPChanNum) << std::endl;
    std::cout << std::left << std::setw(20) << "通道起始号:" << static_cast<int>(deviceInfo.byStartChan) << std::endl;
    std::cout << std::left << std::setw(20) << "IP通道起始号:" << static_cast<int>(deviceInfo.byStartDChan) << std::endl;

    // 获取第一个IP通道号
    int firstIPChannel = static_cast<int>(deviceInfo.byStartDChan);
    std::cout << std::left << std::setw(20) << "第一个IP通道号:" << firstIPChannel << std::endl;

    // 设置下载条件
    NET_DVR_PLAYCOND struDownloadCond = {0};
    struDownloadCond.dwChannel = firstIPChannel;
    struDownloadCond.struStartTime.dwYear = 2024;
    struDownloadCond.struStartTime.dwMonth = 12;
    struDownloadCond.struStartTime.dwDay = 31;
    struDownloadCond.struStartTime.dwHour = 0;
    struDownloadCond.struStartTime.dwMinute = 0;
    struDownloadCond.struStartTime.dwSecond = 0;
    struDownloadCond.struStopTime.dwYear = 2024;
    struDownloadCond.struStopTime.dwMonth = 12;
    struDownloadCond.struStopTime.dwDay = 31;
    struDownloadCond.struStopTime.dwHour = 0;
    struDownloadCond.struStopTime.dwMinute = 1;
    struDownloadCond.struStopTime.dwSecond = 20;

    // 获取视频文件
    LONG lDownloadHandle = NET_DVR_GetFileByTime_V40(lUserID, const_cast<char *>("download.mp4"), &struDownloadCond);
    if (lDownloadHandle < 0) {
        std::cerr << "获取视频文件失败，错误码: " << NET_DVR_GetLastError() << std::endl;
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -1;
    }

    // 开始下载
    if (!NET_DVR_PlayBackControl(lDownloadHandle, NET_DVR_PLAYSTART, 0, nullptr)) {
        std::cerr << "开始下载失败，错误码: " << NET_DVR_GetLastError() << std::endl;
        NET_DVR_StopGetFile(lDownloadHandle);
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -1;
    }

    std::cout << "正在下载视频文件..." << std::endl;

    // 等待下载完成
    int nPos = 0;
    while ((nPos = NET_DVR_GetDownloadPos(lDownloadHandle)) >= 0 && nPos < 100) {
#ifdef _WIN32
        Sleep(1000); // 每秒检查一次下载进度
#else __linux__
        sleep(1);
#endif
    }
    if (nPos < 0) {
        std::cerr << "下载过程中出错，错误码: " << NET_DVR_GetLastError() << std::endl;
        NET_DVR_StopGetFile(lDownloadHandle);
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();
        return -1;
    }

    std::cout << "下载完成!" << std::endl;

    // Keep the program running
    std::cout << "按回车键退出 ..." << std::endl;
    std::cin.get();

    // 停止下载
    NET_DVR_StopGetFile(lDownloadHandle);

    // 注销设备
    NET_DVR_Logout(lUserID);

    // 清理SDK资源
    NET_DVR_Cleanup();



    return 0;
}