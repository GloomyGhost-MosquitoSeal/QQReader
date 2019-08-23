/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/
#include <iostream>
#include "stdafx.h"
#include "sapi.h"
#include "sphelper.h"
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载

using namespace std;

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;


LPWSTR StringToWString(const std::string& s)
{
	int nLength = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, NULL);
	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength);
	return lpwszStr;
}

std::string msgtospeech(int64_t *fromGroup, int64_t *fromQQ, const char *msg) {
	const std::string& fromQQ1 = to_string(static_cast<int>(static_cast<double>(*fromQQ)));
	const std::string& fromGroup1 = to_string(static_cast<int>(static_cast<double>(*fromGroup)));
	const std::string& msg1 = msg;

	if (static_cast<int>(static_cast<double>(*fromGroup)) == 0) {
		return "来自：" + fromQQ1 + "的：" + msg1;
	}
	else
	{
		return "来自：" + fromGroup1 + "的：" +  fromQQ1 + "的：" + msg1;
	}
}


void synthesis(const std::string& s)
{
	::CoInitialize(NULL);             // COM初始化
	CLSID CLSID_SpVoice;
	CLSIDFromProgID(_T("SAPI.SpVoice"), &CLSID_SpVoice);
	ISpVoice* pSpVoice = NULL;
	IEnumSpObjectTokens* pSpEnumTokens = NULL;
	CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)& pSpVoice);
	SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pSpEnumTokens);
	ULONG ulTokensNumber = 0;
	pSpEnumTokens->GetCount(&ulTokensNumber);
	ISpObjectToken* pSpToken = NULL;
	pSpEnumTokens->Item(2, &pSpToken);//上面代码可以知道语音包的序号多少
	pSpVoice->SetVoice(pSpToken);

	pSpVoice->Speak(StringToWString(s), SPF_DEFAULT, NULL);     // 朗读中文和英文的混合字符串
	pSpVoice->Release();
	pSpEnumTokens->Release();
	::CoUninitialize();
}

/* 
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	return 0;
}


/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {
	int64_t fromGroup = 0;
	synthesis(msgtospeech(&fromGroup, &fromQQ, msg));
	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
	return EVENT_BLOCK;
}


/*
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	synthesis(msgtospeech(&fromGroup, &fromQQ, msg));
	return EVENT_BLOCK; //关于返回值说明, 见“_eventPrivateMsg”函数
}
